#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации сцены
*/

struct Scene::Impl
{
  object_id_t          id;                      //идентификатор сцены
  scene_graph::Scene&  scene;                   //сцена
  SceneManager&        scene_manager;           //менеджер сцен
  xtl::auto_connection on_node_bind_connection; //оповещение о появлении нового узла в сцене

/// Конструктор
  Impl (scene_graph::Scene& in_scene, SceneManager& in_scene_manager, object_id_t in_id)
    : id (in_id)
    , scene (in_scene)
    , scene_manager (in_scene_manager)
  {
    on_node_bind_connection = scene.Root ().RegisterEventHandler (scene_graph::NodeSubTreeEvent_AfterBind, xtl::bind (&Impl::OnNodeBinded, this, _2));

    scene_manager.Context ().CreateScene (id);
    scene_manager.Context ().SetSceneName (id, scene.Name ());
  }

/// Деструктор
  ~Impl ()
  {
    try
    {
      scene_manager.Context ().DestroyScene (id);
    }
    catch (...)
    {
    }
  }

/// Оповещение о появлении нового узла в сцене
  void OnNodeBinded (scene_graph::Node& src_node)
  {
    try
    {
      NodePtr node = scene_manager.GetNode (src_node);

      if (!node)
        return;

      ///???????????????? attach to scene
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::client::Scene::Impl::OnNodeBinded");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

Scene::Scene (scene_graph::Scene& scene, SceneManager& scene_manager, object_id_t id)
{
  try
  {
    impl.reset (new Impl (scene, scene_manager, id));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Scene::Scene");
    throw;
  }
}

Scene::~Scene ()
{
}

/*
    Идентификатор сцены
*/

object_id_t Scene::Id ()
{
  return impl->id;
}

/*
    Сцена-источник
*/

scene_graph::Scene& Scene::SourceScene ()
{
  return impl->scene;
}
