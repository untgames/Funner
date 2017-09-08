#include "shared.h"

#ifdef  LINUX
const char* LIBRARY_NAME = "data/render_gles2.rfx";
#else
const char* LIBRARY_NAME = "data/render.rfx";
#endif

void on_application_initialized ()
{
  try
  {
    common::LogFilter log_filter ("render.manager*", &log_print);

    Test test (L"Effect loader test", false);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource (LIBRARY_NAME);

    printf ("Effects loaded\n");
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of effect_loader_test:\n");
  
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
