#include "shared.h"

using namespace render::scene::client;

/*
    Конструктор / деструктор
*/

SceneObject::SceneObject (SceneManager& in_scene_manager)
  : scene_manager (in_scene_manager)
{
  need_update = false;

  UpdateNotify ();
}

SceneObject::~SceneObject ()
{
  if (need_update)
  {
    SceneUpdateList& list = scene_manager.UpdateList ();

    if (prev_update) prev_update->next_update = next_update;
    else             list.first               = next_update;

    if (next_update) next_update->prev_update = prev_update;
    else             list.last                = prev_update;
  }
}

/*
    Имя объекта
*/

const char* SceneObject::NameCore ()
{
  return "";
}

/*
    Запрос синхронизации
*/

void SceneObject::UpdateNotify ()
{
  if (need_update)
    return;

  SceneUpdateList& list = scene_manager.UpdateList ();

  need_update = true;
  next_update = 0;
  prev_update = list.last;
  
  if (prev_update) prev_update->next_update = this;
  else             list.last = list.first   = this;
}

/*
    Синхронизация
*/

void SceneObject::Update (Context& context)
{
  try
  {
    need_update = false;

    UpdateCore (context);  
  }
  catch (xtl::exception& e)
  {
    try
    {
      const char* name = NameCore ();

      e.touch ("render::scene::client::SceneObject::Update(%s)", name);
      throw;
    }
    catch (...)
    {
      e.touch ("render::scene::client::SceneObject::Update");
      throw;
    }
  }
}
