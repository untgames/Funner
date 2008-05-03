#include "shared.h"

int main ()
{
  printf ("Results of node_transform_orientation_test:\n");
  
  Node::Pointer node (Node::Create ());
    
  printf ("set orientation from axis angle result: ");
  node->SetOrientation (-390, 0, 0, 1);
  dump_orientation (*node);
  printf ("\n");

  printf ("set orientation euler angle result: ");
  node->SetOrientation (10, 20, 30);
  dump_orientation (*node);
  printf ("\n");  

  printf ("reset orientation result: ");    
  node->ResetOrientation ();
  dump_orientation (*node);
  printf ("\n");  

  return 0;
}
