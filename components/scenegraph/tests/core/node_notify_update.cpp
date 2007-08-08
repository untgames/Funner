#include "shared.h"

typedef com_ptr<Node> NodePtr;

void on_update1 (Node& node, NodeEvent)
{
  printf ("node '%s' updated\n", node.Name ());  
}

void on_update2 (Node& node, NodeEvent)
{
  node.SetName (node.Name ());
}

int main ()
{
  printf ("Results of node_notify_update_test:\n");
  
  NodePtr node (Node::Create (), false);
  
  node->SetName   ("node1");
  node->Event (NodeEvent_AfterUpdate).connect (&on_update1);
  
  printf ("Update color\n");  
  
  node->SetName (node->Name ());
  
  printf ("Update name\n");
  
  node->SetName ("node2");
  
  node->Event (NodeEvent_AfterUpdate).connect (&on_update2);
  
  printf ("Begin update\n");
  
  NodeUpdateLock lock (*node);
  
  printf ("Update color\n");
  
  node->SetName (node->Name ());
  
  printf ("Update name\n");
  
  node->SetName ("node3");
  
  printf ("End update\n");
  
  NodeUpdateLock ().Swap (lock);

  return 0;
}
