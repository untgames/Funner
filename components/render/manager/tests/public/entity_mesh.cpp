#include "shared.h"

int main ()
{
  printf ("Results of entity_mesh_test:\n");
  
  try
  {
    common::LogFilter log_filter ("render.manager*", &log_print);    
    
    Test test (L"Entity test", false);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource ("data/test.xmesh");
    render_manager.LoadResource ("data/test.xmtl");
    render_manager.LoadResource ("data/render.rfx");    
    render_manager.LoadResource ("data/bottom.jpg");
    render_manager.LoadResource ("data/pic1.dds");    
    
    Entity entity = render_manager.CreateEntity ();
    
    entity.SetPrimitive ("mesh2");
    
    entity.UpdateCache ();
    
    printf ("Entity created successfull\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
