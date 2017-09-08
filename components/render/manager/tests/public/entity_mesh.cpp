#include "shared.h"

#ifdef LINUX
const char* LIBRARY_NAME="data/render_gles2.rfx";
#else
const char* LIBRARY_NAME="data/render.rfx";
#endif

void on_application_initialized ()
{
  try
  {
    common::LogFilter log_filter ("render.manager*", &log_print);    
    
    Test test (L"Entity test", false);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource ("data/test.xmesh");
    render_manager.LoadResource ("data/test.xmtl");
    render_manager.LoadResource (LIBRARY_NAME);
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

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of entity_mesh_test:\n");

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
