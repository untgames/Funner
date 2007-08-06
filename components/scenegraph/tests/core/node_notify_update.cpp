#include "shared.h"

typedef com_ptr<Node> NodePtr;

void on_update1 (Node& node)
{
  printf ("node '%s' updated\n", node.Name ());  
}

void on_update2 (Node& node)
{
  node.SetColor (1, 0, 0);
}

int main ()
{
  printf ("Results of node_notify_update_test:\n");
  
  NodePtr node (Node::Create (), false);
  
  node->SetName   ("node1");
  node->Listeners (NodeEvent_AfterUpdate).connect (&on_update1);
  
  printf ("Update color\n");  
  
  node->SetColor (0, 1, 0);
  
  printf ("Update name\n");
  
  node->SetName ("node2");
  
  node->Listeners (NodeEvent_AfterUpdate).connect (&on_update2);
  
  printf ("Begin update\n");
  
  NodeUpdateLock lock (*node);
  
  printf ("Update color\n");
  
  node->SetColor (0, 0, 1);
  
  printf ("Update name\n");
  
  node->SetName ("node3");
  
  printf ("End update\n");
  
  NodeUpdateLock ().Swap (lock);

  return 0;
}
