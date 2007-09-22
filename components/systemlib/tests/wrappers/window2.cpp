#include "shared.h"

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

int main ()
{
  printf ("Results of window1_test:\n");
  
  try
  {
    Window window (WindowStyle_Overlapped, 400, 300);

    window.SetTitle ("Test window");
    
    window.SetPosition (0, 0);    

    auto_connection connection4 = window.RegisterEventHandler (WindowEvent_OnDestroy, &destroy);
    
    printf ("title: '%s'\n", window.Title ());
    printf ("is_active: %d\n", window.IsActive ());
    printf ("position: x=%d y=%d\n", window.Position ().x, window.Position ().y);
    printf ("width: %d\n", window.Width ());
    printf ("height: %d\n", window.Height ());
    
    window.Deactivate ();
    
    printf ("after deactivate\n");
    printf ("is_active: %d\n", window.IsActive ());

    window.Close ();

    Timer timer (xtl::bind (&Window::Close, &window), 1000);

    Application::Run ();        

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
