#include "shared.h"

using namespace plarium::hsserver;
using namespace plarium::system;
using namespace plarium::utility;

namespace
{

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

typedef std::deque<Message> MessageQueue;

//Send-thread function listener
class IBackgroundThreadListener
{
  public:
    //Connection established
    virtual void OnConnected () = 0;

    //Connection failed
    virtual void OnConnectFailed (const char* error) = 0;

    //Error occured
    virtual void OnError (ErrorCode code, const char* error) = 0;

    //Send queue empty
    virtual bool SendQueueEmpty () = 0;

    //Pop message
    virtual Message& SendQueueFront () = 0;
    virtual void     PopSendQueue   () = 0;

    //Check if we should stop send operations
    virtual bool NeedsStop () = 0;

  protected:
    virtual ~IBackgroundThreadListener () {}
};

//Send-thread function arguments
struct SendFunctionArg
{
  std::string                host;
  unsigned short             port;
  TcpClient&                 tcp_client;
  IBackgroundThreadListener& listener;

  SendFunctionArg (const char* in_host, unsigned short in_port, TcpClient& in_tcp_client, IBackgroundThreadListener& in_listener)
    : host (in_host)
    , port (in_port)
    , tcp_client (in_tcp_client)
    , listener (in_listener)
    {}
};

//Receive-thread function arguments
struct ReceiveFunctionArg
{
  TcpClient&                 tcp_client;
  IBackgroundThreadListener& listener;

  ReceiveFunctionArg (TcpClient& in_tcp_client, IBackgroundThreadListener& in_listener)
    : tcp_client (in_tcp_client)
    , listener (in_listener)
    {}
};

//Receive-thread  function
void* receive_function (void* user_data)
{
  ReceiveFunctionArg* data = (ReceiveFunctionArg*)user_data;

  TcpClient&                 tcp_client = data->tcp_client;
  IBackgroundThreadListener& listener   = data->listener;

  delete data;

  for (;;)
  {
    if (listener.NeedsStop ())
      break;
  }

  return 0;
}

//Send-thread function
void* send_function (void* user_data)
{
  SendFunctionArg* data = (SendFunctionArg*)user_data;

  std::string                host       = data->host;
  unsigned short             port       = data->port;
  TcpClient&                 tcp_client = data->tcp_client;
  IBackgroundThreadListener& listener   = data->listener;

  delete data;

  try
  {
    tcp_client.Connect (host.c_str (), port);
  }
  catch (std::exception& e)
  {
    listener.OnConnectFailed (e.what ());
    return 0;
  }
  catch (...)
  {
    listener.OnConnectFailed ("Uknown exception");
    return 0;
  }

  listener.OnConnected ();

  ReceiveFunctionArg* receive_function_arg = new ReceiveFunctionArg (tcp_client, listener);

  Thread receive_thread (receive_function, receive_function_arg);      //thread for receiving data

  for (;;)
  {
    if (listener.NeedsStop ())
    {
      receive_thread.Join ();

      break;
    }
  }

  return 0;
}

}

/*
   HS Server Connection implementation
*/

struct HsConnection::Impl : public ITcpClientListener, public IBackgroundThreadListener
{
  HsConnection*               connection;          //this connection
  TcpClient                   tcp_client;          //tcp connection
  HsConnectionSettings        settings;            //connection settings
  IHsConnectionEventListener* event_listener;      //events listener
  IHsConnectionLogListener*   log_listener;        //log messages listener
  volatile HsConnectionState  state;               //connection state
  Thread*                     send_thread;         //thread for sending data
  volatile bool               needs_stop_threads;  //signal to stop background threads
  MessageQueue                send_queue;          //send messages queue
  Mutex                       send_queue_mutex;    //send messages queue mutex

  Impl (HsConnection* in_connection, const HsConnectionSettings& in_settings)
    : connection (in_connection)
    , event_listener (0)
    , log_listener (0)
    , state (HsConnectionState_Disconnected)
    , send_thread (0)
    , needs_stop_threads (false)
  {
    memcpy (&settings, &in_settings, sizeof (settings));

    tcp_client.SetListener (this);
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

    SendFunctionArg* send_function_arg = new SendFunctionArg (host, port, tcp_client, *this);

    send_thread = new Thread (send_function, send_function_arg);
  }

  void Disconnect ()
  {
    if (state == HsConnectionState_Disconnected)
      return;

    needs_stop_threads = true;

    send_thread->Join ();

    delete send_thread;
    send_thread = 0;

    tcp_client.Disconnect ();

    state = HsConnectionState_Disconnected;

    if (!send_queue.empty ())
      Log (format ("%u messages dropped because of disconnect", send_queue.size ()).c_str ());

    send_queue.clear ();

    OnConnectionStateChanged ();
  }

  //Sending events
  void SendMessage (unsigned short plugin_id, const unsigned char* message, size_t size)
  {
    if (!message && size)
      throw std::invalid_argument ("HsConnection::SendMessage - null message");

    Lock send_queue_lock (send_queue_mutex);

    if (send_queue.size () >= settings.send_queue_size)
    {
      Log ("Message queue overlfow");
      OnError (ErrorCode_SendQueueOverflow, "Queue overflowed");
      return;
    }

    send_queue.push_back (Message (plugin_id, message, size));
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

  //Check if we should stop send operations
  bool NeedsStop ()
  {
    return needs_stop_threads;
  }

  //Send queue empty
  bool SendQueueEmpty ()
  {
    Lock send_queue_lock (send_queue_mutex);

    return send_queue.empty ();
  }

  //Pop message
  Message& SendQueueFront ()
  {
    Lock send_queue_lock (send_queue_mutex);

    return send_queue.front ();
  }

  void PopSendQueue ()
  {
    Lock send_queue_lock (send_queue_mutex);

    send_queue.pop_front ();
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
  if (impl->state != HsConnectionState_Disconnected)
    Disconnect ();

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
