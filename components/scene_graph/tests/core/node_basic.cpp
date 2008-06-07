#include "shared.h"

void on_destroy (Node& node, NodeEvent)
{
  printf ("node '%s' destroyed\n", node.Name ());
}

int main ()
{
  printf ("Results of node_basic_test:\n");
  
  Node::Pointer node (Node::Create ());
  
  node->SetName ("node1");
  node->RegisterEventHandler (NodeEvent_BeforeDestroy, &on_destroy);  

  printf ("name: '%s'\n", node->Name ());
  printf ("use count: %u\n", node->UseCount ());

  return 0;
}
