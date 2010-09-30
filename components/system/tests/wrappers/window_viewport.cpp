#include "shared.h"

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

void update_viewport (Window&, Rect& vp)
{
  vp.left   = 10;
  vp.right  = 20;
  vp.top    = 10;
  vp.bottom = 20;
}

void on_change_viewport (Window&, WindowEvent, const WindowEventContext&)
{
  printf ("viewport changed\n");
}

int main ()
{
  printf ("Results of window_viewport_test:\n");

  try
  {
    Window window (WindowStyle_Overlapped, 400, 300);

    window.SetClientSize (300, 400);

    auto_connection connection1 = window.RegisterEventHandler (WindowEvent_OnClose, &destroy);
    auto_connection connection2 = window.RegisterEventHandler (WindowEvent_OnChangeViewport, &on_change_viewport); 
    
    Rect viewport = window.Viewport ();
    
    printf ("viewport before: %u %u %u %u\n", viewport.left, viewport.top, viewport.right, viewport.bottom);

    window.SetViewportHandler (&update_viewport);        
    
    viewport = window.Viewport ();
    
    printf ("viewport after: %u %u %u %u\n", viewport.left, viewport.top, viewport.right, viewport.bottom);    

    window.Close ();

    Application::Run ();

    return Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
