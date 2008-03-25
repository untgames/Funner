#include "shared.h"

int main ()
{
  printf ("Results of node_transform_position_test:\n");
  
  Node::Pointer node (Node::Create ());
    
  printf ("set position vec3f result: ");
  node->SetPosition (vec3f (14.4f, 17.1f, -6.66f));
  dump_position (*node);
  printf ("\n");

  printf ("set position (x, y, z) result: ");
  node->SetPosition (0.f, 27.14f, 6.12f);
  dump_position (*node);
  printf ("\n");  

  printf ("reset position result: ");
  node->ResetPosition ();
  dump_position (*node);  
  printf ("\n");

  return 0;
}
