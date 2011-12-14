#include "shared.h"

int main ()
{
  printf ("Results of mesh_library_load_test:\n");
  
  try
  {
    common::LogFilter log_filter ("render.manager*", &log_print);    
    
    Test test (L"Mesh library test", false);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource ("data/test.xmesh");
    render_manager.LoadResource ("data/test.xmtl");
    render_manager.LoadResource ("data/render.rfx");    
    render_manager.LoadResource ("data/bottom.jpg");
    render_manager.LoadResource ("data/pic1.dds");    
    
    Primitive primitive = render_manager.CreateSharedPrimitive ("mesh2");
    
    primitive.UpdateCache ();
    
    printf ("Mesh library loaded (%u meshes in primitive 'mesh2')\n", primitive.MeshesCount ());
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
