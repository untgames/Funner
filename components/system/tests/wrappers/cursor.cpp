#include "shared.h"

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

int main ()
{
  printf ("Results of cursor_test:\n");
  
  try
  {
    common::FileSystem::AddSearchPath ("data");
    
    Window window1 (WindowStyle_Overlapped, 400, 300),
           window2 (WindowStyle_Overlapped, 400, 300);    
           
    window2.SetCursor ("aero_busy.ani");

//    window1.HideCursor ();
    window1.SetCursorVisible (true);
    window1.Show ();    
    window2.Show ();
    
    window1.RegisterEventHandler (WindowEvent_OnClose, &destroy);
    window2.RegisterEventHandler (WindowEvent_OnClose, &destroy);

    Application::Run ();

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
