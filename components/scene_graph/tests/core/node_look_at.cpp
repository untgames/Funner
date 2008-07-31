#include "shared.h"

int main ()
{
  printf ("Results of node_look_at_test:\n");
  
  Node::Pointer parent (Node::Create ());  
  
  parent->SetScale (-1, 1, 1);
  
  Node::Pointer node (Node::Create ());

  node->BindToParent (*parent);
  node->LookAt (0, 1, 0, 1, 1, 0, 0, 1, 0, NodeTransformSpace_World);

  printf ("local_position: ");
  dump   (node->Position ());
  printf ("\nlocal_ort_x:    ");
  dump   (node->LocalOrtX ());
  printf ("\nlocal_ort_y:    ");
  dump   (node->LocalOrtY ());
  printf ("\nlocal_ort_z:    ");
  dump   (node->LocalOrtZ ());
  printf ("\n");

  return 0;
}
