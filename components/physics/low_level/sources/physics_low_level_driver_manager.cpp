#include <stl/hash_map>
#include <stl/string>

#include <xtl/intrusive_ptr.h>
#include <xtl/common_exceptions.h>

#include <physics/low_level/driver.h>

#include <common/singleton.h>
#include <common/component.h>

using namespace physics::low_level;
using namespace common;

/*
    Константы
*/

namespace
{

const char* DRIVER_COMPONENTS_MASK = "physics.low_level.*"; //маска имён компонентов низкоуровневых драйверов физичиских систем

}

namespace physics
{

namespace low_level
{

/*
    Описание реализации системы управления низкоуровневыми драйверами физических систем
*/

class DriverManagerImpl
{
  public:
///Регистрация драйвера
    void RegisterDriver (const char* name, IDriver* driver)
    {
      static const char* METHOD_NAME = "physics::low_level::DriverManager::RegisterDriver";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!driver)
        throw xtl::make_null_argument_exception (METHOD_NAME, "driver");

      DriverMap::iterator iter = drivers.find (name);

      if (iter != drivers.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Driver with this name has been already registered");

      drivers.insert_pair (name, driver);
    }

///Отмена регистрации драйвера
    void UnregisterDriver (const char* name)
    {
      if (!name)
        return;
        
      drivers.erase (name);
    }

///Отмена регистрации всех драйверов
    void UnregisterAllDrivers ()
    {
      drivers.clear ();
    }

///Поиск драйвера по имени
    IDriver* FindDriver (const char* name)
    {
      if (!name)
        return 0;
        
      LoadDefaultDrivers ();
        
      DriverMap::iterator iter = drivers.find (name);
      
      return iter != drivers.end () ? get_pointer (iter->second) : 0;
    }

  private:
///Загрузка драйверов по умолчанию
    void LoadDefaultDrivers ()
    {
      static ComponentLoader loader (DRIVER_COMPONENTS_MASK);
    }

  private:
    typedef xtl::com_ptr<IDriver>                 DriverPtr;
    typedef stl::hash_map<stl::string, DriverPtr> DriverMap;

  private:
    DriverMap drivers; //карта драйверов
};

}

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
