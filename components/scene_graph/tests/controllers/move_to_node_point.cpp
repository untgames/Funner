#include "shared.h"

int main ()
{
  printf ("Results of move_to_node_point test:\n");

  Scene scene;

  Node::Pointer node1 = Node::Create (), node2 = Node::Create ();

  node1->BindToScene (scene);
  node2->BindToParent (*node1);
  
  MoveToNodePoint::Pointer mover1 = MoveToNodePoint::Create (*node1), mover2 = MoveToNodePoint::Create (*node2);
  
  LinearAccelerationEvaluator acceleration;

  acceleration.SetAcceleration (1);
  acceleration.SetDeceleration (1);
  acceleration.SetMaxSpeed (0.5);

  mover1->SetAccelerationHandler (acceleration);
  mover2->SetAccelerationHandler (acceleration);

  mover1->Start (&scene.Root (), math::vec3f (10, 0, 0));
  mover2->Start (node1, math::vec3f (0, 10, 0));
  
  scene.Root ().Update (1.0f);

  printf ("node1 position is %f %f %f, node2 position is %f %f %f\n", node1->WorldPosition ().x, node1->WorldPosition ().y, node1->WorldPosition ().z, node2->WorldPosition ().x, node2->WorldPosition ().y, node2->WorldPosition ().z);

  mover1->Stop ();
  mover2->Stop ();

  scene.Root ().Update (1.0f);

  printf ("node1 position is %f %f %f, node2 position is %f %f %f\n", node1->WorldPosition ().x, node1->WorldPosition ().y, node1->WorldPosition ().z, node2->WorldPosition ().x, node2->WorldPosition ().y, node2->WorldPosition ().z);

  return 0;
}
