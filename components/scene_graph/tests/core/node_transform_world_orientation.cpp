#include "shared.h"

int main ()
{
  printf ("Results of node_transform_world_orientation_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  node->BindToParent (*parent);
    
  printf ("set orientation result: ");
  node->SetOrientation (30, 0, 0);
  dump_orientation (*node);
  printf ("\n");

  printf ("translate parent\n");
  
  parent->Translate (0, 2, 0);
  
  printf ("parent position: ");
  dump_position (*parent);
  printf ("\nnode orientation: ");
  dump_orientation (*node);
  printf ("\n");
  
  printf ("rotate parent\n");
  
  parent->Rotate (30, 0, 0);
  
  printf ("parent orientation: ");
  dump_orientation (*parent);
  printf ("\nnode orientation: ");
  dump_orientation (*node);
  printf ("\n");
  
  printf ("scale parent\n");
  
  parent->Scale (2, 1, 1);
  
  printf ("parent scale: ");
  dump_scale (*parent);
  printf ("\nnode orientation: ");
  dump_orientation (*node);
  printf ("\n");
  
  printf ("no parent-orientation inherit\n");
  
  node->SetOrientationInherit (false);
  
  printf ("node orientation: ");
  dump_orientation (*node);
  printf ("\n");

  return 0;
}
