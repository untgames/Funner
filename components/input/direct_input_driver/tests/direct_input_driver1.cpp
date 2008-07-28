#include <clocale>
#include <cstdio>

#include <stl/vector>

#include <xtl/signal.h>
#include <xtl/intrusive_ptr.h>

#include <common/strlib.h>

#include <input/low_level/device.h>
#include <input/low_level/driver.h>
#include <input/low_level/direct_input_driver.h>

#include <syslib/window.h>
#include <syslib/application.h>

using namespace common;
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

typedef xtl::com_ptr<IDevice> DevicePtr;

int main ()
{
  setlocale (LC_ALL, "Russian");

  printf ("Results of direct_input_driver1_test:\n");
  
  try
  {
    Window window (WindowStyle_Overlapped, 400, 300);
    
    window.SetTitle ("Test window");

    DirectInputDriver::RegisterDevice (window);

    printf ("Available devices:\n");

    stl::vector<DevicePtr> devices;

    for (size_t i = 0; i < DirectInputDriver::Driver ()->GetDevicesCount (); i++)
    {
      printf ("  %u: '%s'\n", i + 1, DirectInputDriver::Driver ()->GetDeviceName (i));

      if (common::wcmatch (DirectInputDriver::Driver ()->GetDeviceName (i), "*Wireless*"))
        continue;

      devices.push_back (DevicePtr (DriverManager::CreateDevice ("*", DirectInputDriver::Driver ()->GetDeviceName (i)), false));

      devices.back ()->SetEventHandler (&input_event_handler);

      printf ("Device has properties: '%s'\n", devices.back ()->GetProperties ());

      for (size_t j = 0; !word (devices.back ()->GetProperties (), j, " ", " \t", "''\"\"").empty (); j++)
        printf ("  '%s' = %f\n", word (devices.back ()->GetProperties (), j, " ", " \t", "''\"\"").c_str (), devices.back ()->GetProperty (word (devices.back ()->GetProperties (), j, " ", " \t", "''\"\"").c_str ()));
    }
    
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
