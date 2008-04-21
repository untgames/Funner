#include <input/low_level/driver.h>

#include <common/singleton.h>
#include <common/strlib.h>
#include <common/exception.h>

#include <stl/hash_map>
#include <stl/string>

#include <xtl/intrusive_ptr.h>

using namespace input::low_level;
using namespace common;

namespace input
{

namespace low_level
{

/*
    Описание реализации системы управления низкоуровневыми драйверами устройств ввода
*/

class DriverManagerImpl
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация драйверов
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void RegisterDriver       (const char* name, IDriver* driver);
    void UnregisterDriver     (const char* name);
    void UnregisterAllDrivers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск драйвера по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDriver* FindDriver (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перечисление драйверов
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t   DriversCount ();
    IDriver* Driver       (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание устройства ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* driver_mask, const char* device_mask);

  private:
    typedef xtl::com_ptr<IDriver>      DriverPtr;
    typedef stl::hash_map<stl::string, DriverPtr> DriverMap;
    
  private:
    DriverMap drivers; //карта драйверов
};

}

}

/*
    Регистрация драйверов
*/

void DriverManagerImpl::RegisterDriver (const char* name, IDriver* driver)
{
  if (!name)
    RaiseNullArgument ("input::low_level::DriverManager::RegisterDriver", "name");

  if (!driver)
    RaiseNullArgument ("input::low_level::DriverManager::RegisterDriver", "driver");

  DriverMap::iterator iter = drivers.find (name);

  if (iter != drivers.end ())
    RaiseInvalidArgument ("input::low_level::DriverManager::RegisterDriver", "name", name,
                          "Driver with this name has been already registered");

  drivers.insert_pair (name, driver);
}

void DriverManagerImpl::UnregisterDriver (const char* name)
{
  if (!name)
    return;
    
  drivers.erase (name);
}

void DriverManagerImpl::UnregisterAllDrivers ()
{
  drivers.clear ();
}

/*
    Поиск драйвера по имени
*/

IDriver* DriverManagerImpl::FindDriver (const char* name)
{
  if (!name)
    return 0;
    
  DriverMap::iterator iter = drivers.find (name);
  
  return iter != drivers.end () ? get_pointer (iter->second) : 0;
}

/*
   Перечисление драйверов
*/

size_t DriverManagerImpl::DriversCount ()
{
  return drivers.size ();
}

IDriver* DriverManagerImpl::Driver (size_t index)
{
  if (index >= drivers.size ())
    RaiseOutOfRange ("input::low_level::DriverManager::Driver", "index", index, 0u, drivers.size ());

  DriverMap::iterator iter = drivers.begin ();

  for (size_t i = 0; i < index; ++iter, ++i);

  return get_pointer (iter->second);
}

/*
   Создание устройства ввода
*/

IDevice* DriverManagerImpl::CreateDevice (const char* driver_mask, const char* device_mask)
{
  if (!driver_mask)
    driver_mask = "*";
    
  if (!device_mask)
    device_mask = "*";

    //поиск драйвера

  for (DriverMap::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
    if (wcimatch (iter->first.c_str (), driver_mask))
      return iter->second->CreateDevice (device_mask); 

  return 0;
}

/*
    Обёртки над обращениями к системе управления низкоуровневыми драйверами отрисовки
*/

typedef Singleton<DriverManagerImpl> DriverManagerSingleton;

void DriverManager::RegisterDriver (const char* name, IDriver* driver)
{
  DriverManagerSingleton::Instance ().RegisterDriver (name, driver);
}

void DriverManager::UnregisterDriver (const char* name)
{
  DriverManagerSingleton::Instance ().UnregisterDriver (name);
}

void DriverManager::UnregisterAllDrivers ()
{
  DriverManagerSingleton::Instance ().UnregisterAllDrivers ();
}

IDriver* DriverManager::FindDriver (const char* name)
{
  return DriverManagerSingleton::Instance ().FindDriver (name);
}

size_t DriverManager::DriversCount ()
{
  return DriverManagerSingleton::Instance ().DriversCount ();
}

IDriver* DriverManager::Driver (size_t index)
{
  return DriverManagerSingleton::Instance ().Driver (index);
}

IDevice* DriverManager::CreateDevice (const char* driver_mask, const char* device_mask)
{
  return DriverManagerSingleton::Instance ().CreateDevice (driver_mask, device_mask);
}
