#include "shared.h"

void xtl_exception_updater (const TimeValue&)
{
  throw xtl::format_operation_exception ("my_updater", "test exception");
}

void exception_updater (const TimeValue&)
{
  throw "test exception";
}

void log_handler (const char* log, const char* message)
{
  printf ("'%s': '%s'\n", log, message);
}

int main ()
{
  printf ("Results of node_update_exception_test:\n");
  
  common::LogFilter log ("*", &log_handler);

  Node::Pointer node = Node::Create (), node2 = Node::Create ();
  
  node->SetName ("node");
  node2->SetName ("node2");

  node->AttachController  (&xtl_exception_updater);
  node2->AttachController (&exception_updater);

  node->Update (1);
  node2->Update (1);

  return 0;
}
