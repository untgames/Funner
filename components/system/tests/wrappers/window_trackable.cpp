#include "shared.h"

typedef xtl::shared_ptr<Window> WindowPtr;

WindowPtr window_holder;
xtl::trackable_ptr<Window> window_ptr;

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  window_holder.reset ();

  Application::Exit (0);
}

void on_application_initialized ()
{
  window_holder.reset (new Window (WindowStyle_Overlapped, 400, 300));

  Window& window = *window_holder;

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

  window.RegisterEventHandler (WindowEvent_OnClose, &destroy);

  window.Close ();

  printf ("window pointer: %s\n", window_ptr ? "non null" : "null");
}

int main ()
{
  printf ("Results of window_trackable_test:\n");

  try
  {
    Application::RegisterEventHandler (ApplicationEvent_OnInitialize, &on_application_initialized);

    Application::Run ();
    
    printf ("window pointer: %s\n", window_ptr ? "non null" : "null");

    return Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
