#include "shared.h"

using namespace render::scene::client;

/*
    Конструктор / деструктор
*/

SceneObject::SceneObject (Scene& in_scene)
  : scene (in_scene)
{
  need_update = false;

  UpdateNotify ();
}

SceneObject::~SceneObject ()
{
  if (need_update)
  {
    SceneUpdateList& list = scene.UpdateList ();

    if (prev_update) prev_update->next_update = next_update;
    else             list.first               = next_update;

    if (next_update) next_update->prev_update = prev_update;
    else             list.last                = prev_update;
  }
}

/*
    Имя объекта
*/

stl::string SceneObject::NameCore ()
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

  SceneUpdateList& list = scene.UpdateList ();

  need_update = true;
  next_update = 0;
  prev_update = list.last;
  
  if (prev_update) prev_update->next_update = this;
  else             list.last = list.first   = this;
}

/*
    Синхронизация
*/

void SceneObject::Update ()
{
  try
  {
    need_update = false;

    UpdateCore ();
  }
  catch (xtl::exception& e)
  {
    try
    {
      stl::string name = NameCore ();

      e.touch ("render::scene::client::SceneObject::Update(%s)", name.c_str ());
      throw;
    }
    catch (...)
    {
      e.touch ("render::scene::client::SceneObject::Update");
      throw;
    }
  }
}
