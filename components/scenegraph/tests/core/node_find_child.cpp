#include "shared.h"

typedef com_ptr<Node> NodePtr;

void test (const Node& node, const char* child_name, NodeSearchMode mode)
{
  const Node* child = node.FindChild (child_name, mode);
  
  if (!child) printf ("  no child node with name '%s'\n", child_name);
  else        printf ("  child node '%s' found\n", child->Name ());
}

void dump (const Node& node, size_t level=1)
{
  for (size_t i=0; i<level; i++)
    printf (" ");
    
  printf ("node '%s'\n", node.Name ()); 
  
  for (const Node* child=node.FirstChild (); child; child=child->NextChild ())
    dump (*child, level+1);    
}

int main ()
{
  printf ("Results of node_find_child_test:\n");

  NodePtr node (Node::Create (), false), child1 (Node::Create (), false), child2 (Node::Create (), false);
  
  node->SetName ("node");
  child1->SetName ("child1");
  child2->SetName ("child2");    

  child2->BindToParent (*child1);
  child1->BindToParent (*node);
  
  printf ("entities tree:\n");
  
  dump (*node);

  printf ("search on all sublevels:\n");

  test (*node, "child1", NodeSearchMode_OnAllSublevels);
  test (*node, "child2", NodeSearchMode_OnAllSublevels);

  printf ("search on next sublevel:\n");

  test (*node, "child1", NodeSearchMode_OnNextSublevel);
  test (*node, "child2", NodeSearchMode_OnNextSublevel);

  return 0;
}
