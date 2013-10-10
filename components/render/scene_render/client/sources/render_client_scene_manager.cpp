#include "shared.h"

using namespace render::scene::client;

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
  object_id_t current_id; //текущий доступный идентификатор
  SceneMap    scenes;     //сцены

/// Конструктор
  Impl ()
    : current_id ()
  {
  }

/// Оповещение об удалении сцены
  void OnDestroyScene (scene_graph::Scene* scene)
  {
    if (!scene)
      return;

    scenes.erase (scene);
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

    ScenePtr new_scene (new Scene (scene, connection, id + 1), false);

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
    Синхронизация сцен
*/

void SceneManager::Update ()
{
  try
  {
    for (SceneMap::iterator iter=impl->scenes.begin (), end=impl->scenes.end (); iter!=end; ++iter)
      iter->second.scene->Update ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::SceneManager::Update");
    throw;
  }
}
