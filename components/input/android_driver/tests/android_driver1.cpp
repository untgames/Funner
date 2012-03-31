#include <cstdio>
#include <exception>

#include <input/low_level/driver.h>

using namespace input::low_level;

int main ()
{
  printf ("Results of android_driver_test:\n");

  try
  {
    for (size_t i = 0, count = DriverManager::DriversCount (); i < count; i++)
    {
      printf ("Driver %u name is '%s'\n", i, DriverManager::DriverName (i));
      
      IDriver* driver = DriverManager::Driver (i);
      
      for (size_t i=0, count=driver->GetDevicesCount (); i<count; i++)
      {                
        printf ("  Device %u name is '%s' full name is '%s'\n", i, driver->GetDeviceName (i), driver->GetDeviceFullName (i));
      }
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
