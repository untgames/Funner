#include "shared.h"

int main ()
{
  printf ("Results of node_transform_pivot_inherit_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  parent->SetOrientation (degree (0.f), degree (0.f), degree (90.f));  
  parent->SetScale (2.0f, 2.0f, 1.0f);
  node->SetPosition (2.0f, 0.0f, 0.0f);
  node->SetPivotPosition (1.0f, 0.0f, 0.0f);  
  node->SetOrientation (degree (0.f), degree (0.f), degree (90.f));  
  node->SetScaleInherit (false);

  node->BindToParent (*parent);  

  printf ("parent position: ");
  dump_position (*parent);
  printf ("\n");
  
  printf ("node position: ");
  dump_position (*node);
  printf ("\n");
  
  printf ("test point (no scale inherit): ");

  math::vec3f test = node->WorldTM () * math::vec3f (2.0f, 0.0f, 0.0f);

  dump (test);  

  printf ("\n");  
  
  node->SetOrientationInherit (false);    
    
  printf ("test point (no orientation inherit): ");

  test = node->WorldTM () * math::vec3f (2.0f, 0.0f, 0.0f);

  dump (test);  
    
  return 0;
}
