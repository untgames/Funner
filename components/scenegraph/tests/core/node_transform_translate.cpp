#include "shared.h"

typedef com_ptr<Node> NodePtr;

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
  
  NodePtr node (Node::Create (), false), parent (Node::Create (), false);
  
  node->BindToParent (*parent);
    
  node->SetPosition (1, 1, 1);
  node->SetOrientation (180, 0, 0, 1);

  printf ("parent:\n");
  
  parent->SetPosition    (1, 2, 3);
  parent->SetOrientation (90, 0, 0);
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
