#include "shared.h"

struct MyUpdateHandler
{
  MyUpdateHandler (Node* in_node) : node (in_node) {}

  void operator () (const TimeValue&)
  {
    node->SetPosition (1.0f, 2.0f, 3.0f);
    node->SetPosition (1.0f, 2.0f, 3.0f);
    node->SetPosition (1.0f, 2.0f, 3.0f);    
  }

  Node* node;
};

void on_update_event (Node& node, NodeEvent)
{
  printf ("node updated\n");
}

int main ()
{
  printf ("Results of node_update_events_test:\n");
  
  Node::Pointer node = Node::Create ();      

  node->AttachController (MyUpdateHandler (&*node));
  node->RegisterEventHandler (NodeEvent_AfterUpdate, &on_update_event);
  
  node->Update (TimeValue (1, 2));

  return 0;
}
