#include "shared.h"

void on_application_initialized ()
{
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

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of material_textures_load_test:\n");

  try
  {
    syslib::Application::RegisterEventHandler (syslib::ApplicationEvent_OnInitialize, &on_application_initialized);

    syslib::Application::Run ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
