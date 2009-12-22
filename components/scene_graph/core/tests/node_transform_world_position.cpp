#include "shared.h"

int main ()
{
  printf ("Results of node_transform_world_position_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  node->BindToParent (*parent);
    
  printf ("set position vec3f result: ");
  node->SetPosition (1, 0, 0);
  dump_position (*node);
  printf ("\n");
  
  printf ("translate parent\n");
  
  parent->Translate (0, 2, 0);
  
  printf ("parent position: ");
  dump_position (*parent);
  printf ("\nnode position: ");
  dump_position (*node);
  printf ("\n");
  
  printf ("rotate parent\n");
  
  parent->Rotate (degree (180.f), 0, 1, 0);
  
  printf ("parent orientation: ");
  dump_orientation (*parent);
  printf ("\nnode position: ");
  dump_position (*node);
  printf ("\n");  
  
  printf ("scale parent\n");
  
  parent->Scale (2, 1, 1);
  
  printf ("parent scale: ");
  dump_scale (*parent);
  printf ("\nnode position: ");
  dump_position (*node);
  printf ("\n");  

  return 0;
}
