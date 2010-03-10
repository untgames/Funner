#include "shared.h"

void dump_transform (Node& node)
{
  printf ("  position: ");
  dump_position (node);
  printf ("\n  orientation: ");
  dump_orientation (node);
  printf ("\n  scale: ");
  dump_scale (node);  
  printf ("\n");
}

int main ()
{
  printf ("Results of node_transform_translate_test:\n");
  
  Node::Pointer node (Node::Create ()), parent (Node::Create ());
  
  node->BindToParent (*parent);
    
  node->SetPosition (1, 1, 1);
  node->SetOrientation (degree (180.f), 0, 0, 1);

  printf ("parent:\n");
  
  parent->SetPosition    (1, 2, 3);
  parent->SetOrientation (degree (90.f), degree (0.f), degree (0.f));
  parent->SetScale       (2, 2, 2);
  
  dump_transform (*parent);

  printf ("node:\n");  

  dump_transform (*node);  
  
  printf ("translate node in parent space\n");
  
  node->Translate (1, 0, 0, NodeTransformSpace_Parent); 

  printf ("node:\n");  

  dump_transform (*node);  
  
  printf ("translate node in local space\n");
  
  node->Translate (0, 1, 0, NodeTransformSpace_Local);

  printf ("node:\n");  

  dump_transform (*node);    
  
  printf ("translate node in world space\n");
  
  node->Translate (1, 1, 1, NodeTransformSpace_World);

  printf ("node:\n");  

  dump_transform (*node);      

  return 0;
}
