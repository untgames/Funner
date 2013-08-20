#include "shared.h"

using namespace render::scene::interchange;

namespace
{

const char*  LOG_NAME           = "render.scene.interchange";
const size_t MAX_POP_TIMEOUT_MS = 100;

}

typedef xtl::com_ptr<IConnection> ConnectionPtr;

struct RenderThread::Impl: public IConnection, public xtl::reference_counter, public xtl::trackable
{
  stl::string                   connection_name;        //имя соединения
  ConnectionPtr                 target_connection;      //целевое соединение
  common::Log                   log;                    //поток протоколирования
  stl::string                   manager_name;           //имя менеджера соединения
  CommandQueue                  command_queue;          //очередь команд
  volatile bool                 stop_request;           //флаг запроса остановки нити  
  stl::auto_ptr<syslib::Thread> thread;                 //нить

/// Конструктор
  Impl (const char* thread_connection_name, size_t render_queue_size, const ConnectionPtr& connection)
    : connection_name (thread_connection_name)
    , target_connection (connection)
    , log (common::format ("%s.%s", LOG_NAME, thread_connection_name).c_str ())
    , manager_name (common::format ("RenderThread.%s", thread_connection_name))
    , command_queue (render_queue_size)
    , stop_request ()
  {
    ConnectionManager::RegisterConnection (manager_name.c_str (), thread_connection_name, xtl::bind (&Impl::CreateConnection, this, _2));

    try
    {
      thread.reset (new syslib::Thread (thread_connection_name, xtl::bind (&Impl::Run, this)));
    }
    catch (...)
    {
      ConnectionManager::UnregisterConnection (manager_name.c_str ());
      throw;
    }
  }

/// Деструктор
  ~Impl ()
  {
    try
    {
      ConnectionManager::UnregisterConnection (manager_name.c_str ());

      stop_request = true;

      thread->Join ();
    }
    catch (...)
    {
    }
  }

///Создание соединения
  IConnection* CreateConnection (const char* init_string)
  {
    addref (this);

    return this;
  }

/// Цикл нити
  int Run ()
  {
    while (!stop_request)
    {
      try
      {
        CommandBuffer buffer;

//TODO: optimize buffer pop

        if (!command_queue.Pop (buffer, MAX_POP_TIMEOUT_MS))
          continue; 

        target_connection->ProcessCommands (buffer);
      }
      catch (std::exception& e)
      {
        log.Printf ("%s\n    at render::scene::interchange::RenderThread::Impl::Run", e.what ());
      }
      catch (...)
      {
        log.Printf ("unknown exception\n    at render::scene::interchange::RenderThread::Impl::Run");
      }
    }

    return 0;
  }

///Обработка входного потока данных
  void ProcessCommands (const CommandBuffer& commands)
  {
    try
    {
      command_queue.Push (commands);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::interchange::RenderThread::Impl::ProcessCommands");
      throw;
    }
  }

///Получение события оповещения об удалении
  xtl::trackable& GetTrackable () { return *this; }

///Подсчет ссылок
  void AddRef  () { addref (this); }
  void Release () { release (this); }
};

/*
    Конструктор / деструктор / присваивание
*/

RenderThread::RenderThread (const char* thread_connection_name, size_t render_queue_size, const char* target_connection_name, const char* target_connection_init_string)
{
  try
  {
    if (!thread_connection_name)
      throw xtl::make_null_argument_exception ("", "thread_connection_name");

    if (!target_connection_name)
      throw xtl::make_null_argument_exception ("", "target_connection_name");

    if (!target_connection_init_string)
      target_connection_init_string = "";

    ConnectionPtr connection (ConnectionManager::CreateConnection (target_connection_name, target_connection_init_string), false);

    impl = new Impl (thread_connection_name, render_queue_size, connection);
  } 
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::RenderThread::RenderThread(const char*,size_t,const char*,const char*)");
    throw;
  }
}

RenderThread::RenderThread (const char* thread_connection_name, size_t render_queue_size, IConnection* connection)
{
  try
  {
    if (!connection)
      throw xtl::make_null_argument_exception ("", "connection");

    impl = new Impl (thread_connection_name, render_queue_size, connection);
  } 
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::interchange::RenderThread::RenderThread(const char*,size_t,const Connection&)");
    throw;
  }
}

RenderThread::RenderThread (const RenderThread& thread)
  : impl (thread.impl)
{
  addref (impl);
}

RenderThread::~RenderThread ()
{
  release (impl);
}

RenderThread& RenderThread::operator = (const RenderThread& thread)
{
  RenderThread new_thread (thread);

  stl::swap (impl, new_thread.impl);

  return *this;
}

/*
    Имена соединений
*/

const char* RenderThread::ConnectionName () const
{
  return impl->connection_name.c_str ();
}
