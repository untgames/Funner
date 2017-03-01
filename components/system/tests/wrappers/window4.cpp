#include "shared.h"

typedef xtl::shared_ptr<Window> WindowPtr;

WindowPtr window_holder;

void destroy (Window&, WindowEvent, const WindowEventContext&)
{
  window_holder.reset ();

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

void on_application_initialized ()
{
  window_holder.reset (new Window);

  Window& window = *window_holder;

  printf ("setting style\n");
  window.SetStyle (WindowStyle_Overlapped);
  printf ("setting width\n");
  window.SetWidth (400);
  printf ("setting height\n");
  window.SetHeight (300);
  printf ("setting position\n");
  window.SetPosition (10, 10);
  printf ("setted\n");

  print_window_info (window);

  window.RegisterEventHandler (WindowEvent_OnClose, &destroy);

  window.Close ();
}

int main ()
{
  printf ("Results of window4_test:\n");

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
