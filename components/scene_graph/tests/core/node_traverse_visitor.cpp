#include "shared.h"

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
  
  for (Node::ConstPointer child=node.FirstChild (); child; child=child->NextChild ())
    dump (*child, level+1);    
}

int main ()
{
  printf ("Results of node_traverse_visitor_test:\n");
  
  Node::Pointer node (Node::Create ()), child1 (SpotLight::Create ()), child2 (PerspectiveCamera::Create ());
  
  node->SetName ("node");
  child1->SetName ("child1");
  child2->SetName ("child2");    

  child2->BindToParent (*child1);
  child1->BindToParent (*node);
  
  printf ("entities tree:\n");
  
  dump (*node);  
  
  printf ("visitor traverse top-to-bottom\n");
  
  MyVisitor visitor;
  
  node->VisitEach (visitor, NodeTraverseMode_TopToBottom);
  
  printf ("visitor traverse bottom-to-top\n");
  node->VisitEach (visitor, NodeTraverseMode_BottomToTop);

  return 0;
}
