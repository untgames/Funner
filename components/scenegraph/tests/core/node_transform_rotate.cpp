#include "shared.h"

void dump_state (Node& node, Node& parent)
{
  printf ("  parent orientation: ");
  dump_orientation (parent);
  printf ("\n");  
  printf ("  node orientation: ");
  dump_orientation (node);
  printf ("\n");
}

int main ()
{
  printf ("Results of node_transform_rotate_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  node->BindToParent (*parent);
  node->SetOrientation (0, 60, 0);
  
  parent->SetScale (1, 2, 3);
  parent->SetPosition (3, 4, 5);
  parent->SetOrientation (90, 0, 0);
  
  printf ("initial state\n");
  
  dump_state (*node, *parent);
 
  printf ("rotate node in parent space\n");
  
  node->Rotate (-90, 0, -60, NodeTransformSpace_Parent); 

  dump_state (*node, *parent);
  
  printf ("rotate node in local space\n");
  
  node->Rotate (0, 30, 0, NodeTransformSpace_Local);
  
  dump_state (*node, *parent);  
 
  printf ("rotate node in world space\n");
  
  node->Rotate (0, 0, 90, NodeTransformSpace_World);
  
  dump_state (*node, *parent);

  return 0;
}
