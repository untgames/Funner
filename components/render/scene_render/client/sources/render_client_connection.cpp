#include "shared.h"

using namespace render::scene::client;
using namespace render::scene;

/*
    Описание реализации соединения с сервером
*/

namespace
{

/// Состояния соединения
enum State
{
  State_Disconnected,
  State_LogonAckWaiting,
  State_Connected,
};

/// Контекст обработки
class ContextImpl: public Context
{
  public:
    ContextImpl (SceneRenderImpl& render) : Context (render) {}

    using Context::SetCounterparty;
    using Context::ProcessCommands;
};

typedef xtl::com_ptr<interchange::IConnection> ConnectionPtr;

}

struct Connection::Impl: public xtl::reference_counter, public xtl::trackable
{
  stl::string                    response_connection_name;    //имя соединения для ответов
  syslib::Condition              logon_ack_waiter;            //семафор ожидания ответа на логин
  syslib::Mutex                  mutex;                       //мьютекс данных
  volatile State                 state;                       //состояние соединения
  stl::string                    description;                 //описание соединения
  ConnectionPtr                  client_to_server_connection; //соединение от клиента к серверу
  size_t                         owner_thread_id;             //идентификатор нити владельца соединения
  stl::auto_ptr<SceneRenderImpl> scene_render;                //реализация рендера сцены  
  stl::auto_ptr<ContextImpl>     context;                     //контекст

/// Конструктор
  Impl ()
    : state (State_Disconnected)
    , owner_thread_id (syslib::Thread::GetCurrentThreadId ())
  {
    response_connection_name = common::format ("response_connection.%p", this);

    interchange::ConnectionManager::RegisterConnection (response_connection_name.c_str (), response_connection_name.c_str (), xtl::bind (&Impl::CreateResponseConnection, this, _1, _2));
  }

/// Деструктор
  ~Impl ()
  {
    try
    {
      mutex.Lock ();

      interchange::ConnectionManager::UnregisterConnection (response_connection_name.c_str ());
    }
    catch (...)
    {
    }
  }

/// Соединение получения ответов от сервера
  class ServerToClientConnection: public interchange::IConnection, public xtl::reference_counter, public xtl::trackable
  {
    public:
      ServerToClientConnection (Impl* in_impl) : impl (in_impl), owner_thread_id (impl->owner_thread_id) {}

      ///Обработка входного потока данных
      void ProcessCommands (const interchange::CommandBuffer& commands)
      {
        try
        {
          common::ActionQueue::PushAction (xtl::bind (&ServerToClientConnection::ProcessCommandsCore, xtl::intrusive_ptr<ServerToClientConnection> (this), commands), owner_thread_id);
        }
        catch (xtl::exception& e)
        {
          e.touch ("render::scene::client::Connection::Impl::ServerToClientConnection::ProcessCommands");
          throw;
        }
      }

      ///Получение события оповещения об удалении
      xtl::trackable& GetTrackable () { return *this; }

      ///Подсчет ссылок
      void AddRef ()  { addref (this); }
      void Release () { release (this); }

    private:
      void ProcessCommandsCore (const interchange::CommandBuffer& commands)
      {
        try
        {
          xtl::intrusive_ptr<Impl> local_impl (&*impl);

          if (!local_impl)
            return;

          local_impl->ProcessResponseFromServer (commands);
        }
        catch (xtl::exception& e)
        {
          e.touch ("render::scene::client::Connection::Impl::ServerToClientConnection::ProcessCommandsCore");
          throw;
        }
      }

    private:
      xtl::trackable_ptr<Impl> impl;
      size_t                   owner_thread_id;
  };

/// Функция создания соединения для получения ответов от сервера
  interchange::IConnection* CreateResponseConnection (const char* name, const char* init_string)
  {
    try
    {
      syslib::Lock lock (mutex);

      if (!name)
        throw xtl::make_null_argument_exception ("", "name");

      if (name != response_connection_name)
        throw xtl::format_operation_exception ("", "Connection name '%s' doesn't match with expected '%s'", name, response_connection_name.c_str ());

      common::PropertyMap properties = common::parse_init_string (init_string);

      if (properties.IsPresent ("initiator_name"))
        description = properties.GetString ("initiator_name");

      state = State_Connected;

      logon_ack_waiter.NotifyOne ();

      return new ServerToClientConnection (this);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::Connection::Impl::CreateResponseConnection");
      throw;
    }
  }

/// Обработка ответа от сервера
  void ProcessResponseFromServer (const interchange::CommandBuffer& commands)
  {
    try
    {
      if (!context)
        throw xtl::format_operation_exception ("", "Null context");

      context->ProcessCommands (commands);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::Connection::Impl::ProcessResponseFromServer");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

Connection::Connection (const char* connection_name, const char* init_string, size_t logon_timeout)
{
  try
  {
    impl = new Impl;

      //создание соединения

    syslib::Lock lock (impl->mutex);

    size_t start_time = common::milliseconds ();

    impl->state                       = State_LogonAckWaiting;
    impl->client_to_server_connection = ConnectionPtr (interchange::ConnectionManager::CreateConnection (connection_name, common::format ("%s logon_timeout=%d initiator_name='%s'", init_string, logon_timeout, impl->response_connection_name.c_str ()).c_str ()), false);

      //ожидание ответа от сервера

    while (impl->state == State_LogonAckWaiting)
    {
      size_t elapsed_time = common::milliseconds () - start_time;

      if (elapsed_time > logon_timeout)
        throw xtl::format_operation_exception ("", "Can't create rendering connection '%s' with init_string '%s' for %u ms", connection_name, init_string, logon_timeout);     

      size_t wait_timeout = logon_timeout - elapsed_time;

      impl->logon_ack_waiter.Wait (impl->mutex, wait_timeout);
    }

      //создание контекста

    impl->scene_render.reset (new SceneRenderImpl);
    impl->context.reset (new ContextImpl (*impl->scene_render));

    impl->context->SetCounterparty (&*impl->client_to_server_connection);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Connection::Connection");
    throw;
  }
}

Connection::~Connection ()
{
  release (impl);
}

/*
    Описание соединения
*/

const char* Connection::Description ()
{
  return impl->description.c_str ();
}

/*
    Рендер сцены
*/

SceneRenderImpl& Connection::Render ()
{
  return *impl->scene_render;
}

/*
    Контекст
*/

Context& Connection::Context ()
{
  return *impl->context;
}
