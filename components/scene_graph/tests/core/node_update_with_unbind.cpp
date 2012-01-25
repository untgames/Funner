#include "shared.h"

template <int id> void my_updater (const TimeValue&)
{
  printf ("my_updater%d\n", id);
}

void unbinder (Node* node)
{
  printf ("unbind\n");
  
  node->Unbind ();
}

void binder (Node* parent)
{
  Node::Pointer child = Node::Create ();
  
  child->AttachController (&my_updater<5>);
  
  child->BindToParent (*parent, NodeBindMode_AddRef);
}

int main ()
{
  printf ("Results of node_update_with_unbind_test:\n");
  
  Node::Pointer parent = Node::Create ();
  Node::Pointer child1 = Node::Create ();
  Node::Pointer child2 = Node::Create ();
  Node::Pointer child3 = Node::Create ();  
  Node::Pointer child4 = Node::Create ();
  
  child1->AttachController (&my_updater<1>);
  child2->AttachController (&my_updater<2>);  
  child2->AttachController (xtl::bind (unbinder, &*child2));
  child2->AttachController (xtl::bind (unbinder, &*child3));  
  child3->AttachController (&my_updater<3>);  
  child4->AttachController (&my_updater<4>);
  child4->AttachController (xtl::bind (binder, &*parent));  

  child1->BindToParent (*parent);
  child2->BindToParent (*parent);  
  child3->BindToParent (*parent);  
  child4->BindToParent (*parent);  

  printf ("update with unbind child2\n");
  
  parent->Update (TimeValue (1, 2));
  
  printf ("second update\n");  
  
  parent->Update (TimeValue (1, 2));

  return 0;
}
