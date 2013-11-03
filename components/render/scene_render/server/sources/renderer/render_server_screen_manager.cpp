#include "shared.h"

using namespace render::scene::server;

/*
    Описание реализации менеджера экранов
*/

typedef stl::hash_map<object_id_t, Screen> ScreenMap;

struct ScreenManager::Impl
{
  WindowManager&  window_manager;   //менеджер окон
  ViewportManager viewport_manager; //менеджер областей вывода
  RenderManager   render_manager;   //менеджер рендеринга
  ScreenMap       screens;          //карта экранов

/// Конструктор
  Impl (WindowManager& in_window_manager, const RenderManager& in_render_manager, const ViewportManager& in_viewport_manager)
    : window_manager (in_window_manager)
    , viewport_manager (in_viewport_manager)
    , render_manager (in_render_manager)
  {
  }
};

/*
    Конструктор / деструктор
*/

ScreenManager::ScreenManager (WindowManager& window_manager, const RenderManager& render_manager, const ViewportManager& viewport_manager)
{
  try
  {
    impl = new Impl (window_manager, render_manager, viewport_manager);
  }
  catch (xtl::exception&e)
  {
    e.touch ("render::scene::server::ScreenManager::ScreenManager");
    throw;
  }
}

ScreenManager::~ScreenManager ()
{
}

/*
    Получение экрана / области вывода
*/

Screen& ScreenManager::GetScreen (object_id_t id)
{
  ScreenMap::iterator iter = impl->screens.find (id);

  if (iter == impl->screens.end ())
    throw xtl::format_operation_exception ("render::scene::server::ScreenManager::GetScreen", "Screen with id %llu has not been found", id);

  return iter->second;
}

Viewport& ScreenManager::GetViewport (object_id_t id)
{
  try
  {
    return impl->viewport_manager.GetViewport (id);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ScreenManager::GetViewport");
    throw;
  }
}

/*
    Создание экрана
*/

Screen ScreenManager::CreateScreen (object_id_t id, const char* name, const char* init_string)
{
  try
  {
    ScreenMap::iterator iter = impl->screens.find (id);

    if (iter != impl->screens.end ())
      throw xtl::format_operation_exception ("render::scene::server::ScreenManager::CreateScreen", "Screen with id %llu has been already registered", id);

    Screen screen (name, init_string, impl->window_manager, impl->render_manager, impl->viewport_manager);

    impl->screens.insert_pair (id, screen);
    
    return screen;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ScreenManager::CreateScreen");
    throw;
  }
}

void ScreenManager::RemoveScreen (object_id_t id)
{
  impl->screens.erase (id);
}

/*
    Максимальный уровень вложенности рендеринга
*/

void ScreenManager::SetMaxDrawDepth (size_t level)
{
  try
  {
    impl->viewport_manager.SetMaxDrawDepth (level);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::server::ScreenManager::SetMaxDrawDepth");
    throw;
  }
}

size_t ScreenManager::MaxDrawDepth () const
{
  return impl->viewport_manager.MaxDrawDepth ();
}
