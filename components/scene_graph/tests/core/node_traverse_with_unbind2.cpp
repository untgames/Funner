#include "shared.h"

class MyController: public Controller
{
  public:
    typedef xtl::com_ptr<MyController> Pointer;

    MyController (Node& in_node) : Controller (in_node)
    {
      SetUpdatable (true);
    }
};

void traverser (Node& node)
{
  printf ("node '%s'\n", node.Name ());
  
  if (stl::string (node.Name ()) == "child1")
  {
    Node::Pointer child = Node::Create ();
    
    new MyController (*child);

    child->SetName ("child2");
  
    child->BindToParent (node, NodeBindMode_AddRef);
  }
}

//this test for adding child with controller during traverse
int main ()
{
  printf ("Results of node_traverse_with_unbind2_test:\n");
  
  Node::Pointer parent = Node::Create ();
  Node::Pointer child1 = Node::Create ();
  
  parent->SetName ("parent");
  child1->SetName ("child1");
  
  child1->BindToParent (*parent);
    
  printf ("first traverse\n");
  parent->Traverse (&traverser, NodeTraverseMode_TopToBottom);  
  
  printf ("second traverse\n");  
  parent->Traverse (&traverser, NodeTraverseMode_TopToBottom);    

  return 0;
}
