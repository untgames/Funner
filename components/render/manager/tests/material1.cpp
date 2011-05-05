#include "shared.h"

const char* LIBRARY_NAME = "data/test.xmtl";

int main ()
{
  printf ("Results of material1_test:\n");
  
  try
  {
    Test test (L"Material test", false);
    
    RenderManager render_manager = test.RenderManager ();
    
    media::rfx::MaterialLibrary library (LIBRARY_NAME);
    
    render_manager.LoadResource (library);
    
    printf ("Material library loaded\n");        
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
