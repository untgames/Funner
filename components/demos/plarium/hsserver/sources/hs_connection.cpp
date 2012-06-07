#include "shared.h"

using namespace plarium::hsserver;
using namespace plarium::system;
using namespace plarium::utility;

namespace
{

const char*         PING_MESSAGE                = "[{\"event\":\"user.ping\"}]";
const size_t        DEQUEUE_TIMEOUT             = 100;
const size_t        DEFAULT_RECEIVE_BUFFER_SIZE = 1024;
const size_t        DEFAULT_SEND_BUFFER_SIZE    = 1024;
const size_t        COMPRESSED_LENGTH_SIZE      = 4;
const size_t        HMAC_SIZE                   = 32;
const size_t        IV_SIZE                     = 16;
const unsigned char PACKET_HEADER               = 0x06;
const unsigned char OPTIONS_COMPRESSED          = 0x01;
const unsigned char OPTIONS_ENCRYPTED           = 0x02;

struct Message
{
  unsigned short plugin_id;
  std::string    message;

  Message (unsigned short in_plugin_id, const unsigned char* in_message, size_t size)
    : plugin_id (in_plugin_id)
  {
    message.resize (size);

    memcpy (&message [0], in_message, size);
  }
};

typedef SharedQueue<Message> MessageQueue;

//Send-thread function listener
class IBackgroundThreadListener
{
  public:
    //Connection established
    virtual void OnConnected () = 0;

    //Connection failed
    virtual void OnConnectFailed (const char* error) = 0;

    // Message received
    virtual void OnMessageReceived (unsigned short plugin_id, const unsigned char* message, size_t size) = 0;

    //Error occured
    virtual void OnError (ErrorCode code, const char* error) = 0;

    //Logging
    virtual void Log (const char* message) = 0;

  protected:
    virtual ~IBackgroundThreadListener () {}
};

//Send-thread function arguments
struct SendFunctionArg
{
  std::string                host;
  unsigned short             port;
  HsConnectionSettings&      connection_settings;
  TcpClient&                 tcp_client;
  MessageQueue&              message_queue;
  volatile bool&             need_stop;
  IBackgroundThreadListener& listener;

  SendFunctionArg (const char* in_host, unsigned short in_port, HsConnectionSettings& in_connection_settings,
                   TcpClient& in_tcp_client, MessageQueue& in_message_queue, volatile bool& in_need_stop,
                   IBackgroundThreadListener& in_listener)
    : host (in_host)
    , port (in_port)
    , connection_settings (in_connection_settings)
    , tcp_client (in_tcp_client)
    , message_queue (in_message_queue)
    , need_stop (in_need_stop)
    , listener (in_listener)
    {}
};

//Receive-thread function arguments
struct ReceiveFunctionArg
{
  HsConnectionSettings&      connection_settings;
  TcpClient&                 tcp_client;
  volatile bool&             need_stop;
  IBackgroundThreadListener& listener;

