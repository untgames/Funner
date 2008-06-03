#include <sound/device.h>
#include <common/exception.h>
#include <xtl/function.h>
#include <stdio.h>

using namespace sound::low_level;
using namespace common;

ISoundDevice* my_create_device (const char* driver_name, const char* device_name, const void* window_handle, const char* init_string)
{
  raise<Exception> ("my_create_device", "Attempt to create sound device: driver='%s' device='%s' window_handle=%p init_string='%s'",
                    driver_name, device_name, window_handle, init_string);

  return 0;
}

int main ()
{
  printf ("Results of device1_test:\n");
  
  try
  {
    SoundSystem::RegisterDriver ("driver1", &my_create_device);
    SoundSystem::RegisterDriver ("driver2", &my_create_device);
    SoundSystem::RegisterConfiguration  ("driver1", "configuration1");
    SoundSystem::RegisterConfiguration  ("driver2", "configuration2");
    SoundSystem::RegisterConfiguration  ("driver1", "configuration3");
    
    printf ("configurations:\n");
    
    for (size_t i=0; i<SoundSystem::GetConfigurationsCount (); i++)
      printf ("  %s\n", SoundSystem::GetConfiguration (i));

    SoundSystem::CreateDevice (SoundSystem::FindConfiguration ("*", "*3"), 0, "0");
  }
  catch (std::exception& exception)
  {
    printf ("Exception: %s\n", exception.what ());
  }

  return 0;
}
