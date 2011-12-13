#include "shared.h"

using namespace scene_graph::controllers;

int main ()
{
  printf ("Results of fall_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    physics::Scene* scene (new physics::Scene (manager.CreateScene ()));

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    RigidBody     body (scene->CreateRigidBody (sphere_shape, 1));
    Node::Pointer node (Node::Create ());

    printf ("Initial state:\n");
    dump_body_position (body);
    dump_node_position (*node);

    printf ("Simulating one second, attach controller\n");

    scene->PerformSimulation (1.f);

    SyncPhysicsToNode::Pointer controller (SyncPhysicsToNode::Create (*node, body));

    dump_body_position (body);
    dump_node_position (*node);

    printf ("Update node\n");

    node->Update (0.1f);

    dump_node_position (*node);

    printf ("Simulating one second\n");

    scene->PerformSimulation (1.f);

    dump_body_position (body);
    dump_node_position (*node);

    printf ("Update node\n");

    node->Update (0.1f);

    dump_node_position (*node);

    printf ("Update node\n");

    node->Update (0.1f);

    dump_node_position (*node);

    printf ("Simulating one second, detach controller\n");

    scene->PerformSimulation (1.f);

    controller->Detach ();

    dump_body_position (body);
    dump_node_position (*node);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
