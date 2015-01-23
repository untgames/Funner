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

    font_library.LoadFont ("data/times.xfont");

    context.Attach (font_library);

    FontDpi font_dpi;

    font_dpi.horizontal_dpi = 100;
    font_dpi.vertical_dpi   = 100;

    context.Attach (font_dpi);

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