  ReceiveFunctionArg (HsConnectionSettings& in_connection_settings, TcpClient& in_tcp_client, volatile bool& in_need_stop, IBackgroundThreadListener& in_listener)
    : connection_settings (in_connection_settings)
    , tcp_client (in_tcp_client)
    , need_stop (in_need_stop)
    , listener (in_listener)
    {}
};

//Receive-thread  function
void* receive_function (void* user_data)
{
  std::auto_ptr<ReceiveFunctionArg> data ((ReceiveFunctionArg*)user_data);

  volatile bool& need_stop = data->need_stop;

  size_t         current_buffer_size            = DEFAULT_RECEIVE_BUFFER_SIZE;
  unsigned char* receive_buffer                 = new unsigned char [current_buffer_size];
  size_t         current_decompress_buffer_size = DEFAULT_RECEIVE_BUFFER_SIZE;
  unsigned char* decompress_buffer              = new unsigned char [current_decompress_buffer_size];
  unsigned char  packet_header_buffer [8];
  unsigned char  hashed_key [32];

  if (data->connection_settings.encryption_enabled)
    sha256 (data->connection_settings.encryption_key, data->connection_settings.encryption_key_bits / 8, hashed_key);

  for (;;)
  {
    if (need_stop)
      break;

    try
    {
      //TODO timeout error
      //TODO receive with timeout

      data->tcp_client.Receive (packet_header_buffer, sizeof (packet_header_buffer));

      if (packet_header_buffer [0] != PACKET_HEADER)
      {
        data->listener.OnError (ErrorCode_InvalidHeader, "Invalid package header");
        break;
      }

      unsigned char   options      = packet_header_buffer [1];
      unsigned short* plugin_id    = (unsigned short*)(packet_header_buffer + 2);
      unsigned int*   message_size = (unsigned int*)(packet_header_buffer + 4);

      if (*message_size > current_buffer_size)
      {
        delete [] receive_buffer;

        receive_buffer      = 0;
        current_buffer_size = 0;

        size_t new_buffer_size = *message_size * 2;

        receive_buffer      = new unsigned char [new_buffer_size];
        current_buffer_size = new_buffer_size;
      }

      if (options & OPTIONS_ENCRYPTED)
      {
        if (*message_size < HMAC_SIZE + IV_SIZE)
        {
          data->listener.OnError (ErrorCode_InvalidHeader, "Invalid message size");
          break;
        }
      }

      if (options & OPTIONS_COMPRESSED)
      {
        if (*message_size < COMPRESSED_LENGTH_SIZE)
        {
          data->listener.OnError (ErrorCode_InvalidHeader, "Invalid message size");
          break;
        }
      }

      data->tcp_client.Receive (receive_buffer, *message_size);

      unsigned char* message_body      = receive_buffer;
      size_t         message_body_size = *message_size;

      if (options & OPTIONS_ENCRYPTED)
      {
        unsigned char hmac [HMAC_SIZE];

        hmac_sha256 (hashed_key, sizeof (hashed_key), receive_buffer + HMAC_SIZE, *message_size - HMAC_SIZE, hmac);

        if (memcmp (hmac, receive_buffer, HMAC_SIZE))
        {
          data->listener.OnError (ErrorCode_HashMismatch, "Invalid hash");
          break;
        }

        unsigned char* iv = receive_buffer + HMAC_SIZE;

        message_body = iv + IV_SIZE;

        message_body_size -= HMAC_SIZE + IV_SIZE;

        aes_ofb (data->connection_settings.encryption_key, data->connection_settings.encryption_key_bits, message_body_size, message_body, message_body, (const unsigned char (&)[16])*iv);
      }

      if (options & OPTIONS_COMPRESSED)
      {
        if (message_body_size < COMPRESSED_LENGTH_SIZE)
        {
          data->listener.OnError (ErrorCode_InvalidHeader, "Invalid message size");
          break;
        }

        unsigned int* uncompressed_size = (unsigned int*)message_body;

        message_body += COMPRESSED_LENGTH_SIZE;

        if (*uncompressed_size > current_decompress_buffer_size)
        {
          delete [] decompress_buffer;

          decompress_buffer              = 0;
          current_decompress_buffer_size = 0;

          size_t new_buffer_size = *uncompressed_size * 2;

          decompress_buffer              = new unsigned char [new_buffer_size];
          current_decompress_buffer_size = new_buffer_size;
        }

        zlib_decompress (message_body, message_body_size - COMPRESSED_LENGTH_SIZE, decompress_buffer, *uncompressed_size);

        message_body      = decompress_buffer;
        message_body_size = *uncompressed_size;
      }

      data->listener.OnMessageReceived (*plugin_id, message_body, message_body_size);
      data->listener.Log (format ("Message received '%s'", message_body).c_str ());
    }
    catch (std::exception& e)
    {
      data->listener.OnError (ErrorCode_Generic, e.what ());
      break;
    }
    catch (...)
    {
      data->listener.OnError (ErrorCode_Generic, "Unknown error while receiving events");
      break;
    }
  }

  delete [] receive_buffer;
  delete [] decompress_buffer;

  return 0;
}

//Send-thread function
void* send_function (void* user_data)
{
  std::auto_ptr<SendFunctionArg> data ((SendFunctionArg*)user_data);

  volatile bool& need_stop = data->need_stop;

  try
  {
    data->tcp_client.Connect (data->host.c_str (), data->port);
  }
  catch (std::exception& e)
  {
    data->listener.OnConnectFailed (e.what ());
    return 0;
  }
  catch (...)
  {
    data->listener.OnConnectFailed ("Uknown exception");
    return 0;
  }

  data->listener.OnConnected ();

  std::auto_ptr<ReceiveFunctionArg> receive_function_arg (new ReceiveFunctionArg (data->connection_settings, data->tcp_client, data->need_stop, data->listener));

  Thread receive_thread (receive_function, receive_function_arg.get ());      //thread for receiving data

  receive_function_arg.release ();

  size_t last_keep_alive_time = milliseconds ();

  size_t         current_buffer_size = DEFAULT_SEND_BUFFER_SIZE;
  unsigned char* send_buffer         = new unsigned char [current_buffer_size];
  unsigned char  hashed_key [32];

  if (data->connection_settings.encryption_enabled)
    sha256 (data->connection_settings.encryption_key, data->connection_settings.encryption_key_bits / 8, hashed_key);

  for (;;)
  {
    if (need_stop)
      break;

    try
    {
      std::auto_ptr<Message> message = data->message_queue.Dequeue (DEQUEUE_TIMEOUT);

      if (!message.get ())
      {
        if (milliseconds () - last_keep_alive_time > data->connection_settings.keep_alive_interval)
        {
          std::auto_ptr<Message> ping_message (new Message (1, (const unsigned char*)PING_MESSAGE, strlen (PING_MESSAGE)));

          data->message_queue.Enqueue (ping_message);
        }

        continue;
      }

      last_keep_alive_time = milliseconds ();

      //TODO send with timeout

      size_t message_text_length = message->message.size ();
      size_t packet_size         = 8; //Header size

      if (data->connection_settings.compression_enabled)
      {
        packet_size += 4; //Packed header

        //TODO
        throw std::invalid_argument ("Compression not supported");
      }
      else
      {
        packet_size += message_text_length;
      }

      if (data->connection_settings.encryption_enabled)
      {
        packet_size += 48;
      }

      if (packet_size > current_buffer_size)
      {
        delete [] send_buffer;

        send_buffer         = 0;
        current_buffer_size = 0;

        size_t new_buffer_size = packet_size * 2;

        send_buffer         = new unsigned char [new_buffer_size];
        current_buffer_size = new_buffer_size;
      }

      send_buffer [0] = PACKET_HEADER;

      send_buffer [1] = 0; //Options

      if (data->connection_settings.compression_enabled)
        send_buffer [1] |= OPTIONS_COMPRESSED;

      if (data->connection_settings.encryption_enabled)
        send_buffer [1] |= OPTIONS_ENCRYPTED;

      unsigned short* plugin_id = (unsigned short*)(send_buffer + 2);

      *plugin_id = message->plugin_id;

      unsigned int* message_size = (unsigned int*)(send_buffer + 4);

      *message_size = packet_size - 8;

      if (data->connection_settings.encryption_enabled)
      {
        unsigned char* hmac         = send_buffer + 8;
        unsigned char* iv           = hmac + HMAC_SIZE;
        unsigned char* message_data = iv + IV_SIZE;

        for (size_t i = 0; i < IV_SIZE; i++)
          iv [i] = rand () % 0xff;

        aes_ofb (data->connection_settings.encryption_key, data->connection_settings.encryption_key_bits, message_text_length, message->message.data (), message_data, (const unsigned char (&)[16])*iv);

        hmac_sha256 (hashed_key, sizeof (hashed_key), iv, IV_SIZE + message_text_length, (unsigned char (&)[32])*hmac);
      }

      data->tcp_client.Send (send_buffer, packet_size);

      data->listener.Log (format ("Message '%s' sent", message->message.c_str ()).c_str ());
    }
    catch (std::exception& e)
    {
      data->listener.OnError (ErrorCode_Generic, e.what ());
      break;
    }
    catch (...)
    {
      data->listener.OnError (ErrorCode_Generic, "Unknown error while sending events");
      break;
    }
  }

  delete [] send_buffer;

  receive_thread.Join ();

  return 0;
}

}

