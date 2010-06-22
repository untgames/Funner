#include "shared.h"

void on_window_destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

void on_application_initialized ()
{
  try
  {
    Window window (WindowStyle_Overlapped);
  
    window.SetTitle ("Test window");
    window.Show ();

    window.RegisterEventHandler (WindowEvent_OnDestroy, &on_window_destroy);
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
    Application::Exit (1);
  }
}

int main ()
{
  printf ("Results of application_exit_test:\n");
  
  try
  {
    Application::RegisterEventHandler (ApplicationEvent_OnInitialized, &on_application_initialized);

    Application::Run ();        
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
