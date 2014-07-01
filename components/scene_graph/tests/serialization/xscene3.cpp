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
  printf ("Results of xscene3_test:\n");

  try
  {
    SceneManager manager;
    
    manager.LoadScene ("data/test3.xscene", "", &print_log);
    
    SceneContext context;

    media::FontLibrary font_library;

    context.Attach (font_library);
    
    context.SetLogHandler (&print_log);
    
    Node::Pointer node = manager.CreateScene ("scene3", context);
    
    node->Traverse (&traverser);
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
