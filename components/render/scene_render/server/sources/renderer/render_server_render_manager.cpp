#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Описание реализации менеджера рендеринга
*/

namespace
{

struct ServerLog
{
  common::Log log; //поток отладочного протоколирования

  ServerLog (const char* name)
    : log (common::format ("render.server.%s", name).c_str ())
  {
    log.Printf ("Render server has been created");
  }

  ~ServerLog ()
  {
    log.Printf ("Render server has been destroyed");
  }
};

}

struct RenderManager::Impl: public ServerLog, public xtl::reference_counter
{
  manager::RenderManager render_manager;

/// Конструктор
  Impl (const char* name)
    : ServerLog (name)
  {
  }
};

/*
    Конструктор / деструктор
*/

RenderManager::RenderManager (const char* name)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");

    impl = new Impl (name);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::RenderManager::RenderManager");
    throw;
  }
}

RenderManager::RenderManager (const RenderManager& manager)
  : impl (manager.impl)
{
  addref (impl);
}

RenderManager::~RenderManager ()
{
  release (impl);
}

RenderManager& RenderManager::operator = (const RenderManager& manager)
{
  RenderManager tmp (manager);

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Поток отладочного протоколирования
*/

common::Log& RenderManager::Log ()
{
  return impl->log;
}

/*
    Менеджер
*/

render::manager::RenderManager& RenderManager::Manager ()
{
  return impl->render_manager;
}
