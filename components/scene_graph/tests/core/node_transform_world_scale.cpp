#include "shared.h"

int main ()
{
  printf ("Results of node_transform_world_scale_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  node->BindToParent (*parent);
    
  printf ("set scale result: ");
  node->SetScale (1, 2, 3);
  dump_scale (*node);
  printf ("\n");

  printf ("translate parent\n");
  
  parent->Translate (0, 2, 0);
  
  printf ("parent position: ");
  dump_position (*parent);
  printf ("\nnode scale: ");
  dump_scale (*node);
  printf ("\n");
  
  printf ("rotate parent\n");
  
  parent->Rotate (degree (30.f), degree (0.f), degree (0.f));
  
  printf ("parent orientation: ");
  dump_orientation (*parent);
  printf ("\nnode scale: ");
  dump_scale (*node);
  printf ("\n");
  
  printf ("scale parent\n");
  
  parent->Scale (2, 3, -1);
  
  printf ("parent scale: ");
  dump_scale (*parent);
  printf ("\nnode scale: ");
  dump_scale (*node);
  printf ("\n");
  
  printf ("no parent-scale inherit\n");
  
  node->SetScaleInherit (false);
  
  printf ("node scale: ");
  dump_scale (*node);
  printf ("\n");

  return 0;
}
