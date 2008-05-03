#include <stdio.h>

#include <xtl/signal.h>
#include <xtl/intrusive_ptr.h>

#include <input/low_level/device.h>
#include <input/low_level/driver.h>
#include <input/low_level/window_driver.h>

#include <syslib/window.h>
#include <syslib/application.h>

using namespace syslib;
using namespace input::low_level;

void on_window_destroy (Window&, WindowEvent, const WindowEventContext&)
{
  Application::Exit (0);
}

void input_event_handler (const char* event)
{
  printf ("New event: '%s'\n", event);
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

    device->SetEventHandler (&input_event_handler);

    printf ("Device has properties: '%s'\n", device->GetProperties ());

    device->SetProperty ("AUTOCENTER_CURSOR", 1.f);
    device->SetProperty ("HORISONTAL_WHEEL_SENSITIVITY", -2.4f);
    device->SetProperty ("CURSOR_SENSITIVITY", -1.1f);

    printf ("Property 'AUTOCENTER_CURSOR'  = %f\n", device->GetProperty ("AUTOCENTER_CURSOR"));
    printf ("Property 'VERTICAL_WHEEL_SENSITIVITY'  = %f\n", device->GetProperty ("VERTICAL_WHEEL_SENSITIVITY"));
    printf ("Property 'HORISONTAL_WHEEL_SENSITIVITY'  = %f\n", device->GetProperty ("HORISONTAL_WHEEL_SENSITIVITY"));
    printf ("Property 'CURSOR_SENSITIVITY' = %f\n", device->GetProperty ("CURSOR_SENSITIVITY"));

    window.Show ();

    window.RegisterEventHandler (WindowEvent_OnDestroy, &on_window_destroy);

    Application::Run ();        

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
