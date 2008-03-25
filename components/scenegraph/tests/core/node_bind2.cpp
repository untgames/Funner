#include "shared.h"

void on_bind_child (Node& node, Node& child, NodeSubTreeEvent)
{
  printf ("node '%s' binded to '%s' sub-tree\n", child.Name (), node.Name ());
  
    //проверка корректности обработки сутации вызова Bind в обработчике (вызов должен быть проигнорирован)
  
  child.Unbind ();
}

void on_unbind_child (Node& node, Node& child, NodeSubTreeEvent)
{
  printf ("node '%s' unbinded from '%s' sub-tree\n", child.Name (), node.Name ());
  
    //проверка корректности обработки сутации вызова Bind в обработчике (вызов должен быть проигнорирован)
  
//  child.BindToParent (node);
}

int main ()
{
  printf ("Results of node_bind2_test:\n");
  
  Node::Pointer node (Node::Create ()),  parent1 (Node::Create ()), parent2 (Node::Create ()),
                child (Node::Create ());
  
  parent1->SetName ("parent1");
  parent2->SetName ("parent2");  
  node->SetName    ("child1");
  child->SetName   ("child2");
  
  parent1->Event (NodeSubTreeEvent_AfterBind).connect (&on_bind_child);
  parent1->Event (NodeSubTreeEvent_BeforeUnbind).connect (&on_unbind_child);
  
  parent2->BindToParent (*parent1);
  child->BindToParent (*node);
  node->BindToParent (*parent2);
  
  printf ("exit\n");

  return 0;
}
