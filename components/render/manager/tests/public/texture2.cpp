#include "shared.h"

const char* IMAGE_NAME = "data/bottom.jpg";

void on_application_initialized ()
{
  try
  {
    Test test (L"Texture2 test", false);

    RenderManager render_manager = test.RenderManager ();

    render_manager.LoadResource (IMAGE_NAME);

    Texture texture = render_manager.CreateSharedTexture (IMAGE_NAME);

    printf ("Texture:\n");
    printf ("  width:  %u\n", texture.Width ());
    printf ("  height: %u\n", texture.Height ());
    printf ("  depth:  %u\n", texture.Depth ());
    printf ("  format: %s\n", get_name (texture.Format ()));
  }
  catch (std::exception& e)
  {
    printf ("%s\n", e.what ());
  }

  syslib::Application::Exit (0);
}

int main ()
{
  printf ("Results of texture2_test:\n");

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
