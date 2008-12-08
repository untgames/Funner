#include "shared.h"

void test (const Node& node, const char* child_name, NodeSearchMode mode)
{
  Node::ConstPointer child = node.FindChild (child_name, mode);
  
  if (!child) printf ("  no child node with name '%s'\n", child_name);
  else        printf ("  child node '%s' found\n", child->Name ());
}

void dump (const Node& node, size_t level=1)
{
  for (size_t i=0; i<level; i++)
    printf (" ");
    
  printf ("node '%s'\n", node.Name ()); 
  
  for (Node::ConstPointer child=node.FirstChild (); child; child=child->NextChild ())
    dump (*child, level+1);    
}

int main ()
{
  printf ("Results of node_find_child_test:\n");

  Node::Pointer node (Node::Create ()), child1 (Node::Create ()), child2 (Node::Create ());
  
  node->SetName ("node");
  child1->SetName ("child1");
  child2->SetName ("child2");    

  child1->BindToParent (*node);
  child2->BindToParent (*node);
  
  printf ("entities tree:\n");
  
  dump (*node);

  printf ("unbind child2:\n");

  node->UnbindChild ("child2");

  dump (*node);
  node->UnbindAllChildren ();

  child1->BindToParent (*node);
  child2->BindToParent (*node);
  
  printf ("unbind child1:\n");

  node->UnbindChild ("child1");

  dump (*node);

  return 0;
}
