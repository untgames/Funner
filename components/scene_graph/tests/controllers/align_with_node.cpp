#include "shared.h"

int main ()
{
  printf ("Results of look_to_node_point test:\n");

  Scene scene;

  Node::Pointer node1 = Node::Create (), node2 = Node::Create ();

  node1->BindToScene (scene);
  node2->BindToScene (scene);
  
  AlignWithNode::Pointer mover1 = AlignWithNode::Create (*node2);

  LinearAccelerationEvaluator acceleration;

  acceleration.SetAcceleration (10);
  acceleration.SetDeceleration (10);
  acceleration.SetMaxSpeed (5);

  mover1->SetAccelerationHandler (acceleration);

  mover1->Start (node1, NodeOrt_Z, NodeOrt_Z, NodeOrt_X);

  node1->SetOrientation (math::degree (90.f), math::degree (0.f), math::degree (0.f));
  
  scene.Root ().Update (0.2f);

  printf ("node2 orientation is %f %f %f %f\n", node2->WorldOrientation ().x, node2->WorldOrientation ().y, node2->WorldOrientation ().z, node2->WorldOrientation ().w);

  mover1->Stop ();

  scene.Root ().Update (0.1f);

  printf ("node2 orientation is %f %f %f %f\n", node2->WorldOrientation ().x, node2->WorldOrientation ().y, node2->WorldOrientation ().z, node2->WorldOrientation ().w);

  return 0;
}
