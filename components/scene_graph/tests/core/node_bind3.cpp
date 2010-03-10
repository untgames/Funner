#include "shared.h"

int main ()
{
  printf ("Results of node_bind3_test:\n");
  
  Node::Pointer node (Node::Create ()), parent1 (Node::Create ()), parent2 (Node::Create ());

  node->SetPosition    (1, 2, 3);
  node->SetOrientation (degree (90.0f), 1, 0, 0);
  node->SetScale       (-1, 2, 3);
  
  parent1->SetPosition    (-1, -5, 2);
  parent1->SetOrientation (degree (90.0f), 0, 1, 0);
  parent1->SetScale       (-1, -2, 1);  
  
  parent1->SetPosition    (8, 2, 3);
  parent1->SetOrientation (degree (90.0f), 0, 0, 1);
  parent1->SetScale       (2, 3, 1);
  
  node->BindToParent (*parent1);
  
  printf           ("before\n  position:    ");
  dump_position    (*node);
  printf           ("\n  orientation: ");
  dump_orientation (*node);
  printf           ("\n  scale:       ");
  dump_scale       (*node);  
  printf           ("\n");  
  
  node->BindToParent (*parent2, NodeBindMode_AddRef, NodeTransformSpace_World);
  
  printf           ("after rebind\n  position:    ");
  dump_position    (*node);
  printf           ("\n  orientation: ");  
  dump_orientation (*node);
  printf           ("\n  scale:       ");  
  dump_scale       (*node);  
  printf           ("\n");  
  
  node->Unbind (NodeTransformSpace_World);
  
  printf           ("after unbind\n  position:    ");
  dump_position    (*node);
  printf           ("\n  orientation: ");  
  dump_orientation (*node);
  printf           ("\n  scale:       ");  
  dump_scale       (*node);  
  printf           ("\n");    

  return 0;
}
