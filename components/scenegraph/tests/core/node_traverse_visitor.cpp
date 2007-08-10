#include "shared.h"

typedef com_ptr<Node> NodePtr;

class MyVisitor: public visitor<void, Node, Light, Camera>
{
  public:
    void visit (Node& object) { printf ("node '%s'\n", object.Name ()); }
    void visit (Light& object)  { printf ("light '%s'\n", object.Name ()); }
    void visit (Camera& object) { printf ("camera '%s'\n", object.Name ()); }
};

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
  printf ("Results of node_traverse_visitor_test:\n");
  
  NodePtr node (Node::Create (), false), child1 (Light::Create (), false), child2 (PerspectiveCamera::Create (), false);
  
  node->SetName ("node");
  child1->SetName ("child1");
  child2->SetName ("child2");    

  child2->BindToParent (*child1);
  child1->BindToParent (*node);
  
  printf ("entities tree:\n");
  
  dump (*node);  
  
  printf ("visitor traverse top-to-bottom\n");
  node->VisitEach (MyVisitor (), NodeTraverseMode_TopToBottom);
  
  printf ("visitor traverse bottom-to-top\n");
  node->VisitEach (MyVisitor (), NodeTraverseMode_BottomToTop);

  return 0;
}
