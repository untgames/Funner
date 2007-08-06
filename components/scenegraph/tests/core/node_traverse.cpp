#include "shared.h"

typedef com_ptr<Node> NodePtr;

void print (Node& node)
{
  printf ("node '%s'\n", node.Name ());
}

void cprint (const Node& node)
{
  printf ("node '%s'\n", node.Name ());
}

void dump (const Node& node, size_t level=1)
{
  for (size_t i=0; i<level; i++)
    printf (" ");
    
  cprint (node);
  
  for (const Node* child=node.FirstChild (); child; child=child->NextChild ())
    dump (*child, level+1);    
}

int main ()
{
  printf ("Results of node_traverse_test:\n");
  
  NodePtr node (Node::Create (), false), child1 (Node::Create (), false), child2 (Node::Create (), false);
  
  node->SetName ("node");
  child1->SetName ("child1");
  child2->SetName ("child2");    

  child2->BindToParent (*child1);
  child1->BindToParent (*node);
  
  printf ("entities tree:\n");
  
  dump (*node);
  
  printf ("traverse top-to-bottom\n");  
  node->Traverse (&print, NodeTraverseMode_TopToBottom);
  
  printf ("traverse bottom-to-top\n");
  node->Traverse (&print, NodeTraverseMode_BottomToTop);
  
  printf ("traverse top-to-bottom (const)\n");  
  const_cast<const Node&> (*node).Traverse (&cprint, NodeTraverseMode_TopToBottom);
  
  printf ("traverse bottom-to-top (const)\n");
  const_cast<const Node&> (*node).Traverse (&cprint, NodeTraverseMode_BottomToTop);

  return 0;
}
