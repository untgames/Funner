#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Constants
*/

const size_t RENDER_THREAD_SIZE = 2; //rendering command queue size (usually one command is one frame)

/*
    Acceptor implementation
*/

struct ConnectionAcceptor::Impl: public interchange::IRenderThreadListener
{
  stl::string                              name;                      //server name
  ServerThreadingModel                     threading_model;           //threading management model
  stl::auto_ptr<interchange::RenderThread> render_thread;             //rendering thread
  size_t                                   owner_thread_id;           //owner thread id
  xtl::shared_ptr<ServerImpl>              server;                    //rendering server
  syslib::Semaphore                        server_creation_waiter;    //server creation waiter semaphore
  stl::string                              server_creation_exception; //server creation exception

/// Constructor
  Impl (const char* in_name, ServerThreadingModel model)
    : name (in_name)
    , threading_model (model)
    , owner_thread_id (syslib::Thread::GetCurrentThreadId ())
    , server_creation_waiter (0)
  {
    switch (threading_model)
    {
      case ServerThreadingModel_SingleThreaded:
        InitServer ();
        break;
      case ServerThreadingModel_MultiThreaded:
        render_thread.reset (new interchange::RenderThread (name.c_str (), RENDER_THREAD_SIZE, this));
        break;
      default:
        throw xtl::format_operation_exception ("", "Invalid threading model %d", threading_model);
    }

      //waiting for server creation

    server_creation_waiter.Wait ();

    if (!server_creation_exception.empty ())
      throw xtl::format_operation_exception ("", server_creation_exception.c_str ());

    if (!server)
      throw xtl::format_operation_exception ("", "Internal error: server creation failed");

      //registering connections acceptor

    interchange::ConnectionManager::RegisterConnection (name.c_str (), name.c_str (), xtl::bind (&Impl::CreateConnection, this, _2));
  }

/// Destructor
  ~Impl ()
  {
    try
    {
      interchange::ConnectionManager::UnregisterConnection (name.c_str ());

      render_thread.reset ();

      ShutdownServer ();
    }
    catch (...)
    {
    }
  }

/// Rendering thread events handlers
  void OnRenderThreadStarted () 
  {
    try
    {
      InitServer ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ConnectionAcceptor::Impl::OnRenderThreadStarted");
      throw;
    }
  }

  void OnRenderThreadStopped () 
  {
    ShutdownServer ();
  }

/// Server creation
  void InitServer ()
  {
    try
    {
      try
      {
        server.reset (new ServerImpl (name.c_str ()));
      }
      catch (xtl::exception& e)
      {
        e.touch ("render::scene::server::ConnectionAcceptor::Impl::InitServer");
        throw;
      }
    }
    catch (std::exception& e)
    {
      server_creation_exception = e.what ();
    }
    catch (...)
    {
      server_creation_exception = "unknown exception\n    at render::scene::server::ConnectionAcceptor::Impl::InitServer";
    }

    server_creation_waiter.Post ();
  }

/// Server destroying
  void ShutdownServer ()
  {
    server.reset ();
  }

/// Connection creation
  interchange::IConnection* CreateConnection (const char* init_string)
  {
    try
    {
      if (!server)
        throw xtl::format_operation_exception ("", "Internal error: server was destroyed");

      switch (threading_model)
      {
        case ServerThreadingModel_SingleThreaded:
          if (syslib::Thread::GetCurrentThreadId () != owner_thread_id)
            throw xtl::format_operation_exception ("", "Can't create rendering connection from thread %u in single threaded model (use ServerThreadingModel_MultiThreaded)", syslib::Thread::GetCurrentThreadId ());

          return new Connection (*server, init_string);
        case ServerThreadingModel_MultiThreaded:
        {
          common::PropertyMap properties = common::parse_init_string (init_string);

          size_t logon_timeout = size_t (-1);

          if (properties.IsPresent ("logon_timeout"))
            logon_timeout = size_t (properties.GetInteger ("logon_timeout"));

          return render_thread->CreateConnectionFromThread (xtl::bind (&Impl::CreateConnectionInRenderThread, this, stl::string (init_string)), logon_timeout);
        }
        default:
          throw xtl::format_operation_exception ("", "Invalid threading model %d", threading_model);
      }
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ConnectionAcceptor::Impl::CreateConnection");
      throw;
    }
  }

/// Creation of connection from other thread
  interchange::IConnection* CreateConnectionInRenderThread (stl::string& init_string)
  {
    try
    {
      return new Connection (*server, init_string.c_str ());
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::ConnectionAcceptor::Impl::CreateConnectionInRenderThread");
      throw;
    }
  }
};

/*
    Constructor / destructor
*/

ConnectionAcceptor::ConnectionAcceptor (const char* name, ServerThreadingModel threading_model)
{
  try
  {
    impl.reset (new Impl (name, threading_model));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ConnectionAcceptor::ConnectionAcceptor");
    throw;
  }
}

ConnectionAcceptor::~ConnectionAcceptor ()
{
}

/*
    Waiting for queued commands completion
*/

bool ConnectionAcceptor::WaitQueuedCommands (size_t timeout_ms)
{
  try
  {
    if (!impl->render_thread)
      return true;

    return impl->render_thread->WaitQueuedCommands (timeout_ms);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ConnectionAcceptor::WaitQueuedCommands");
    throw;
  }
}
