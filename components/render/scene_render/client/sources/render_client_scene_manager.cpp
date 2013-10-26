#include "shared.h"

using namespace render::scene::client;

/*
    Константы
*/

namespace
{

const char* LOG_NAME = "render.scene.client"; //имя потока отладочного протоколирования

}

/*
    Описание реализации менеджера сцен
*/

namespace
{

struct SceneDesc
{
  Scene*               scene;
  xtl::auto_connection on_destroy_source_scene;
  xtl::auto_connection on_destroy_target_scene;

  SceneDesc (Scene* in_scene) : scene (in_scene) {}
};

typedef stl::hash_map<scene_graph::Scene*, SceneDesc> SceneMap;

}

struct SceneManager::Impl
{
  common::Log      log;         //поток отладочного протоколирования
  SceneUpdateList  update_list; //список обновлений
  object_id_t      current_id;  //текущий доступный идентификатор  
  SceneMap         scenes;      //сцены

/// Конструктор
  Impl ()
    : log (LOG_NAME)
    , current_id ()
  {
    ResetUpdateList ();
  }

/// Оповещение об удалении сцены
  void OnDestroyScene (scene_graph::Scene* scene)
  {
    if (!scene)
      return;

    scenes.erase (scene);
  }

/// Сброк списка обновлений
  void ResetUpdateList ()
  {
    update_list.first = update_list.last = 0;
  }
};

/*
    Конструктор / деструктор
*/

SceneManager::SceneManager ()
  : impl (new Impl)
{
}

SceneManager::~SceneManager ()
{
}

/*
    Получение сцены
*/

ScenePtr SceneManager::GetScene (scene_graph::Scene& scene, Connection& connection)
{
  try
  {
    SceneMap::iterator iter = impl->scenes.find (&scene);

    if (iter != impl->scenes.end ())
      return iter->second.scene;

    object_id_t& id = impl->current_id;

    if (!(id + 1))
      throw xtl::format_operation_exception ("", "ID pool is full for a new scene");

    ScenePtr new_scene (new Scene (scene, connection, impl->update_list, id + 1), false);

    xtl::trackable::function_type destroy_handler (xtl::bind (&Impl::OnDestroyScene, &*impl, &scene));

    iter = impl->scenes.insert_pair (&scene, SceneDesc (new_scene.get ())).first;

    try
    {
      iter->second.on_destroy_source_scene = scene.Root ().RegisterEventHandler (scene_graph::NodeEvent_BeforeDestroy, xtl::bind (&Impl::OnDestroyScene, &*impl, &scene));
      iter->second.on_destroy_target_scene = new_scene->connect_tracker (destroy_handler);

      ++id;

      return new_scene;
    }
    catch (...)
    {
      impl->scenes.erase (iter);
      throw;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::SceneManager::GetScene");
    throw;
  }
}

/*
    Список обновлений
*/

SceneUpdateList& SceneManager::UpdateList ()
{
  return impl->update_list;
}

/*
    Синхронизация сцен
*/

void SceneManager::Update ()
{
  SceneObject* first = impl->update_list.first;

  impl->ResetUpdateList ();

  for (SceneObject* object=first; object; object=object->next_update)
  {
    try
    {
      object->Update ();
    }
    catch (std::exception& e)
    {
      impl->log.Printf ("%s\n    at render::scene::client::SceneManager::Update", e.what ());
    }
    catch (...)
    {
      impl->log.Printf ("unknown exception\n    at render::scene::client::SceneManager::Update");
    }
  }
}