/*
   HS Server Connection implementation
*/

struct HsConnection::Impl : public ITcpClientListener, public IBackgroundThreadListener
{
  HsConnection*               connection;           //this connection
  TcpClient                   tcp_client;           //tcp connection
  HsConnectionSettings        settings;             //connection settings
  IHsConnectionEventListener* event_listener;       //events listener
  IHsConnectionLogListener*   log_listener;         //log messages listener
  volatile HsConnectionState  state;                //connection state
  std::auto_ptr<Thread>       send_thread;          //thread for sending data
  volatile bool               needs_stop_threads;   //signal to stop background threads
  MessageQueue                send_queue;           //send messages queue

  Impl (HsConnection* in_connection, const HsConnectionSettings& in_settings)
    : connection (in_connection)
    , event_listener (0)
    , log_listener (0)
    , state (HsConnectionState_Disconnected)
    , needs_stop_threads (false)
    , send_queue (in_settings.send_queue_size)
  {
    memcpy (&settings, &in_settings, sizeof (settings));

    tcp_client.SetListener (this);
  }

  ~Impl ()
  {
    Disconnect ();
  }

  //Connection
  void Connect (const char* host, unsigned short port)
  {
    if (!host)
      throw std::invalid_argument ("HsConnection::Connect - null host");

    if (state != HsConnectionState_Disconnected)
      throw std::logic_error ("HsConnection::Connect - connection already connected");

    state = HsConnectionState_Connecting;
    OnConnectionStateChanged ();

    needs_stop_threads = false;

    std::auto_ptr<SendFunctionArg> send_function_arg (new SendFunctionArg (host, port, settings, tcp_client, send_queue, needs_stop_threads, *this));

    send_thread.reset (new Thread (send_function, send_function_arg.get ()));

    send_function_arg.release ();
  }

