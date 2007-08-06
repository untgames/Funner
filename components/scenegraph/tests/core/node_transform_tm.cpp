#include "shared.h"

typedef com_ptr<Node> NodePtr;

int main ()
{
  printf ("Results of node_transform_tm_test:\n");
  
  NodePtr node (Node::Create (), false), parent (Node::Create (), false);
  
  node->BindToParent (*parent);
    
  node->SetPosition    (1, 1, 1);
  parent->SetPosition    (2, 2, 2);
  parent->SetOrientation (90, 1, 0, 0);
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
