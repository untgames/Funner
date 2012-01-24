#include "shared.h"

template <int id> void my_updater (float)
{
  printf ("my_updater%d\n", id);
}

void unbinder (Node* node)
{
  printf ("unbind\n");
  
  node->Unbind ();
}

void traverser (Node& node)
{
  printf ("node '%s'\n", node.Name ());

  if (stl::string (node.Name ()) == "child2")
  {
    if (node.NextChild ())
      node.NextChild ()->Unbind ();

    node.Unbind ();
  }
  
  if (stl::string (node.Name ()) == "child4")
  {
    Node::Pointer child = Node::Create ();
    
    child->SetName ("child5");
  
    child->BindToParent (node, NodeBindMode_AddRef);
  }
}

int main ()
{
  printf ("Results of node_traverse_with_unbind_test:\n");
  
  Node::Pointer parent = Node::Create ();
  Node::Pointer child1 = Node::Create ();
  Node::Pointer child2 = Node::Create ();
  Node::Pointer child3 = Node::Create ();  
  Node::Pointer child4 = Node::Create ();
  
  parent->SetName ("parent");
  child1->SetName ("child1");
  child2->SetName ("child2");
  child3->SetName ("child3");
  child4->SetName ("child4");
  
  child1->BindToParent (*parent);
  child2->BindToParent (*parent);  
  child3->BindToParent (*parent);  
  child4->BindToParent (*parent);  
    
  printf ("first traverse\n");
  parent->Traverse (&traverser, NodeTraverseMode_TopToBottom);  
  
  printf ("second traverse\n");  
  parent->Traverse (&traverser, NodeTraverseMode_TopToBottom);    

  return 0;
}
