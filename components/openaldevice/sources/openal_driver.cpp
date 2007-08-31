#include "shared.h"

using namespace sound::low_level;

namespace
{

//функция создания OpenAL устройства
ISoundDevice* create_device (const char*, const char* device_name, const void*, const char*)
{
  return new OpenALDevice (device_name);
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
     //здесь будут регистрироваться конфигурации
     
     SoundSystem::RegisterConfiguration (name, "default");
  }
  catch (...)
  {
    SoundSystem::UnregisterAllConfigurations (name, "*");
    SoundSystem::UnregisterDriver (name);
    throw;
  }
}

}

}
