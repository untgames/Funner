#include "shared.h"

const char* LIBRARY_NAME = "data/test.xmtl";

int main ()
{
  printf ("Results of material_load_test:\n");
  
  try
  {
    common::LogFilter log_filter ("render.manager*", &log_print);    
    
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
