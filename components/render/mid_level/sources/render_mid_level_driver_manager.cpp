#include <stl/vector>
#include <stl/string>
#include <stl/algorithm>

#include <xtl/intrusive_ptr.h>
#include <xtl/common_exceptions.h>

#include <common/singleton.h>
#include <common/component.h>
#include <common/strlib.h>

#include <render/mid_level/driver.h>

using namespace render::mid_level;
using namespace common;

namespace
{

const size_t DRIVER_ARRAY_RESERVE = 5; //резервируемый размер массива драйверов 

/*
    Описание реализации системы управления драйверами систем рендеринга
*/

class DriverManagerImpl
{
  public:
///Конструктор
    DriverManagerImpl ()
    {
      drivers.reserve (DRIVER_ARRAY_RESERVE);
    }

///Регистрация драйверов
    void RegisterDriver (const char* name, IDriver* driver)
    {
      static const char* METHOD_NAME = "input::mid_level::DriverManager::RegisterDriver";

      if (!name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "name");

      if (!driver)
        throw xtl::make_null_argument_exception (METHOD_NAME, "driver");

      for (DriverArray::iterator i = drivers.begin (); i != drivers.end (); ++i)
        if (i->name == name)
          throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Driver with this name has been already registered");

      drivers.push_back (RendererDriver (driver, name));
    }

///Отмена регистрации драйвера
    void UnregisterDriver (const char* name)
    {
      if (!name)
        return;

      for (DriverArray::iterator i = drivers.begin (); i != drivers.end (); ++i)
        if (i->name == name)
        {
          drivers.erase (i);
          return;
        }
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

      for (DriverArray::iterator i = drivers.begin (); i != drivers.end (); ++i)
        if (i->name == name)
          return get_pointer (i->driver);

      return 0;
    }

///Получение количества зарегистрированных драйверов
    size_t DriversCount ()
    {
      LoadDefaultDrivers ();

      return drivers.size ();
    }

///Получение драйвера по индексу
    IDriver* Driver (size_t index)
    {
      LoadDefaultDrivers ();

      if (index >= drivers.size ())
        throw xtl::make_range_exception ("input::mid_level::DriverManager::Driver", "index", index, drivers.size ());

      return get_pointer (drivers [index].driver);      
    }
    
///Получение имени драйвера по индексу
    const char* DriverName (size_t index)
    {
      LoadDefaultDrivers ();      
      
      if (index >= drivers.size ())
        throw xtl::make_range_exception ("input::mid_level::DriverManager::Driver", "index", index, drivers.size ());

      return drivers [index].name.c_str ();      
    }

///Создание системы рендеринга
    IRenderer* CreateRenderer (const char* driver_mask, const char* renderer_mask)
    {
      if (!driver_mask)
        driver_mask = "*";
        
      if (!renderer_mask)
        renderer_mask = "*";
        
      LoadDefaultDrivers ();

        //поиск драйвера

      for (DriverArray::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
        if (wcimatch (iter->name.c_str (), driver_mask))
        {
          IDriver& driver = *iter->driver;
          
          for (size_t i=0, count=driver.GetRenderersCount (); i < count; i++)
          {
            if (wcimatch (driver.GetRendererName (i), renderer_mask))
              return driver.CreateRenderer (driver.GetRendererName (i));
          }
        }

      throw xtl::format_operation_exception ("render::mid_level::DriverManagerImpl::CreateRenderer",
        "No configuration with driver_mask='%s' and renderer_mask='%s'", driver_mask, renderer_mask);
    }
    
  private:
    void LoadDefaultDrivers ()
    {
      static common::ComponentLoader loader ("render.mid_level.*");
    }

  private:
    typedef xtl::com_ptr<IDriver> DriverPtr;

    struct RendererDriver
    {
      RendererDriver (IDriver* in_driver, const char* in_name) : driver (in_driver), name (in_name) {} 

      DriverPtr   driver;
      stl::string name;
    };

    typedef stl::vector<RendererDriver> DriverArray;
  
  private:
    DriverArray drivers; //массив драйверов
};

}

/*
    RendererDriverManager
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

IRenderer* DriverManager::CreateRenderer (const char* driver_mask, const char* renderer_mask)
{
  return DriverManagerSingleton::Instance ()->CreateRenderer (driver_mask, renderer_mask);
}
