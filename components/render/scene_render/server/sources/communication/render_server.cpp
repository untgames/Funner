#include "shared.h"

using namespace render::scene::server;

/*
    Описание реализации сервера рендеринга сцены
*/

struct Server::Impl
{
  ConnectionAcceptor       acceptor;            //объект, принимающий входящие подключения
  ServerLoopbackConnection loopback_connection; //соединение для взаимодействия с сервером
  ClientWindowManager      window_manager;      //менеджер окон

/// Конструктор
  Impl (const char* name, ServerThreadingModel threading_model)
    : acceptor (name, threading_model)
    , loopback_connection (name)
    , window_manager (loopback_connection)
  {
  }
};

/*
    Конструктор / деструктор
*/

Server::Server (const char* name, ServerThreadingModel threading_model)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    switch (threading_model)
    {
      case ServerThreadingModel_SingleThreaded:
      case ServerThreadingModel_MultiThreaded:
        break;
      default:
        throw xtl::make_argument_exception ("", "threading_model", threading_model);
    }

    impl.reset (new Impl (name, threading_model));    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server::Server");
    throw;
  }
}

Server::~Server ()
{
}

/*
    Присоединение окон
*/

void Server::AttachWindow (const char* name, syslib::Window& window, const common::PropertyMap& properties)
{
  try
  {
    impl->window_manager.AttachWindow (name, window, properties);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server::AttachWindow(const char*,syslib::Window&,const common::PropertyMap&)");
    throw;
  }
}

void Server::AttachWindow (const char* name, syslib::Window& window, const common::ParseNode& properties)
{
  try
  {
    impl->window_manager.AttachWindow (name, window, to_properties (properties));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server::AttachWindow(const char*,syslib::Window&,const common::ParseNode&)");
    throw;
  }
}

void Server::DetachWindow (const char* name)
{
  try
  {
    impl->window_manager.DetachWindow (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server::DetachWindow");
    throw;
  }
}

void Server::DetachAllWindows ()
{
  try
  {
    impl->window_manager.DetachAllWindows ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server::DetachAllWindows");
    throw;
  }
}

/*
    Ожидание незавершенных операций
*/

bool Server::TryFinish (size_t timeout_ms)
{
  try
  {
    return impl->acceptor.WaitQueuedCommands (timeout_ms);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server::TryFinish");
    throw;
  }  
}

void Server::Finish (size_t timeout_ms)
{
  if (!TryFinish (timeout_ms))
    throw xtl::format_operation_exception ("render::scene::server::Server::Finish", "Can't finish queued tasks for %u ms", timeout_ms);
}

void Server::Finish ()
{
  Finish (size_t (-1));
}
