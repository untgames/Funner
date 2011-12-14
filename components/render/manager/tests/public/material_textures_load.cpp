#include "shared.h"

int main ()
{
  printf ("Results of material_textures_load_test:\n");
  
  try
  {
    common::LogFilter log_filter ("render.manager*", &log_print);    
    
    Test test (L"Material test", false);
    
    RenderManager render_manager = test.RenderManager ();
    
    render_manager.LoadResource ("data/test.xmtl");
    render_manager.LoadResource ("data/render.rfx");    
    render_manager.LoadResource ("data/bottom.jpg");
    render_manager.LoadResource ("data/pic1.dds");
    
    Material material = render_manager.CreateSharedMaterial ("canyon.stn_2");    
    
    material.UpdateCache ();

    printf ("All resources have loaded\n");    
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  return 0;
}
