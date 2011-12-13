#include <clocale>
#include <cstdio>

#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/signal.h>
#include <xtl/string.h>

#include <common/log.h>
#include <common/strlib.h>

#include <input/low_level/device.h>
#include <input/low_level/driver.h>

#include <syslib/application.h>

using namespace common;
using namespace syslib;
using namespace input::low_level;

void input_event_handler (IDevice* device, const char* event)
{
  if (!xtl::xstrcmp ("'Button1' down", event))
    Application::Exit (0);

  printf ("New event: '%s'\n", event);

  printf ("Control name is '%S'\n", device->GetControlName (word (event, 0, " ", " \t", "''").c_str ()));
}

void debug_log_handler (const char* message)
{
  printf ("Debug log message: %s\n", message);
}

void print_log (const char* log_name, const char* message)
{
  printf ("%s: %s\n", log_name, message);
}

typedef xtl::com_ptr<IDevice> DevicePtr;

int main ()
{
  setlocale (LC_ALL, "Russian");

  printf ("Results of direct_input_driver1_test:\n");

  LogFilter filter ("*", &print_log);
  
  try
  {
    IDriver* direct_input_driver = DriverManager::FindDriver ("DirectInput8");

    if (!direct_input_driver)
      throw xtl::format_operation_exception ("", "Can't find direct input 8 driver");

    direct_input_driver->SetDebugLog (&debug_log_handler);

    printf ("Available devices:\n");

    stl::vector<DevicePtr> devices;

    for (size_t i = 0; i < direct_input_driver->GetDevicesCount (); i++)
    {
      printf ("  %u: name is '%s'", i + 1, direct_input_driver->GetDeviceName (i));

      devices.push_back (DevicePtr (DriverManager::CreateDevice ("*", direct_input_driver->GetDeviceFullName (i), "buffer_size=0"), false));

      devices.back ()->RegisterEventHandler (xtl::bind (&input_event_handler, devices.back ().get (), _1));

      printf (" full name is '%s'\n", devices.back ()->GetFullName ());
      printf ("Device has properties: '%s'\n", devices.back ()->GetProperties ());

      for (size_t j = 0; !word (devices.back ()->GetProperties (), j, " ", " \t", "''\"\"").empty (); j++)
        printf ("  '%s' = %f\n", word (devices.back ()->GetProperties (), j, " ", " \t", "''\"\"").c_str (), devices.back ()->GetProperty (word (devices.back ()->GetProperties (), j, " ", " \t", "''\"\"").c_str ()));

      if (common::wcmatch (direct_input_driver->GetDeviceName (i), "*USB*xis*"))
      {
        devices.back ()->SetProperty ("Axis1.dead_zone", 0.4f);
        devices.back ()->SetProperty ("Axis1.saturation", 0.6f);
      }
    }
    
    Application::Run ();        

    return Application::GetExitCode ();    
  }  
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
