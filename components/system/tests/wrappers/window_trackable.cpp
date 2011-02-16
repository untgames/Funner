#include "shared.h"

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

int main ()
{
  printf ("Results of window_trackable_test:\n");

  xtl::trackable_ptr<Window> window_ptr;

  try
  {
    {
      Window window (WindowStyle_Overlapped, 400, 300);
      
      window_ptr = &window;

      window.SetTitle ("Test window");

      window.SetPosition (0, 0);

      printf ("title: '%s'\n", window.Title ());
      printf ("is_active: %d\n", window.IsActive ());
      printf ("position: x=%lu y=%lu\n", window.Position ().x, window.Position ().y);
      printf ("width: %lu\n", window.Width ());
      printf ("height: %lu\n", window.Height ());

      window.Show ();

      printf ("after show\n");

      window.SetClientSize (300, 400);

      Rect client_rect = window.ClientRect ();

      printf ("Client width=%u, height=%u\n", client_rect.right - client_rect.left, client_rect.bottom - client_rect.top);

      printf ("is_active: %d\n", window.IsActive ());

      window.Deactivate ();

      printf ("after deactivate\n");
      printf ("is_active: %d\n", window.IsActive ());

      printf ("is multitouch enabled: %d\n", window.IsMultitouchEnabled ());

      window.SetMultitouchEnabled (true);

      printf ("is multitouch enabled: %d\n", window.IsMultitouchEnabled ());

      auto_connection connection = window.RegisterEventHandler (WindowEvent_OnClose, &destroy);

      window.Close ();
      
      printf ("window pointer: %s\n", window_ptr ? "non null" : "null");      

      Application::Run ();
    }
    
    printf ("window pointer: %s\n", window_ptr ? "non null" : "null");

    return Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
