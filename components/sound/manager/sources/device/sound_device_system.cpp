#include <sound/device.h>
#include <stl/string>
#include <stl/hash_map>
#include <stl/vector>
#include <stl/algorithm>
#include <common/strlib.h>
#include <common/singleton.h>
#include <common/exception.h>
#include <xtl/function.h>

using namespace sound::low_level;
using namespace common;
using namespace stl;

namespace sound
{

namespace low_level
{

/*
    Описание реализации системы управления устройствами воспроизведения звука
*/

class SoundSystemImpl
{
  public:
    typedef SoundSystem::CreateDeviceHandler CreateDeviceHandler;
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация драйверов
///////////////////////////////////////////////////////////////////////////////////////////////////  
    void RegisterDriver       (const char* driver_name, const CreateDeviceHandler& creater);
    void UnregisterDriver     (const char* driver_name);
    void UnregisterAllDrivers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация конфигураций
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* RegisterConfiguration       (const char* driver_name, const char* device_name);
    void        UnregisterConfiguration     (const char* configuration_name);
    void        UnregisterConfiguration     (const char* driver_name, const char* device_name);
    void        UnregisterAllConfigurations (const char* driver_mask, const char* device_mask);
    void        UnregisterAllConfigurations ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление доступных конфигураций
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t      GetConfigurationsCount () const;
    const char* GetConfiguration       (size_t index) const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск конфигурации
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* FindConfiguration (const char* driver_mask, const char* device_mask) const; //в случае неудачи - 0

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание устройства воспроизведения звука
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISoundDevice* CreateDevice (const char* driver_name, const char* device_name, const void* window_handle, const char* init_string);
    ISoundDevice* CreateDevice (const char* configuration_name, const void* window_handle, const char* init_string = "");
    
  private:
    static string GetConfigurationName (const char* driver_name, const char* device_name);
  
  private:
    typedef hash_map<hash_key<const char*>, CreateDeviceHandler> DriverMap;
  
    struct Configuration
    {      
      string              name;
      string              driver_name, device_name;
      DriverMap::iterator driver_iter;
    };
  
    typedef hash_map<hash_key<const char*>, Configuration> ConfigurationMap;
    typedef vector<ConfigurationMap::iterator>             ConfigurationArray;

  private:
    DriverMap          drivers;
    ConfigurationMap   configurations;
    ConfigurationArray configuration_array;
};

}

}

/*
    Регистрация драйверов
*/

void SoundSystemImpl::RegisterDriver (const char* driver_name, const CreateDeviceHandler& creater)
{
  if (!driver_name)
    raise_null_argument ("sound::low_level::SoundSystem::RegisterDriver", "driver_name");
    
  if (!creater)
    raise_null_argument ("sound::low_level::SoundSystem::RegisterDriver", "creater");
    
  DriverMap::iterator iter = drivers.find (driver_name);
  
  if (iter != drivers.end ())
    raise_invalid_argument ("sound::low_level::SoundSystem::RegisterDriver", "driver_name", driver_name,
                          "Driver with this name already registered");

  drivers.insert_pair (driver_name, creater);
}

void SoundSystemImpl::UnregisterDriver (const char* driver_name)
{
  if (!driver_name)
    raise_null_argument ("sound::low_level::SoundSystem::UnregisterDriver", "driver_name");

  drivers.erase (driver_name);

  UnregisterAllConfigurations (driver_name, "*");
}

void SoundSystemImpl::UnregisterAllDrivers ()
{
  drivers.clear ();
  UnregisterAllConfigurations ();
}

/*
    Получение имени конфигурации
*/

string SoundSystemImpl::GetConfigurationName (const char* driver_name, const char* device_name)
{
  return format ("%s::%s", driver_name, device_name);
}

/*
    Регистрация конфигураций
*/