  void Disconnect ()
  {
    if (state == HsConnectionState_Disconnected)
      return;

    needs_stop_threads = true;

    send_thread->Join ();

    send_thread.reset (0);

    tcp_client.Disconnect ();

    state = HsConnectionState_Disconnected;

    if (!send_queue.Empty ())
      Log (format ("%u messages dropped because of disconnect", send_queue.Size ()).c_str ());

    send_queue.Clear ();

    OnConnectionStateChanged ();
  }

  //Sending events
  void SendMessage (unsigned short plugin_id, const unsigned char* message, size_t size)
  {
    if (!message && size)
      throw std::invalid_argument ("HsConnection::SendMessage - null message");

    if (state == HsConnectionState_Disconnected)
      throw std::logic_error ("HsConnection::SendMessage - connection disconnected");

    std::auto_ptr<Message> new_message (new Message (plugin_id, message, size));

    if (!send_queue.Enqueue (new_message))
    {
      Log ("Message queue overlfow");
      OnError (ErrorCode_SendQueueOverflow, "Queue overflowed");
      return;
    }
  }

  //Connection disconnected
  void OnDisconnect (TcpClient& sender)
  {
    Disconnect ();
  }

  void Log (const char* message)
  {
    if (!log_listener)
      return;

    try
    {
      log_listener->OnLogMessage (*connection, message);
    }
    catch (...)
    {
    }
  }

  void OnConnectionStateChanged ()
  {
    if (!event_listener)
      return;

    switch (state)
    {
      case HsConnectionState_Disconnected:
        Log ("Disconnected");
        break;
      case HsConnectionState_Connected:
        Log ("Connection established");
        break;
      case HsConnectionState_Connecting:
        Log ("Connection started");
        break;
    }

    try
    {
      event_listener->OnStateChanged (*connection, state);
    }
    catch (std::exception& e)
    {
      Log (format ("IHsConnectionEventListener::OnStateChanged - exception '%s'", e.what ()).c_str ());
    }
    catch (...)
    {
      Log ("IHsConnectionEventListener::OnStateChanged - unknown exception");
    }
  }

  void OnConnected ()
  {
    state = HsConnectionState_Connected;
    OnConnectionStateChanged ();
  }

  void OnConnectFailed (const char* error)
  {
    state = HsConnectionState_Disconnected;
    OnError (ErrorCode_Generic, error);
    OnConnectionStateChanged ();
  }

  void OnMessageReceived (unsigned short plugin_id, const unsigned char* message, size_t size)
  {
    if (!event_listener)
      return;

    try
    {
      event_listener->OnMessageReceived (*connection, plugin_id, message, size);
    }
    catch (std::exception& e)
    {
      Log (format ("IHsConnectionEventListener::OnMessageReceived - exception '%s'", e.what ()).c_str ());
    }
    catch (...)
    {
      Log ("IHsConnectionEventListener::OnMessageReceived - unknown exception");
    }
  }

  void OnError (ErrorCode code, const char* error)
  {
    Log (error);

    if (!event_listener)
      return;

    try
    {
      event_listener->OnError (*connection, code, error);
    }
    catch (std::exception& e)
    {
      Log (format ("IHsConnectionEventListener::OnError - exception '%s'", e.what ()).c_str ());
    }
    catch (...)
    {
      Log ("IHsConnectionEventListener::OnError - unknown exception");
    }
  }
};

/*
   Constructor / destructor
*/

HsConnection::HsConnection (const HsConnectionSettings& settings)
  : impl (new Impl (this, settings))
  {}

HsConnection::~HsConnection ()
{
  delete impl;
}

/*
   Connection
*/

void HsConnection::Connect (const char* host, unsigned short port)
{
  impl->Connect (host, port);
}

void HsConnection::Disconnect ()
{
  impl->Disconnect ();
}

/*
   Sending events
*/

void HsConnection::SendMessage (unsigned short plugin_id, const unsigned char* message, size_t size)
{
  impl->SendMessage (plugin_id, message, size);
}

/*
   Event listening
*/

void HsConnection::SetEventListener (IHsConnectionEventListener* listener)
{
  if (impl->state != HsConnectionState_Disconnected)
    throw std::logic_error ("HsConnection::SetEventListener - listener can be changed only in disconnected state");

  impl->event_listener = listener;
}

void HsConnection::SetLogListener (IHsConnectionLogListener* listener)
{
  if (impl->state != HsConnectionState_Disconnected)
    throw std::logic_error ("HsConnection::SetEventListener - listener can be changed only in disconnected state");

  impl->log_listener = listener;
}

IHsConnectionEventListener* HsConnection::EventListener () const
{
  return impl->event_listener;
}

IHsConnectionLogListener* HsConnection::LogListener () const
{
  return impl->log_listener;
}

/*
   Connection state
*/

HsConnectionState HsConnection::State () const
{
  return impl->state;
}
