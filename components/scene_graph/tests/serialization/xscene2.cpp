#include "shared.h"

void print_log (const char* message)
{
  printf ("%s\n", message);
}

void traverser (Node& node)
{
  dump_node (node);
}

int main ()
{
  printf ("Results of xscene2_test:\n");

  try
  {
    SceneManager manager;
    
    manager.LoadScene ("data/test2.xscene", "", &print_log);
    
    SceneContext context;
    
    context.SetLogHandler (&print_log);
    context.Properties ().SetProperty ("BeforeNodeOffset", math::vec3f (0.0f, 0.0f, 1.0f));
    
    Node::Pointer node = manager.CreateScene ("scene1", context);
    
    node->Traverse (&traverser);
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
