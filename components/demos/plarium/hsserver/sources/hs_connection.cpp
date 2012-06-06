#include "shared.h"

using namespace plarium::hsserver;
using namespace plarium::system;
using namespace plarium::utility;

namespace
{

const size_t DEQUEUE_TIMEOUT = 1000;

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

    //Error occured
    virtual void OnError (ErrorCode code, const char* error) = 0;

  protected:
    virtual ~IBackgroundThreadListener () {}
};

//Send-thread function arguments
struct SendFunctionArg
{
  std::string                host;
  unsigned short             port;
  TcpClient&                 tcp_client;
  MessageQueue&              message_queue;
  volatile bool&             need_stop;
  IBackgroundThreadListener& listener;

  SendFunctionArg (const char* in_host, unsigned short in_port, TcpClient& in_tcp_client,
                   MessageQueue& in_message_queue, volatile bool& in_need_stop,
                   IBackgroundThreadListener& in_listener)
    : host (in_host)
    , port (in_port)
    , tcp_client (in_tcp_client)
    , message_queue (in_message_queue)
    , need_stop (in_need_stop)
    , listener (in_listener)
    {}
};

//Receive-thread function arguments
struct ReceiveFunctionArg
{
  TcpClient&                 tcp_client;
  volatile bool&             need_stop;
  IBackgroundThreadListener& listener;

  ReceiveFunctionArg (TcpClient& in_tcp_client, volatile bool& in_need_stop, IBackgroundThreadListener& in_listener)
    : tcp_client (in_tcp_client)
    , need_stop (in_need_stop)
    , listener (in_listener)
    {}
};

//Receive-thread  function
void* receive_function (void* user_data)
{
  std::auto_ptr<ReceiveFunctionArg> data ((ReceiveFunctionArg*)user_data);

  volatile bool& need_stop = data->need_stop;

  for (;;)
  {
    if (need_stop)
      break;
  }

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

  std::auto_ptr<ReceiveFunctionArg> receive_function_arg (new ReceiveFunctionArg (data->tcp_client, data->need_stop, data->listener));

  Thread receive_thread (receive_function, receive_function_arg.get ());      //thread for receiving data

  receive_function_arg.release ();

  for (;;)
  {
    try
    {
      if (need_stop)
      {

        break;
      }

      std::auto_ptr<Message> message = data->message_queue.Dequeue (DEQUEUE_TIMEOUT);

      if (!message.get ())
        continue;

      //...
    }
    catch (...)
    {
      ///...................
    }
  }

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

    std::auto_ptr<SendFunctionArg> send_function_arg (new SendFunctionArg (host, port, tcp_client, send_queue, needs_stop_threads, *this));

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
