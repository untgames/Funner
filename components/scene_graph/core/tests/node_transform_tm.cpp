#include "shared.h"

int main ()
{
  printf ("Results of node_transform_tm_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  node->BindToParent (*parent);
    
  node->SetPosition    (1, 1, 1);
  parent->SetPosition    (2, 2, 2);
  parent->SetOrientation (degree (90.f), 1, 0, 0);
  parent->SetScale       (2, 2, 2);
  
  vec3f test_point;
  vec4f test_vec (1, 0, 0, 0);

  printf ("test-point: local=");
  dump   (node->LocalTM () * test_point);
  printf (" world=");
  dump   (node->WorldTM () * test_point);
  printf ("\n");
  printf ("test-vec: local=");
  dump   (node->LocalTM () * test_vec);
  printf (" world=");
  dump   (node->WorldTM () * test_vec);
  printf ("\n");    

  return 0;
}