const char* SoundSystemImpl::RegisterConfiguration (const char* driver_name, const char* device_name)
{
  if (!driver_name)
    raise_null_argument ("sound::low_level::SoundSystem::RegisterConfiguration", "driver_name");

  if (!device_name)
    raise_null_argument ("sound::low_level::SoundSystem::RegisterConfiguration", "device_name");

  DriverMap::iterator driver_iter = drivers.find (driver_name);

  if (driver_iter == drivers.end ())
    raise_invalid_argument ("sound::low_level::SoundSystem::RegisterConfiguration", "driver_name", driver_name,
                          "There is no driver with this name");

    //формирование имени конфигурации

  string configuration_name;

  GetConfigurationName (driver_name, device_name).swap (configuration_name);

    //проверка наличия конфигурации

  ConfigurationMap::iterator cfg_iter = configurations.find (configuration_name.c_str ());

  if (cfg_iter != configurations.end ())
    return cfg_iter->second.name.c_str (); //конфигурация с таким именем уже зарегистрирована
    
  cfg_iter = configurations.insert_pair (configuration_name.c_str (), Configuration ()).first;
  
  try
  {  
    Configuration& configuration = cfg_iter->second;
    
    configuration.name.swap (configuration_name);
    
    configuration.driver_name = driver_name;
    configuration.device_name = device_name;
    configuration.driver_iter = driver_iter;
    
    configuration_array.push_back (cfg_iter);
    
    return configuration.name.c_str ();
  }
  catch (...)
  {
    configurations.erase (cfg_iter);
    throw;
  }
}

void SoundSystemImpl::UnregisterConfiguration (const char* configuration_name)
{
  if (!configuration_name)
    raise_null_argument ("sound::low_level::SoundSystemImpl::UnregisterConfiguration", "configuration_name");
    
  ConfigurationMap::iterator cfg_iter = configurations.find (configuration_name);

  if (cfg_iter == configurations.end ())
    return; //конфигурация с таким именем отсутствует

  configuration_array.erase (remove (configuration_array.begin (), configuration_array.end (), cfg_iter), configuration_array.end ());

  configurations.erase (cfg_iter);
}

void SoundSystemImpl::UnregisterConfiguration (const char* driver_name, const char* device_name)
{
  if (!driver_name || !device_name)
    return;

  for (ConfigurationMap::iterator i=configurations.begin (), end=configurations.end (); i!=end; ++i)
    if (i->second.driver_name == driver_name && i->second.device_name == device_name)
    {      
      configuration_array.erase (remove (configuration_array.begin (), configuration_array.end (), i), configuration_array.end ());      
      
      configurations.erase (i);
      return;
    }
}

void SoundSystemImpl::UnregisterAllConfigurations (const char* driver_mask, const char* device_mask)
{
  if (!driver_mask || !device_mask)
    return;

  for (ConfigurationMap::iterator i=configurations.begin (), end=configurations.end (); i!=end;)
    if (wcmatch (i->second.driver_name.c_str (), driver_mask) && wcmatch (i->second.device_name.c_str (), device_mask))
    {      
      configuration_array.erase (remove (configuration_array.begin (), configuration_array.end (), i), configuration_array.end ());      
      
      ConfigurationMap::iterator next = i;
      
      ++next;
      
      configurations.erase (i);

      i = next;
    }
    else ++i;
}

void SoundSystemImpl::UnregisterAllConfigurations ()
{
  configurations.clear ();
  configuration_array.clear ();
}

/*
    Перечисление доступных конфигураций
*/

size_t SoundSystemImpl::GetConfigurationsCount () const
{
  return configuration_array.size ();
}

const char* SoundSystemImpl::GetConfiguration (size_t index) const
{
  if (index >= configuration_array.size ())
    raise_out_of_range ("sound::low_level::SoundSystem::GetConfiguration", "index", index, configuration_array.size ());
    
  return configuration_array [index]->second.name.c_str ();
}

/*
    Создание устройства воспроизведения звука
*/

