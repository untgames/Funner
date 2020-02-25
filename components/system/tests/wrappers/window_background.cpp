#include "shared.h"

typedef xtl::shared_ptr<Window> WindowPtr;

WindowPtr window_holder;

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

void on_application_initialized ()
{
  window_holder.reset (new Window (WindowStyle_Overlapped, 400, 300));

  Window& window = *window_holder;

  window.SetBackgroundState (true);
  window.SetBackgroundColor (Color (255, 0, 0));

  window.RegisterEventHandler (WindowEvent_OnClose, &destroy);

  window.Maximize ();
}

int main ()
{
  printf ("Results of window_background_test:\n");
  
  try
  {
    Application::RegisterEventHandler (ApplicationEvent_OnInitialize, &on_application_initialized);

    Application::Run ();

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
