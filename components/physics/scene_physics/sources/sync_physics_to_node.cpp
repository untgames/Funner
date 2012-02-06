#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;
using namespace physics;

/*
   Контроллер устанавливающий трансформацию узла в соответствии с трансформацией тела
*/

struct SyncPhysicsToNode::Impl
{
  Node*                node;
  physics::RigidBody   body;
  bool                 needs_update;
  xtl::auto_connection body_update_connection;

  Impl (Node& in_node, const physics::RigidBody& in_body)
    : node (&in_node)
    , body (in_body)
    , needs_update (true)
  {
    body_update_connection = body.RegisterTransformUpdateCallback (xtl::bind (&SyncPhysicsToNode::Impl::OnTransformUpdate, this));
  }

  void OnTransformUpdate ()
  {
    needs_update = true;
  }

  void Update ()
  {
    if (!needs_update)
      return;

    const Transform& body_transform = body.WorldTransform ();

    node->SetWorldPosition    (body_transform.position);
    node->SetWorldOrientation (body_transform.orientation);

    needs_update = false;
  }
};

/*
   Конструктор / деструктор
*/

SyncPhysicsToNode::SyncPhysicsToNode (Node& node, const physics::RigidBody& body)
  : Controller (node)
  , impl (new Impl (node, body))
  {}

SyncPhysicsToNode::~SyncPhysicsToNode ()
{
}

/*
   Создание контроллера
*/

SyncPhysicsToNode::Pointer SyncPhysicsToNode::Create (Node& node, const physics::RigidBody& body)
{
  return Pointer (new SyncPhysicsToNode (node, body), false);
}

/*
   Тело, с которым связан контроллер
*/

const physics::RigidBody& SyncPhysicsToNode::AttachedBody () const
{
  return impl->body;
}

/*
   Обновление
*/

void SyncPhysicsToNode::Update (const TimeValue&)
{
  impl->Update ();
}
