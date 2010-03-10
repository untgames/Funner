#include "shared.h"

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
  
  Node::Pointer node (Node::Create ());
  
  node->SetName   ("node1");
  xtl::auto_connection update1_connection = node->RegisterEventHandler (NodeEvent_AfterUpdate, &on_update1);
  
  printf ("Update color\n");  
  
  node->SetName (node->Name ());
  
  printf ("Update name\n");
  
  node->SetName ("node2");
  
  xtl::auto_connection update2_connection = node->RegisterEventHandler (NodeEvent_AfterUpdate, &on_update2);
  
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
