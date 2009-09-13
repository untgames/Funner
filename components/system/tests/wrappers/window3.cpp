#include "shared.h"

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

const char* get_name (syslib::WindowStyle style)
{
  switch (style)
  {
    case WindowStyle_PopUp:      return "WindowStyle_PopUp";
    case WindowStyle_Overlapped: return "WindowStyle_Overlapped";
    default:                     return "Unknown";
  }
}

void print_window_info (syslib::Window& window)
{
  printf ("is_active: %d\n", window.IsActive ());
  printf ("is_visible: %d\n", window.IsVisible ());
  printf ("position: x=%lu y=%lu\n", window.Position ().x, window.Position ().y);
  printf ("width: %lu\n", window.Width ());
  printf ("height: %lu\n", window.Height ());
  printf ("window style: %s\n", get_name (window.Style ()));
}

int main ()
{
  printf ("Results of window3_test:\n");

  try
  {
    Window window (WindowStyle_Overlapped, 400, 300);

    window.SetPosition (10, 10);
    window.SetTitle ("Test window");

    window.Show ();

    print_window_info (window);

    window.SetStyle (WindowStyle_PopUp);

    print_window_info (window);
    
    auto_connection connection = window.RegisterEventHandler (WindowEvent_OnClose, &destroy);
    
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
