#include "shared.h"

int main ()
{
  printf ("Results of node_transform_scale_test:\n");
  
  Node::Pointer node (Node::Create ());
    
  printf ("set scale vec3f result: ");
  node->SetScale (vec3f (14.4f, 17.1f, -6.66f));
  dump_scale (*node);
  printf ("\n");

  printf ("set scale (sx, sy, sz) result: ");
  node->SetScale (0.f, 27.14f, 6.12f);
  dump_scale (*node);
  printf ("\n");

  printf ("reset scale result: ");
  node->ResetScale ();
  dump_scale (*node);
  printf ("\n");  

  return 0;
}
