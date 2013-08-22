#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Описание реализации сервера рендеринга
*/

struct ServerImpl::Impl
{
  server::WindowManager window_manager; //менеджер окон
};

/*
    Конструктор / деструктор
*/

ServerImpl::ServerImpl ()
{
  try
  {
    impl.reset (new Impl);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ServerImpl::ServerImpl");
    throw;
  }
}

ServerImpl::~ServerImpl ()
{
}

/*
    Менеджеры
*/

WindowManager& ServerImpl::WindowManager ()
{
  return impl->window_manager;
}
