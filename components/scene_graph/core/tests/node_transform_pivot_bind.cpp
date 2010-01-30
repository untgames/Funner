#include "shared.h"

int main ()
{
  printf ("Results of node_transform_pivot_bind_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  parent->SetScale (3.0f, 3.0f, 1.0f);
  parent->SetPivotPosition (0.0f, 1.0f, 0.0f);  
  node->SetPosition (2.0f, 0.0f, 0.0f);
  node->SetPivotPosition (1.0f, 0.0f, 0.0f);
  node->SetScaleInherit (false);
  
  printf ("position before: ");
  dump (node->WorldTM () * vec3f (1.0f, 0.0f, 0.0f));
  printf ("\n");

  node->BindToParent (*parent, NodeBindMode_Default, NodeTransformSpace_World);
  
  printf ("position after bind: ");
  dump (node->WorldTM () * vec3f (1.0f, 0.0f, 0.0f));
  printf ("\n");
  
  node->Rotate (degree (0.0f), degree (0.0f), degree (90.0f));
  
  printf ("position after rotate: ");
  dump (node->WorldTM () * vec3f (1.0f, 0.0f, 0.0f));
  printf ("\n");
  
  return 0;
}
