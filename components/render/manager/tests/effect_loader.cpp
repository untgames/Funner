#include "shared.h"

const char* LIBRARY_NAME = "data/render.rfx";

int main ()
{
  printf ("Results of effect_loader_test:\n");
  
  try
  {
//    Test test (L"Effect loader test", false);
    Test test (L"Effect loader test", true);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource (LIBRARY_NAME);
    
    printf ("Effects loaded\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
