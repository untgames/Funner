#include "shared.h"

void on_update (Node& node, NodeEvent)
{
  printf ("node '%s' updated\n", node.Name ());
}

void on_bind (Node& node, NodeEvent)
{
  printf ("node '%s' binded to parent '%s'\n", node.Name (), node.Parent () ? node.Parent ()->Name () : "#ROOT");
}

void on_unbind (Node& node, NodeEvent)
{
  printf ("node '%s' unbinded from parent '%s'\n", node.Name (), node.Parent () ? node.Parent ()->Name () : "#ROOT");
}

void on_before_destroy (Node& node, NodeEvent)
{
  printf ("node '%s' before destroy\n", node.Name ());
}

void on_after_destroy (Node& node, NodeEvent)
{
  printf ("node '%s' destroyed\n", node.Name ());
}

void register_notifiers (Node& node)
{
  node.Event (NodeEvent_AfterUpdate).connect (&on_update);
  node.Event (NodeEvent_AfterBind).connect (&on_bind);
  node.Event (NodeEvent_BeforeUnbind).connect (&on_unbind);
  node.Event (NodeEvent_BeforeDestroy).connect (&on_before_destroy);
  node.Event (NodeEvent_AfterDestroy).connect (&on_after_destroy);
}

int main ()
{
  printf ("Results of node_bind1_test:\n");
  
  Node::Pointer node (Node::Create ());
  
  node->SetName   ("node1");
  register_notifiers (*node);
   
  Node::Pointer parent1 (Node::Create ());
  
  parent1->SetName ("parent1");
  register_notifiers (*parent1);
  
  node->BindToParent (*parent1);
  
  Node::Pointer parent2 (Node::Create ());  
  
  parent2->SetName ("parent2");
  register_notifiers (*parent2);
  
  node->BindToParent (*parent2);    

  return 0;
}