ISoundDevice* SoundSystemImpl::CreateDevice (const char* configuration_name, const void* window_handle, const char* init_string)
{
  if (!configuration_name)
    raise_null_argument ("sound::low_level::SoundSystem::CreateDevice", "configuration_name");
    
  if (!init_string)
    init_string = "";

  ConfigurationMap::iterator cfg_iter = configurations.find (configuration_name);

  if (cfg_iter == configurations.end ())
    raise_invalid_argument ("sound::low_level::SoundSystem::CreateDevice", "configuration_name", configuration_name,
                          "There is no configuration with this name");
                          
  Configuration& cfg = cfg_iter->second;

  return cfg.driver_iter->second (cfg.driver_name.c_str (), cfg.device_name.c_str (), window_handle, init_string);  
}
    
ISoundDevice* SoundSystemImpl::CreateDevice (const char* driver_name, const char* device_name, const void* window_handle, const char* init_string)
{
  if (!driver_name)
    raise_null_argument ("sound::low_level::SoundSystemImpl::CreateDevice", "driver_name");

  if (!device_name)
    raise_null_argument ("sound::low_level::SoundSystemImpl::CreateDevice", "device_name");

  return CreateDevice (GetConfigurationName (driver_name, device_name).c_str (), window_handle, init_string);
}

/*
    Поиск конфигурации
*/

//в случае неудачи - 0
const char* SoundSystemImpl::FindConfiguration (const char* driver_mask, const char* device_mask) const
{
  if (!driver_mask || !device_mask)
    return 0;

  for (ConfigurationMap::const_iterator i=configurations.begin (), end=configurations.end (); i!=end; ++i)
    if (wcmatch (i->second.driver_name.c_str (), driver_mask) && wcmatch (i->second.device_name.c_str (), device_mask))
      return i->second.name.c_str ();
      
  return 0;
}

/*
    Обёртки над обращениями к системе управления устройствами воспроизведения звука
*/

typedef Singleton<sound::low_level::SoundSystemImpl> SoundSystemSingleton;

void SoundSystem::RegisterDriver (const char* driver_name, const CreateDeviceHandler& creater)
{
  SoundSystemSingleton::Instance ().RegisterDriver (driver_name, creater);
}

void SoundSystem::UnregisterDriver (const char* driver_name)
{
  SoundSystemSingleton::Instance ().UnregisterDriver (driver_name);
}

void SoundSystem::UnregisterAllDrivers ()
{
  SoundSystemSingleton::Instance ().UnregisterAllDrivers ();
}

const char* SoundSystem::RegisterConfiguration (const char* driver_name, const char* device_name)
{
  return SoundSystemSingleton::Instance ().RegisterConfiguration (driver_name, device_name);
}

void SoundSystem::UnregisterConfiguration (const char* configuration_name)
{
  SoundSystemSingleton::Instance ().UnregisterConfiguration (configuration_name);
}

void SoundSystem::UnregisterConfiguration (const char* driver_name, const char* device_name)
{
  SoundSystemSingleton::Instance ().UnregisterConfiguration (driver_name, device_name);
}

void SoundSystem::UnregisterAllConfigurations (const char* driver_mask, const char* device_mask)
{
  SoundSystemSingleton::Instance ().UnregisterAllConfigurations (driver_mask, device_mask);
}

void SoundSystem::UnregisterAllConfigurations ()
{
  SoundSystemSingleton::Instance ().UnregisterAllConfigurations ();
}

size_t SoundSystem::GetConfigurationsCount ()
{
  return SoundSystemSingleton::Instance ().GetConfigurationsCount ();
}

const char* SoundSystem::GetConfiguration (size_t index)
{
  return SoundSystemSingleton::Instance ().GetConfiguration (index);
}

const char* SoundSystem::FindConfiguration (const char* driver_mask, const char* device_mask)
{
  return SoundSystemSingleton::Instance ().FindConfiguration (driver_mask, device_mask);
}

ISoundDevice* SoundSystem::CreateDevice (const char* driver_name, const char* device_name, const void* window_handle, const char* init_string)
{
  return SoundSystemSingleton::Instance ().CreateDevice (driver_name, device_name, window_handle, init_string);
}

ISoundDevice* SoundSystem::CreateDevice (const char* configuration_name, const void* window_handle, const char* init_string)
{
  return SoundSystemSingleton::Instance ().CreateDevice (configuration_name, window_handle, init_string);
}
