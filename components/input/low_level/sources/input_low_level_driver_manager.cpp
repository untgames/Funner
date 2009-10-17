#include <stl/vector>
#include <stl/string>

#include <xtl/intrusive_ptr.h>
#include <xtl/common_exceptions.h>

#include <common/component.h>
#include <common/singleton.h>
#include <common/strlib.h>

#include <input/low_level/driver.h>

using namespace input::low_level;
using namespace common;

const size_t DRIVER_ARRAY_RESERVE = 4; //резервируемый размер массива драйверов

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
    DriverManagerImpl ()
    {
      drivers.reserve (DRIVER_ARRAY_RESERVE);
    }

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
    size_t      DriversCount ();
    IDriver*    Driver       (size_t index);
    const char* DriverName   (size_t index);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание устройства ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char* driver_mask, const char* device_mask, const char* init_string = "");

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка драйверов по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadDefaultDrivers ();

  private:
    typedef xtl::com_ptr<IDriver>  DriverPtr;
    struct InputDriver
    {
      InputDriver (IDriver* in_driver, const char* in_name) : driver (in_driver), name (in_name) {} 

      DriverPtr   driver;
      stl::string name;
    };
    typedef stl::vector<InputDriver> DriverArray;
  
  private:
    DriverArray drivers; //карта драйверов
};

}

}

/*
    Регистрация драйверов
*/

void DriverManagerImpl::RegisterDriver (const char* name, IDriver* driver)
{
  static const char* METHOD_NAME = "input::low_level::DriverManager::RegisterDriver";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (!driver)
    throw xtl::make_null_argument_exception (METHOD_NAME, "driver");

  for (DriverArray::iterator i = drivers.begin (); i != drivers.end (); ++i)
    if (i->name == name)
      throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Driver with this name has been already registered");

  drivers.push_back (InputDriver (driver, name));
}

void DriverManagerImpl::UnregisterDriver (const char* name)
{
  if (!name)
    return;
    
  for (DriverArray::iterator i = drivers.begin (); i != drivers.end (); ++i)
    if (i->name == name)
      drivers.erase (i--);
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
    
  LoadDefaultDrivers ();

  for (DriverArray::iterator i = drivers.begin (); i != drivers.end (); ++i)
    if (i->name == name)
      return get_pointer (i->driver);
  
  return 0;
}

/*
   Перечисление драйверов
*/

size_t DriverManagerImpl::DriversCount ()
{
  LoadDefaultDrivers ();

  return drivers.size ();
}

IDriver* DriverManagerImpl::Driver (size_t index)
{
  LoadDefaultDrivers ();

  if (index >= drivers.size ())
    throw xtl::make_range_exception ("input::low_level::DriverManager::Driver", "index", index, 0u, drivers.size ());

  return get_pointer (drivers [index].driver);
}

const char* DriverManagerImpl::DriverName (size_t index)
{
  LoadDefaultDrivers ();

  if (index >= drivers.size ())
    throw xtl::make_range_exception ("input::low_level::DriverManager::Driver", "index", index, 0u, drivers.size ());

  return drivers [index].name.c_str ();
}

/*
   Создание устройства ввода
*/

IDevice* DriverManagerImpl::CreateDevice (const char* driver_mask, const char* device_mask, const char* init_string)
{
  LoadDefaultDrivers ();

  if (!driver_mask)
    driver_mask = "*";
    
  if (!device_mask)
    device_mask = "*";

  if (!init_string)
    init_string = "";

    //поиск драйвера

  for (DriverArray::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
    if (wcimatch (iter->name.c_str (), driver_mask))
    {
      for (size_t i = 0; i < iter->driver->GetDevicesCount (); i++)
      {
        if (wcimatch (iter->driver->GetDeviceName (i), device_mask))
          return iter->driver->CreateDevice (iter->driver->GetDeviceName (i), init_string);
      }
    }
    
  throw xtl::format_operation_exception ("input::low_level::DriverManagerImpl::CreateDevice",
    "No configuration with driver_mask='%s' and device_mask='%s'", driver_mask, device_mask);
}

/*
   Загрузка драйверов по умолчанию
*/

void DriverManagerImpl::LoadDefaultDrivers ()
{
  static ComponentLoader loader ("input.low_level.*");
}

/*
    Обёртки над обращениями к системе управления низкоуровневыми драйверами отрисовки
*/

typedef Singleton<DriverManagerImpl> DriverManagerSingleton;

void DriverManager::RegisterDriver (const char* name, IDriver* driver)
{
  DriverManagerSingleton::Instance ()->RegisterDriver (name, driver);
}

void DriverManager::UnregisterDriver (const char* name)
{
  DriverManagerSingleton::Instance ()->UnregisterDriver (name);
}

void DriverManager::UnregisterAllDrivers ()
{
  DriverManagerSingleton::Instance ()->UnregisterAllDrivers ();
}

IDriver* DriverManager::FindDriver (const char* name)
{
  return DriverManagerSingleton::Instance ()->FindDriver (name);
}

size_t DriverManager::DriversCount ()
{
  return DriverManagerSingleton::Instance ()->DriversCount ();
}

IDriver* DriverManager::Driver (size_t index)
{
  return DriverManagerSingleton::Instance ()->Driver (index);
}

const char* DriverManager::DriverName (size_t index)
{
  return DriverManagerSingleton::Instance ()->DriverName (index);
}

IDevice* DriverManager::CreateDevice (const char* driver_mask, const char* device_mask, const char* init_string)
{
  return DriverManagerSingleton::Instance ()->CreateDevice (driver_mask, device_mask, init_string);
}
