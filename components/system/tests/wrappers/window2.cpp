#include "shared.h"

typedef xtl::shared_ptr<Window> WindowPtr;
typedef xtl::shared_ptr<Timer> TimerPtr;

TimerPtr timer_holder;
WindowPtr window_holder;

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  window_holder.reset ();

  Application::Exit (0);
}

void after_show (Timer& timer)
{
  timer.Pause ();

  Window& window = *window_holder;

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
}

void on_application_initialized ()
{
  window_holder.reset (new Window (WindowStyle_Overlapped, 400, 300));

  Window& window = *window_holder;

  window.SetTitle ("Test window");

  window.SetPosition (0, 0);

  printf ("title: '%s'\n", window.Title ());
  printf ("is_active: %d\n", window.IsActive ());
  printf ("position: x=%lu y=%lu\n", window.Position ().x, window.Position ().y);
  printf ("width: %lu\n", window.Width ());
  printf ("height: %lu\n", window.Height ());

  window.Show ();

  timer_holder.reset (new Timer (&after_show, 100));
}

int main ()
{
  printf ("Results of window2_test:\n");

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
