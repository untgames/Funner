#include "shared.h"

void on_window_destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

int main ()
{
  printf ("Results of application_exit_test:\n");
  
  try
  {
    Window window (WindowStyle_Overlapped, 400, 300);
    
    window.SetTitle ("Test window");
    window.Show ();

    window.RegisterEventHandler (WindowEvent_OnDestroy, &on_window_destroy);

    Application::Run ();        
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return Application::GetExitCode ();    
}
