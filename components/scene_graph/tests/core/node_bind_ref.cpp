#include "shared.h"

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
  node.RegisterEventHandler (NodeEvent_AfterBind, &on_bind);
  node.RegisterEventHandler (NodeEvent_BeforeUnbind, &on_unbind);
  node.RegisterEventHandler (NodeEvent_BeforeDestroy, &on_before_destroy);
  node.RegisterEventHandler (NodeEvent_AfterDestroy, &on_after_destroy);
}

void test (NodeBindMode mode)
{
  Node::Pointer parent1 (Node::Create ());
  
  parent1->SetName ("parent1");
  register_notifiers (*parent1);  
  
  Node::Pointer node (Node::Create ());
  
  node->SetName   ("node1");
  register_notifiers (*node);   
  
  node->BindToParent (*parent1, mode);
}

int main ()
{
  printf ("Results of node_bind_ref_test:\n");
  
  printf ("test weak-ref bind mode\n");    
  test   (NodeBindMode_WeakRef);
  
  printf ("test add-ref bind mode\n");    
  test   (NodeBindMode_AddRef);  

  return 0;
}
