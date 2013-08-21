#include "shared.h"

using namespace render::scene::server;

/*
    Описание реализации сервера рендеринга сцены
*/

struct Server::Impl
{
  ConnectionAcceptor acceptor; //объект, принимающий входящие подключения

/// Конструктор
  Impl (const char* name)
    : acceptor (name)
  {
  }
};

/*
    Конструктор / деструктор
*/

Server::Server (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl.reset (new Impl (name));    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::Server");
    throw;
  }
}

Server::~Server ()
{
}

/*
    Присоединение окон
*/

void Server::AttachWindow (const char* name, syslib::Window& window, const char* init_string)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Server::DetachWindow (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Server::DetachAllWindows ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
