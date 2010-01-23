#include "shared.h"

int main ()
{
  printf ("Results of node_transform_pivot_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  parent->SetOrientation (degree (0.f), degree (0.f), degree (90.f));  
  parent->SetPivotPosition (1.0f, 0.0f, 0.0f);
  node->SetPosition (2.0f, 0.0f, 0.0f);
  node->SetOrientation (degree (0.f), degree (0.f), degree (90.f));
  
  node->BindToParent (*parent);  
    
  printf ("parent position: ");
  dump_position (*parent);
  printf ("\n");
  
  printf ("node position: ");
  dump_position (*node);
  printf ("\n");
  
  printf ("test point: ");
  
  math::vec3f test = node->WorldTM () * math::vec3f (2.0f, 0.0f, 0.0f);
  
  dump (test);  
  
  printf ("\n");  
  
  math::vec3f saved_position = node->Position ();
  
  printf ("test local translate: ");
  
  node->Translate (0.0f, -2.0f, 0.0f, NodeTransformSpace_Local);
  
  dump_position (*node);
  printf ("\n");  
  
  printf ("restore position: ");

  node->SetPosition (saved_position);

  dump_position (*node);
  printf ("\n");  
  
  printf ("test parent translate: ");
  
  node->Translate (0.0f, -2.0f, 0.0f, NodeTransformSpace_Parent);  
  
  dump_position (*node);
  printf ("\n");  
  
  node->SetPosition (saved_position);

  printf ("test world translate: ");    
  
  node->Translate (0.0f, -2.0f, 0.0f, NodeTransformSpace_World);  
  
  dump_position (*node);
  printf ("\n");    
  
  printf ("create sub child\n");
  
  Node::Pointer child (Node::Create ());
  
  child->BindToParent (*node);
 
  node->SetPosition (2.0f, 0.0f, 0.0f);
  node->SetPivotPosition (1.0f, 0.0f, 0.0f);
  node->SetScale (2.0f, 2.0f, 1.0f);
  child->SetPosition    (-1.0f, -1.0f, 0.0f);
  child->SetPivotPosition (-1.0f, 0.0f, 0.0f);
  child->SetOrientation (degree (0.0f), degree (0.0f), degree (90.0f));

  printf ("test node position: ");
  dump_position (*node);
  printf ("\n");  
  
  printf ("test child position: ");
  dump_position (*child);
  printf ("\n");  
  
  printf ("test point: ");
  
  test = child->WorldTM () * math::vec3f (1.0f, 1.0f, 0.0f);
  
  dump (test);
  
  printf ("\n");  
  
  return 0;
}
