#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

using interchange::object_id_t;

/*
    Описание реализации менеджера сцен
*/

typedef stl::hash_map<object_id_t, Scene> SceneMap;

struct SceneManager::Impl
{
  SceneMap scenes; //сцены
};

/*
    Конструктор / деструктор
*/

SceneManager::SceneManager  ()
  : impl (new Impl)
{
}

SceneManager::~SceneManager ()
{
}

/*
    Добавление и удаление сцен
*/

Scene SceneManager::CreateScene (object_id_t id)
{
  SceneMap::iterator iter = impl->scenes.find (id);

  if (iter != impl->scenes.end ())
    throw xtl::format_operation_exception ("render::scene::server::SceneManager::GetScene", "Scene with id %llu has been already added", id);

  return impl->scenes.insert_pair (id, Scene ()).first->second;
}

void SceneManager::RemoveScene (object_id_t id)
{
  impl->scenes.erase (id);
}

/*
    Поиск сцены
*/

Scene& SceneManager::GetScene (object_id_t id) const
{
  SceneMap::iterator iter = impl->scenes.find (id);

  if (iter == impl->scenes.end ())
    throw xtl::format_operation_exception ("render::scene::server::SceneManager::GetScene", "Scene with id %llu has not been added", id);

  return iter->second;
}
