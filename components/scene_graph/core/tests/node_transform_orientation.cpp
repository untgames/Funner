#include "shared.h"

int main ()
{
  printf ("Results of node_transform_orientation_test:\n");
  
  Node::Pointer node (Node::Create ());
    
  printf ("set orientation from axis angle result: ");
  node->SetOrientation (degree (-390.0f), 0, 0, 1);
  dump_orientation (*node);
  printf ("\n");

  printf ("set orientation euler angle result: ");
  node->SetOrientation (degree (10.0f), degree (20.0f), degree (30.0f));
  dump_orientation (*node);
  printf ("\n");  

  printf ("reset orientation result: ");    
  node->ResetOrientation ();
  dump_orientation (*node);
  printf ("\n");  

  return 0;
}
