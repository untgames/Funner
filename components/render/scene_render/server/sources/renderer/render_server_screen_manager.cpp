#include "shared.h"

using namespace render::scene::server;

/*
    Описание реализации менеджера экранов
*/

typedef stl::hash_map<object_id_t, Screen> ScreenMap;

struct ScreenManager::Impl
{
  WindowManager& window_manager; //менеджер окон
  ScreenMap      screens;        //карта экранов

/// Конструктор
  Impl (WindowManager& in_window_manager)
    : window_manager (in_window_manager)
  {
  }
};

/*
    Конструктор / деструктор
*/

ScreenManager::ScreenManager (WindowManager& window_manager)
{
  try
  {
    impl = new Impl (window_manager);
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
    Получение экрана
*/

Screen ScreenManager::GetScreen (object_id_t id)
{
  ScreenMap::iterator iter = impl->screens.find (id);

  if (iter == impl->screens.end ())
    throw xtl::format_operation_exception ("render::scene::server::ScreenManager::GetScreen", "Screen with id %llu has not been found", id);

  return iter->second;
}

/*
    Создание экрана
*/

Screen ScreenManager::CreateScreen (object_id_t id, const char* name)
{
  try
  {
    ScreenMap::iterator iter = impl->screens.find (id);

    if (iter != impl->screens.end ())
      throw xtl::format_operation_exception ("render::scene::server::ScreenManager::CreateScreen", "Screen with id %llu has been already registered", id);

    Screen screen (name);

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
