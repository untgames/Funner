#include <cstdio>
#include <exception>

#include <input/low_level/driver.h>

using namespace input::low_level;

int main ()
{
  printf ("Results of iphone_driver_test:\n");

  try
  {
    for (size_t i = 0, count = DriverManager::DriversCount (); i < count; i++)
      printf ("Driver %u name is '%s'\n", DriverManager::DriverName (i));
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
