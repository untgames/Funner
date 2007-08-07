#include "shared.h"

typedef com_ptr<Node> NodePtr;

void on_destroy (Node& node)
{
  printf ("node '%s' destroyed\n", node.Name ());
}

int main ()
{
  printf ("Results of node_basic_test:\n");
  
  NodePtr node (Node::Create (), false);
  
  node->SetName ("node1");
  node->Event   (NodeEvent_BeforeDestroy).connect (&on_destroy);  

  printf ("name: '%s'\n", node->Name ());
  printf ("use count: %u\n", node->UseCount ());

  return 0;
}
