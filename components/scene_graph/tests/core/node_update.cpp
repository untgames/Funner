#include "shared.h"

template <int id> void my_updater (float dt)
{
  printf ("my_updater%d(%.2f)\n", id, dt);
}

int main ()
{
  printf ("Results of node_update_test:\n");
  
  Node::Pointer parent = Node::Create ();
  Node::Pointer child  = Node::Create ();  

  printf ("update parent without controller & child with controller\n");
  
  DefaultController::Pointer child_controller = child->AttachController (&my_updater<2>);    

  child->BindToParent (*parent);

  parent->Update (0.5f);
  
  printf ("update parent with controller & child with controller\n");  
  
  DefaultController::Pointer parent_controller = parent->AttachController (&my_updater<1>);
  
  parent->Update (0.5f);
  
  printf ("update bottom-to-top\n");
  
  parent->Update (0.5f, NodeTraverseMode_BottomToTop);

  printf ("update only parent\n");
  
  parent->Update (0.5f, NodeTraverseMode_OnlyThis);  
  
  printf ("disconnect parent & update parent\n");
  
  parent_controller->Detach ();
  
  parent->Update (0.5f);
  
  printf ("create parent of parent & update it\n");
  
  Node::Pointer parent_parent = Node::Create ();  
  
  parent->BindToParent (*parent_parent);
  
  parent_parent->Update (0.5f);
  
  printf ("disconnect child & update parent of parent\n");
  
  child_controller->Detach ();
  
  parent_parent->Update (0.5f);
  
  printf ("yet one update\n");
  
  parent_parent->Update (0.5f); //для проверки отсоединения от списка обновлений

  return 0;
}
