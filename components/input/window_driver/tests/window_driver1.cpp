#include <stdio.h>

#include <xtl/intrusive_ptr.h>
#include <xtl/string.h>
#include <xtl/signal.h>

#include <input/low_level/device.h>
#include <input/low_level/driver.h>
#include <input/low_level/window_driver.h>

#include <syslib/window.h>
#include <syslib/application.h>

using namespace syslib;
using namespace input::low_level;

void input_event_handler (const char* event)
{
  printf ("New event: '%s'\n", event);

  if (!xtl::xstrcmp ("Window closed", event))
    Application::Exit (0);
}

void viewport_handler (Window& window, Rect& viewport)
{
  viewport.left   = window.Width () / 4;
  viewport.top    = window.Height () / 4;
  viewport.right  = window.Width () / 4 * 3;
  viewport.bottom = window.Height () / 4 * 3;
}

int main ()
{
  printf ("Results of window_driver1_test:\n");

  try
  {
    Window window (WindowStyle_Overlapped, 400, 300);

    window.SetTitle ("Test window");

    WindowDriver::RegisterDevice (window);

    xtl::com_ptr<IDevice> device (DriverManager::CreateDevice ("*", "*"), false);

    device->RegisterEventHandler (&input_event_handler);

    printf ("Device name is '%s', full name is '%s'\n", device->GetName (), device->GetFullName ());
    printf ("Device has properties: '%s'\n", device->GetProperties ());

//    device->SetProperty ("Cursor.auto_center", 1.f);
    device->SetProperty ("WheelX.sensitivity", -2.4f);
    device->SetProperty ("Cursor.sensitivity", -1.1f);

    printf ("Property 'Cursor.auto_center' = %f\n", device->GetProperty ("Cursor.auto_center"));
    printf ("Property 'WheelY.sensitivity' = %f\n", device->GetProperty ("WheelY.sensitivity"));
    printf ("Property 'WheelX.sensitivity' = %f\n", device->GetProperty ("WheelX.sensitivity"));
    printf ("Property 'Cursor.sensitivity' = %f\n", device->GetProperty ("Cursor.sensitivity"));

    window.Show ();

    window.SetViewportHandler (viewport_handler);
    window.InvalidateViewport ();

    Application::Run ();

    return Application::GetExitCode ();
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
