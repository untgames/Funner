#include "shared.h"

#include <network/tcp_client.h>

using namespace script;
using namespace network;

namespace components
{

namespace network_script_bind
{

/*
    Константы
*/

const char* TCP_CLIENT_LIBRARY = "Network.TcpClient";
const char* COMPONENT_NAME     = "script.binds.Network";
const char* BINDER_NAME        = "Network";

///враппер для TcpClient в скриптах
class ScriptTcpClient: public TcpClient, public xtl::dynamic_cast_root
{
  public:
    typedef xtl::shared_ptr<ScriptTcpClient> Pointer;
    
///Конструктор    
    ScriptTcpClient (const char* host, unsigned short port, unsigned int timeout) : TcpClient (host, port, timeout) {}
    
///Приём данных
    stl::string Receive ()
    {
      stl::string result;      
      
      if (!TcpClient::ReceiveAvailable ())
        return result;
      
      result.fast_resize (TcpClient::ReceiveAvailable ());      
      
      if (!ReceiveExactly (&result [0], (unsigned int)result.size (), 1000))
        return stl::string ();
        
      return result;
    }

    void SwitchToAsyncSending () { TcpClient::SwitchToAsyncSending (); }

    void SwitchToAsyncReceiving () { TcpClient::SwitchToAsyncReceiving (); }

///Регистрация обработчика асинхронного получения данных
    typedef xtl::function<void (const stl::string&)> AsyncReceivingEventHandler;

    xtl::connection RegisterAsyncReceivingEventHandler (const AsyncReceivingEventHandler& handler)
    {
      return TcpClient::RegisterAsyncReceivingEventHandler (AsyncReceivingAdaptor (handler));
    }
   
///Порождающие функции
    static Pointer Create (const char* host, unsigned short port)
    {
      return CreateWithTimeout (host, port, 0);
    }
    
    static Pointer CreateWithTimeout (const char* host, unsigned short port, unsigned int timeout)
    {
      return Pointer (new ScriptTcpClient (host, port, timeout));
    }

  private:
    struct AsyncReceivingAdaptor
    {
      AsyncReceivingAdaptor (const ScriptTcpClient::AsyncReceivingEventHandler& in_handler) : handler (in_handler) {}

      void operator () (const void* buffer, size_t size) const
      {
        common::ActionQueue::PushAction (xtl::bind (handler, stl::string ((const char*)buffer, size)), common::ActionThread_Main);
      }

      AsyncReceivingEventHandler handler;
    };
};

void bind_tcp_client_library (Environment& environment)
{
  InvokerRegistry lib = environment.CreateLibrary (TCP_CLIENT_LIBRARY);

    //регистрация функций создания

  lib.Register ("Create", make_invoker (make_invoker (&ScriptTcpClient::CreateWithTimeout), make_invoker (&ScriptTcpClient::Create)));

    //регистрация операций
    
  lib.Register ("Close",                               make_invoker (&ScriptTcpClient::Close));
  lib.Register ("Send",                                make_invoker (xtl::implicit_cast<void (ScriptTcpClient::*)(const char*)> (&ScriptTcpClient::Send)));
  lib.Register ("Receive",                             make_invoker (&ScriptTcpClient::Receive));
  lib.Register ("SwitchToAsyncSending",                make_invoker (&ScriptTcpClient::SwitchToAsyncSending));
  lib.Register ("SwitchToAsyncReceiving",              make_invoker (&ScriptTcpClient::SwitchToAsyncReceiving));
  lib.Register ("CreateAsyncReceivingCallbackHandler", make_callback_invoker<ScriptTcpClient::AsyncReceivingEventHandler::signature_type> ());    
  lib.Register ("RegisterAsyncReceivingEventHandler",  make_invoker (&ScriptTcpClient::RegisterAsyncReceivingEventHandler));

  lib.Register ("get_ReceiveBufferSize", make_invoker (&ScriptTcpClient::ReceiveBufferSize));
  lib.Register ("set_ReceiveBufferSize", make_invoker (&ScriptTcpClient::SetReceiveBufferSize));  
  lib.Register ("get_SendBufferSize",    make_invoker (&ScriptTcpClient::SendBufferSize));
  lib.Register ("set_SendBufferSize",    make_invoker (&ScriptTcpClient::SetSendBufferSize));  
  lib.Register ("get_TcpNoDelay",        make_invoker (&ScriptTcpClient::IsTcpNoDelay));
  lib.Register ("set_TcpNoDelay",        make_invoker (&ScriptTcpClient::SetTcpNoDelay));
  lib.Register ("get_Closed",            make_invoker (&ScriptTcpClient::IsClosed));
  lib.Register ("get_Connected",         make_invoker (&ScriptTcpClient::IsConnected));  
  lib.Register ("get_ReceiveAvailable",  make_invoker (&ScriptTcpClient::ReceiveAvailable));  
  lib.Register ("get_AsyncSending",      make_invoker (&ScriptTcpClient::IsAsyncSendingEnabled));
  lib.Register ("get_AsyncReceiving",    make_invoker (&ScriptTcpClient::IsAsyncReceivingEnabled));

    //регистрация типов данных

  environment.RegisterType<ScriptTcpClient> (TCP_CLIENT_LIBRARY);  
}

void bind_network_library (Environment& environment)
{
  bind_tcp_client_library (environment);
}

/*
    Компонент
*/

class Component
{
  public:
    Component ()
    {
      LibraryManager::RegisterLibrary (BINDER_NAME, &Bind);
    }

  private:
    static void Bind (Environment& environment)
    {
      bind_network_library (environment);
    }
};

extern "C"
{

common::ComponentRegistrator<Component> NetworkScriptBind (COMPONENT_NAME);

}

}

}
