#include "shared.h"

const char* LIBRARY_NAME = "data/test.xmtl";

void on_application_initialized ()
{
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

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of material_load_test:\n");

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
