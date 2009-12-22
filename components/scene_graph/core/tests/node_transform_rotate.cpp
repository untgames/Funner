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
  node->SetOrientation (degree (0.f), degree (60.f), degree (0.f));
  
  parent->SetScale (1, 2, 3);
  parent->SetPosition (3, 4, 5);
  parent->SetOrientation (degree (90.0f), degree (0.0f), degree (0.0f));
  
  printf ("initial state\n");
  
  dump_state (*node, *parent);
 
  printf ("rotate node in parent space\n");
  
  node->Rotate (degree (-90.0f), degree (0.0f), degree (-60.0f), NodeTransformSpace_Parent); 

  dump_state (*node, *parent);
  
  printf ("rotate node in local space\n");
  
  node->Rotate (degree (0.0f), degree (30.0f), degree (0.f), NodeTransformSpace_Local);
  
  dump_state (*node, *parent);  
 
  printf ("rotate node in world space\n");
  
  node->Rotate (degree (0.0f), degree (0.f), degree (90.f), NodeTransformSpace_World);
  
  dump_state (*node, *parent);

  return 0;
}
