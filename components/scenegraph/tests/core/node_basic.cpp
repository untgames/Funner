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
  
  node->SetName   ("node1");
  node->SetColor  (1, 0, 0);
  node->Listeners (NodeEvent_BeforeDestroy).connect (&on_destroy);
  
  const vec3f& color = node->Color ();

  printf ("name: '%s'\n", node->Name ());
  printf ("color: [%g %g %g]\n", color.x, color.y, color.z);
  printf ("use count: %u\n", node->UseCount ());

  return 0;
}
