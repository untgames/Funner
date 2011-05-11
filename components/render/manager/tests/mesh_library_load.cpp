#include "shared.h"

const char* LIBRARY_NAME = "data/test.xmesh";

int main ()
{
  printf ("Results of mesh_library_load_test:\n");
  
  try
  {
    common::LogFilter log_filter ("render.manager*", &log_print);    
    
    Test test (L"Mesh library test", false);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource (LIBRARY_NAME);
    
    Primitive primitive = render_manager.CreateSharedPrimitive ("mesh2");
    
    printf ("Mesh library loaded (%u meshes in primitive 'mesh2')\n", primitive.MeshesCount ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
