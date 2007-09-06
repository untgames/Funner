#include "shared.h"

using namespace sound::low_level;

namespace
{

//функция создания OpenAL устройства
ISoundDevice* create_device (const char* driver_name, const char* device_name, const void*, const char*)
{
  return new OpenALDevice (driver_name, device_name);  
}

}

namespace sound
{

namespace low_level
{

/*
    Регистрация драйвера воспроизведения звука на базе OpenAL
*/

void register_openal_driver (const char* name)
{ 
  SoundSystem::RegisterDriver (name, &create_device);

  try
  {
     const char* devices;

     if (alcIsExtensionPresent (NULL, "ALC_ENUMERATE_ALL_EXT"))
       devices = alcGetString (NULL, ALC_ALL_DEVICES_SPECIFIER);
     else if (alcIsExtensionPresent (NULL, "ALC_ENUMERATION_EXT"))
       devices = alcGetString (NULL, ALC_DEVICE_SPECIFIER);
     else
     {
       SoundSystem::RegisterConfiguration (name, "default");
       return;
     }

     for (; strlen (devices); devices += strlen (devices) + 1)
       SoundSystem::RegisterConfiguration (name, devices);
  }
  catch (...)
  {
    SoundSystem::UnregisterDriver (name);
    throw;
  }
}

}

}
