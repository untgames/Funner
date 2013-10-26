#include "shared.h"

using namespace render::scene::client;

/*
    Описание реализации узла
*/

struct Node::Impl
{
  scene_graph::Node&   node;                  //исходный узел
  xtl::auto_connection on_update_connection;  //оповещение об обновлении объекта

/// Конструктор
  Impl (scene_graph::Node& in_node)
    : node (in_node)
  {
  }
};

/*
    Конструктор / деструктор
*/

Node::Node (scene_graph::Node& node, SceneManager& scene_manager)
  : SceneObject (scene_manager)
  , impl (new Impl (node))
{
  impl->on_update_connection = node.RegisterEventHandler (scene_graph::NodeEvent_AfterUpdate, xtl::bind (&Node::UpdateNotify, static_cast<SceneObject*> (this)));
}

Node::~Node ()
{
}

/*
    Исходный узел
*/

scene_graph::Node& Node::SourceNode ()
{
  return impl->node;
}

/*
    Реализация синхронизации
*/

void Node::UpdateCore (render::scene::client::Context& context)
{
  //?????????????
}

/*
    Имя объекта
*/

const char* Node::NameCore ()
{
  return impl->node.Name ();
}
