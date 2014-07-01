#include "shared.h"

using namespace render::scene::server;
using namespace render::scene;

using interchange::object_id_t;

/*
    Описание реализации менеджера сцен
*/

typedef stl::hash_map<object_id_t, Scene>   SceneMap;
typedef stl::hash_map<object_id_t, NodePtr> NodeMap;

struct SceneManager::Impl
{
  RenderManager render_manager; //менеджер рендеринга
  SceneMap      scenes;         //сцены
  NodeMap       nodes;          //узлы
  object_id_t   cached_node_id; //закэшированный идентификатор узла (для последовательных обновлений одного узла)
  Node*         cached_node;    //закэшированный узел (для последовательных обновлений одного узла)

/// Конструктор
  Impl (const RenderManager& in_render_manager)
    : render_manager (in_render_manager)
    , cached_node_id ()
    , cached_node ()   
  {
  }
};

/*
    Конструктор / деструктор
*/

SceneManager::SceneManager (const RenderManager& render_manager)
  : impl (new Impl (render_manager))
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
    throw xtl::format_operation_exception ("render::scene::server::SceneManager::CreateScene", "Scene with id %llu has been already added", id);

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

/*
    Добавление и удаление узла
*/

xtl::intrusive_ptr<Node> SceneManager::CreateNode (interchange::object_id_t id, interchange::NodeType type)
{
  NodeMap::iterator iter = impl->nodes.find (id);

  if (iter != impl->nodes.end ())
    throw xtl::format_operation_exception ("render::scene::server::SceneManager::CreateNode", "Node with id %llu has been already added", id);

  NodePtr node (NodeFactory::CreateNode (impl->render_manager, type), false);  

  impl->nodes.insert_pair (id, node);

  impl->cached_node_id = id;
  impl->cached_node    = node.get ();

  return node;
}

void SceneManager::RemoveNode (interchange::object_id_t id)
{
  impl->nodes.erase (id);

  if (id == impl->cached_node_id)
  {
    impl->cached_node_id = 0;
    impl->cached_node    = 0;
  }
}

/*
    Поиск узла
*/

Node& SceneManager::GetNode (interchange::object_id_t id) const
{
  if (id == impl->cached_node_id && id)
    return *impl->cached_node;

  NodeMap::iterator iter = impl->nodes.find (id);

  if (iter == impl->nodes.end ())
    throw xtl::format_operation_exception ("render::scene::server::SceneManager::GetNode", "Node with id %llu has not been added", id);

  impl->cached_node_id = id;
  impl->cached_node    = &*iter->second;

  return *impl->cached_node;
}
