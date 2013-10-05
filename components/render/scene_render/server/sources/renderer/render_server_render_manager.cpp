#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

/*
    Константы
*/

const size_t LISTENER_ARRAY_RESERVE_SIZE = 32; //резервируемое количество слушателей

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

typedef stl::vector<IRenderManagerListener*> ListenerArray;

struct RenderManager::Impl: public ServerLog, public xtl::reference_counter
{
  manager::RenderManager render_manager;                   //менеджер рендеринга
  xtl::auto_connection   configuration_changed_connection; //соединение с обработчиком изменения конфигурации
  ListenerArray          listeners;                        //слушатели событий

/// Конструктор
  Impl (const char* name)
    : ServerLog (name)
  {
    configuration_changed_connection = render_manager.RegisterEventHandler (manager::RenderManagerEvent_OnConfigurationChanged, xtl::bind (&Impl::OnConfigurationChanged, this));

    listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
  }

///Обработчик изменения конфигурации
  void OnConfigurationChanged ()
  {
    common::ParseNode configuration = render_manager.Configuration ();

    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
    {
      try
      {
        (*iter)->OnRenderManagerConfigurationChanged (configuration);
      }
      catch (...)
      {
      }
    }
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

/*
    Подписка на события менеджера
*/

void RenderManager::AttachListener (IRenderManagerListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::scene::server::RenderManager::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void RenderManager::DetachListener (IRenderManagerListener* listener)
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void RenderManager::DetachAllListeners ()
{
  impl->listeners.clear ();
}
