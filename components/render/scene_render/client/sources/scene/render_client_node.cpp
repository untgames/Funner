#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации узла
*/

struct Node::Impl
{  
  Node&                 owner;                         //обратная ссылки
  scene_graph::Node&    node;                          //исходный узел
  SceneManager&         scene_manager;                 //менеджер сцен
  Scene*                scene;                         //текущая сцена
  interchange::NodeType node_type;                     //тип узла
  object_id_t           id;                            //идентификатор объекта
  xtl::auto_connection  on_update_connection;          //оповещение об обновлении объекта  
  xtl::auto_connection  on_update_world_tm_connection; //оповещение об обновлении матрицы мировых преобразований
  xtl::auto_connection  on_scene_changed_connection;   //оповещение об изменении принадлежности сцене
  size_t                name_hash;                     //хэш имени узла  
  bool                  need_update_world_tm;          //требуется обновление матрицы преобразований
  bool                  need_update_scene;             //требуется обновление сцены

/// Конструктор
  Impl (Node& in_owner, scene_graph::Node& in_node, SceneManager& in_scene_manager, interchange::NodeType in_node_type)
    : owner (in_owner)
    , node (in_node)
    , scene_manager (in_scene_manager)
    , scene ()
    , node_type (in_node_type)
    , id (scene_manager.Client ().AllocateId (ObjectType_Node))
    , name_hash (0)
    , need_update_world_tm (true)
    , need_update_scene (true)
  {
    scene_manager.Context ().CreateNode (id, node_type);
  }

/// Деструктор
  ~Impl ()
  {
    try
    {
      scene_manager.Context ().DestroyNode (id);
    }
    catch (...)
    {
    }
  }

/// Оповещение об обновлении матрицы преобразований
  void UpdateWorldMatrixNotify ()
  {
    need_update_world_tm = true;
  }

/// Изменение сцены
  void SetScene (Scene* new_scene)
  {
    if (new_scene == scene)
      return;

    scene             = new_scene;
    need_update_scene = true;

    owner.UpdateNotify ();
  }

/// Оповещение об обновлении сцены
  void OnSceneChanged ()
  {
    scene_graph::Scene* sg_scene = node.Scene ();

    if (!sg_scene)
    {
      SetScene (0);
      return;
    }

    ScenePtr new_scene = scene_manager.FindScene (*sg_scene);

    if (!new_scene)
    {
      SetScene (0);
      return;
    }

    SetScene (new_scene.get ());
  }
};

/*
    Конструктор / деструктор
*/

Node::Node (scene_graph::Node& node, SceneManager& scene_manager, render::scene::interchange::NodeType node_type)
  : SceneObject (scene_manager)
{
  try
  {
    impl.reset (new Impl (*this, node, scene_manager, node_type));

    impl->on_update_connection          = node.RegisterEventHandler (scene_graph::NodeEvent_AfterUpdate, xtl::bind (&Node::UpdateNotify, static_cast<SceneObject*> (this)));
    impl->on_update_world_tm_connection = node.RegisterEventHandler (scene_graph::NodeEvent_AfterWorldTransformUpdate, xtl::bind (&Impl::UpdateWorldMatrixNotify, &*impl));
    impl->on_scene_changed_connection   = node.RegisterEventHandler (scene_graph::NodeEvent_AfterSceneChange, xtl::bind (&Impl::OnSceneChanged, &*impl));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Node::Node");
    throw;
  }
}

Node::~Node ()
{
}

/*
    Идентификатор узла / тип узла
*/

object_id_t Node::Id () const
{
  return impl->id;
}

render::scene::interchange::NodeType Node::Type () const
{
  return impl->node_type;
}

/*
    Исходный узел
*/

scene_graph::Node& Node::SourceNode () const
{
  return impl->node;
}

/*
    Менеджер сцены
*/

SceneManager& Node::Scenes () const
{
  return impl->scene_manager;
}

/*
    Реализация синхронизации
*/

void Node::UpdateCore (render::scene::client::Context& context)
{
  try
  {
      //обновление сцены

    if (impl->need_update_scene)
    {
      context.SetNodeScene (impl->id, impl->scene ? impl->scene->Id () : 0);

      impl->need_update_scene = false;
    }

      //обновление имени узла

    size_t name_hash = impl->node.NameHash ();

    if (impl->name_hash != impl->node.NameHash ())
    {
      context.SetNodeName (impl->id, impl->node.Name ());

      impl->name_hash = name_hash;
    }

      //обновление матрицы преобразований

    if (impl->need_update_world_tm)
    {
      context.SetNodeWorldMatrix (impl->id, impl->node.WorldTM ());

      impl->need_update_world_tm = false;
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::scene::client::Node::UpdateCore");
    throw;
  }
}

/*
    Имя объекта
*/

const char* Node::NameCore ()
{
  return impl->node.Name ();
}

/*
    Указатель на сцену (может быть нулевым)
*/

Scene* Node::SceneOwner () const
{
  return impl->scene;
}

void Node::ResetSceneOwner ()
{
  impl->SetScene (0);
}
