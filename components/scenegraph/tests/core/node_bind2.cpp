#include "shared.h"

typedef com_ptr<Node> NodePtr;

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
  
  NodePtr node (Node::Create (), false),  parent1 (Node::Create (), false), parent2 (Node::Create (), false),
          child (Node::Create (), false);
  
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
