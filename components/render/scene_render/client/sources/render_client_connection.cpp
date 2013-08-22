#include "shared.h"

using namespace render::scene::client;
using namespace render::scene;

/*
    Максимальное ожидание ответа от сервера
*/

const size_t DEFAULT_WAIT_FEEDBACK_TIMEOUT = 1000; //время ожидания ответа от сервера в миллисекундах
const size_t RESPONSE_QUEUE_SIZE           = 128;  //размер очереди ответов от сервера

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
    ContextImpl (ClientImpl& render) : Context (render) {}

    using Context::SetCounterparty;
    using Context::ProcessCommands;
};

typedef xtl::com_ptr<interchange::IConnection> ConnectionPtr;

}

struct Connection::Impl: public xtl::reference_counter, public xtl::trackable
{
  stl::string                response_connection_name;    //имя соединения для ответов
  syslib::Condition          logon_ack_waiter;            //семафор ожидания ответа на логин
  syslib::Mutex              mutex;                       //мьютекс данных
  volatile State             state;                       //состояние соединения
  stl::string                description;                 //описание соединения
  ConnectionPtr              client_to_server_connection; //соединение от клиента к серверу
  stl::auto_ptr<ClientImpl>  client;                      //реализация рендера сцены
  stl::auto_ptr<ContextImpl> context;                     //контекст
  interchange::CommandQueue  response_queue;              //очередь команд

/// Конструктор
  Impl ()
    : state (State_Disconnected)
    , response_queue (RESPONSE_QUEUE_SIZE)
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
      ServerToClientConnection (const interchange::CommandQueue& in_queue) : queue (in_queue)  {}

      ///Обработка входного потока данных
      void ProcessCommands (const interchange::CommandBuffer& commands)
      {
        try
        {
          queue.Push (interchange::CommandQueueItem (static_cast<interchange::IConnection*> (0), commands));
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
      interchange::CommandQueue queue;
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

      if (properties.IsPresent ("initiator"))
        description = properties.GetString ("initiator");

      state = State_Connected;

      logon_ack_waiter.NotifyOne ();

      return new ServerToClientConnection (response_queue);
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
    impl->client_to_server_connection = ConnectionPtr (interchange::ConnectionManager::CreateConnection (connection_name, common::format ("%s logon_timeout=%d initiator='%s'", init_string, logon_timeout, impl->response_connection_name.c_str ()).c_str ()), false);

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

    impl->client.reset (new ClientImpl);
    impl->context.reset (new ContextImpl (*impl->client));

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

ClientImpl& Connection::Client ()
{
  return *impl->client;
}

/*
    Контекст
*/

Context& Connection::Context ()
{
  return *impl->context;
}

/*
    Ожидание ответа от сервера
*/

void Connection::WaitServerFeedback ()
{
  try
  {
    if (!TryWaitServerFeedback (DEFAULT_WAIT_FEEDBACK_TIMEOUT))
      throw xtl::format_operation_exception ("", "No feedback from server is received for %u ms", DEFAULT_WAIT_FEEDBACK_TIMEOUT);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Connection::WaitServerFeedback");
    throw;
  }
}

bool Connection::TryWaitServerFeedback (size_t timeout_ms)
{
  try
  {
    interchange::CommandQueueItem item;

    if (!impl->response_queue.Pop (item, timeout_ms))
      return false;

    impl->ProcessResponseFromServer (item.buffer);

    return true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Connection::TryWaitServerFeedback");
    throw;
  }
}
