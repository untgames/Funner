#include "shared.h"

int main ()
{
  printf ("Results of look_to_node_point test:\n");

  Scene scene;

  Node::Pointer node1 = Node::Create ();

  node1->BindToScene (scene);
  
  LookToNodePoint::Pointer mover1 = LookToNodePoint::Create (*node1);

  LinearAccelerationEvaluator acceleration;

  acceleration.SetAcceleration (10);
  acceleration.SetDeceleration (10);
  acceleration.SetMaxSpeed (5);

  mover1->SetAccelerationHandler (acceleration);

  mover1->Start (&scene.Root (), math::vec3f (10, 0, 0), NodeOrt_Z, NodeOrt_Y);
  
  scene.Root ().Update (0.2f);

  printf ("node1 orientation is %f %f %f %f\n", node1->WorldOrientation ().x, node1->WorldOrientation ().y, node1->WorldOrientation ().z, node1->WorldOrientation ().w);

  mover1->Stop ();

  scene.Root ().Update (0.1f);

  printf ("node1 orientation is %f %f %f %f\n", node1->WorldOrientation ().x, node1->WorldOrientation ().y, node1->WorldOrientation ().z, node1->WorldOrientation ().w);

  return 0;
}
