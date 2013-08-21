#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Описание реализации состояния соединения
*/

struct ConnectionState::Impl
{
};

/*
    Конструктор / деструктор
*/

ConnectionState::ConnectionState ()
{
  try
  {
    impl.reset (new Impl);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ConnectionState::ConnectionState");
    throw;
  }
}

ConnectionState::~ConnectionState ()
{
}

/*
    Команды клиента
*/

void ConnectionState::LoadResource (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ConnectionState::UnloadResource (const char* name)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
