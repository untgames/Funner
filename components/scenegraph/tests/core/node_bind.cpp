#include "shared.h"

typedef com_ptr<Node> NodePtr;

void on_update (Node& node)
{
  printf ("node '%s' updated\n", node.Name ());
}

void on_bind (Node& node)
{
  printf ("node '%s' binded to parent '%s'\n", node.Name (), node.Parent () ? node.Parent ()->Name () : "#ROOT");
}

void on_unbind (Node& node)
{
  printf ("node '%s' unbinded from parent '%s'\n", node.Name (), node.Parent () ? node.Parent ()->Name () : "#ROOT");
}

void on_before_destroy (Node& node)
{
  printf ("node '%s' before destroy\n", node.Name ());
}

void on_after_destroy (Node& node)
{
  printf ("node '%s' destroyed\n", node.Name ());
}

void register_notifiers (Node& node)
{
  node.Listeners (NodeEvent_AfterUpdate).connect (&on_update);
  node.Listeners (NodeEvent_AfterBind).connect (&on_bind);
  node.Listeners (NodeEvent_BeforeUnbind).connect (&on_unbind);
  node.Listeners (NodeEvent_BeforeDestroy).connect (&on_before_destroy);
  node.Listeners (NodeEvent_AfterDestroy).connect (&on_after_destroy);
}

int main ()
{
  printf ("Results of node_bind_test:\n");
  
  NodePtr node (Node::Create (), false);
  
  node->SetName   ("node1");
  register_notifiers (*node);
   
  NodePtr parent1 (Node::Create (), false);
  
  parent1->SetName ("parent1");
  register_notifiers (*parent1);
  
  node->BindToParent (*parent1);
  
  NodePtr parent2 (Node::Create (), false);  
  
  parent2->SetName ("parent2");
  register_notifiers (*parent2);
  
  node->BindToParent (*parent2);    

  return 0;
}
