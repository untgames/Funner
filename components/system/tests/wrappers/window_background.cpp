#include "shared.h"

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

int main ()
{
  printf ("Results of window_background_test:\n");
  
  try
  {
    common::FileSystem::AddSearchPath ("data");
    
    Window window (WindowStyle_Overlapped, 400, 300);
    
    window.SetBackgroundState (true);
    window.SetBackgroundColor (Color (255, 0, 0));
    
    window.RegisterEventHandler (WindowEvent_OnClose, &destroy);

    window.Maximize ();

    Application::Run ();

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
