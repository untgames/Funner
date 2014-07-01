#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации сцены
*/

namespace
{

typedef stl::vector<object_id_t> ObjectIdArray;

struct NodeCollector
{
  SceneManager*  scene_manager;
  Scene*         scene;
  ObjectIdArray* ids;

  NodeCollector (SceneManager& in_scene_manager, Scene& in_scene, ObjectIdArray& in_ids) : scene_manager (&in_scene_manager), scene (&in_scene), ids (&in_ids) {}

  void operator () (scene_graph::Entity& entity)
  {
    try
    {
      NodePtr node = scene_manager->GetNode (entity);

      if (!node)
        return;

      ids->push_back (node->Id ());

      node->SetSceneOwner (scene, false);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::NodeCollector::operator ()");
      throw;
    }
  }
};

struct NodeSceneReseter
{
  SceneManager* scene_manager;

  NodeSceneReseter (SceneManager& in_scene_manager) : scene_manager (&in_scene_manager) {}

  void operator () (scene_graph::Entity& entity)
  {
    try
    {
      NodePtr node = scene_manager->FindNode (entity);

      if (!node)
        return;

      node->SetSceneOwner (0, true);
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::scene::server::NodeSceneReseter::operator ()");
      throw;
    }
  }
};

}

struct Scene::Impl
{
  object_id_t          id;                      //идентификатор сцены
  scene_graph::Scene&  scene;                   //сцена
  SceneManager&        scene_manager;           //менеджер сцен
  xtl::auto_connection on_node_bind_connection; //оповещение о появлении нового узла в сцене

/// Конструктор
  Impl (Scene& owner, scene_graph::Scene& in_scene, SceneManager& in_scene_manager, object_id_t in_id)
    : id (in_id)
    , scene (in_scene)
    , scene_manager (in_scene_manager)
  {
    on_node_bind_connection = scene.Root ().RegisterEventHandler (scene_graph::NodeSubTreeEvent_AfterBind, xtl::bind (&Impl::OnNodeBinded, this, _2));

    scene_manager.Context ().CreateScene (id);
    scene_manager.Context ().SetSceneName (id, scene.Name ());

    ObjectIdArray ids;

    ids.reserve (scene.EntitiesCount ());

    try
    {
      scene.Traverse (NodeCollector (scene_manager, owner, ids));

      scene_manager.Context ().SetSceneNodes (id, render::scene::interchange::RawArray<object_id_t> (&ids [0], ids.size ()));
    }
    catch (...)
    {
      scene.Traverse (NodeSceneReseter (scene_manager));

      throw;
    }
  }

/// Деструктор
  ~Impl ()
  {
    try
    {
      scene_manager.Context ().DestroyScene (id);

      scene.Traverse (NodeSceneReseter (scene_manager));
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
      scene_manager.GetNode (src_node);
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
    impl.reset (new Impl (*this, scene, scene_manager, id));
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
