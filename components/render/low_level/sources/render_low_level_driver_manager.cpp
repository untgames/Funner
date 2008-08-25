#include <stl/hash_map>
#include <stl/string>

#include <xtl/intrusive_ptr.h>
#include <xtl/common_exceptions.h>

#include <render/low_level/device.h>
#include <render/low_level/driver.h>

#include <common/singleton.h>
#include <common/component.h>
#include <common/strlib.h>

using namespace render::low_level;
using namespace common;

/*
    Константы
*/

namespace
{

const char* DRIVER_COMPONENTS_MASK = "render.low_level.*"; //маска имён компонентов низкоуровневых драйверов отрисовки

}

namespace render
{

namespace low_level
{

/*
    Описание реализации системы управления низкоуровневыми драйверами отрисовки
*/

class DriverManagerImpl
{
  public:
///Регистрация драйвера
    void RegisterDriver (const char* name, IDriver* driver)
    {
      static const char* METHOD_NAME = "render::low_level::DriverManager::RegisterDriver";

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

///Создание адаптера
    IAdapter* CreateAdapter (const char* driver_name, const char* adapter_name, const char* path)
    {
      static const char* METHOD_NAME = "render::low_level::DriverManagerImpl::CreateAdapter";

        //проверка корректности аргументов

      if (!driver_name)
        throw xtl::make_null_argument_exception (METHOD_NAME, "driver_name");

        //загрузка драйверов "по умолчанию"
        
      LoadDefaultDrivers ();
      
        //поиск драйвера
        
      DriverMap::iterator iter = drivers.find (driver_name);
      
      if (iter == drivers.end ())
        throw xtl::make_argument_exception (METHOD_NAME, "driver_name", driver_name, "Driver not registered");
        
        //создание адаптера

      return iter->second->CreateAdapter (adapter_name, path);
    }

///Создание цепочки обмена
    ISwapChain* CreateSwapChain
     (const char*          driver_mask,     //маска имени драйвера
      const char*          adapter_mask,    //маска имени адаптера
      const SwapChainDesc& swap_chain_desc) //дескриптор цепочки обмена
    {
        //поиск драйвера
        
      AdapterArray adapters;        

      DriverPtr driver = GetDriver (driver_mask, adapter_mask, adapters);

        //создание SwapChain и устройства отрисовки

      return driver->CreateSwapChain (adapters.size (), &adapters [0], swap_chain_desc);
    }

///Создание устройства отрисовки
    void CreateSwapChainAndDevice
     (const char*               driver_mask,     //маска имени драйвера
      const char*               adapter_mask,    //маска имени адаптера
      const SwapChainDesc&      swap_chain_desc, //дескриптор цепочки обмена
      const char*               init_string,     //строка инициализации
      xtl::com_ptr<ISwapChain>& out_swap_chain,  //результирующая цепочка обмена
      xtl::com_ptr<IDevice>&    out_device)      //результирующее устройство отрисовки
    {
        //проверка корректности аргументов

      if (!init_string)
        init_string = "";
        
        //поиск драйвера
        
      AdapterArray adapters;        

      DriverPtr driver = GetDriver (driver_mask, adapter_mask, adapters);

        //создание SwapChain и устройства отрисовки

      xtl::com_ptr<ISwapChain> swap_chain (driver->CreateSwapChain (adapters.size (), &adapters [0], swap_chain_desc), false);
      xtl::com_ptr<IDevice>    device (driver->CreateDevice (get_pointer (swap_chain), init_string), false);

      out_swap_chain = swap_chain;
      out_device     = device;
    }

  private:
    typedef stl::vector<IAdapter*> AdapterArray;

///Получение драйвера
    IDriver* GetDriver (const char* driver_mask, const char* adapter_mask, AdapterArray& adapters)
    {
        //проверка корректности аргументов

      if (!driver_mask)
        driver_mask = "*";
        
      if (!adapter_mask)
        adapter_mask = "*";

        //загрузка драйверов "по умолчанию"
        
      LoadDefaultDrivers ();
        
        //поиск драйвера
        
      for (DriverMap::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
        if (wcimatch (iter->first.c_str (), driver_mask))
        {
          IDriver* driver = iter->second.get ();
          
            //поиск предпочтительных адаптеров            

          adapters.clear   ();
          adapters.reserve (driver->GetAdaptersCount ());

          for (size_t i=0, count=driver->GetAdaptersCount (); i<count; i++)
          {
            IAdapter*   adapter      = driver->GetAdapter (i);
            const char* adapter_name = "";

            if (!adapter || !(adapter_name = adapter->GetName ()))
              continue; //проверка корректности параметров адаптера
              
            if (!wcimatch (adapter_name, adapter_mask))
              continue;
              
            adapters.push_back (adapter);
          }          
          
          if (!adapters.empty ())
            return driver;
        }

      throw xtl::format_operation_exception ("render::low_level::DriverManagerImpl::GetDriver",
        "No match driver found (driver_mask='%s', adapter_mask='%s')", driver_mask, adapter_mask);      
    }

///Загрузка драйверов по умолчанию
    void LoadDefaultDrivers ()
    {
      static ComponentLoader loader (DRIVER_COMPONENTS_MASK);
    }

  private:
    typedef xtl::com_ptr<IDriver>      DriverPtr;
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

IAdapter* DriverManager::CreateAdapter (const char* driver_name, const char* adapter_name, const char* path)
{
  return DriverManagerSingleton::Instance ().CreateAdapter (driver_name, adapter_name, path);
}

ISwapChain* DriverManager::CreateSwapChain
 (const char*          driver_mask,
  const char*          adapter_mask,
  const SwapChainDesc& swap_chain_desc)
{
  return DriverManagerSingleton::Instance ().CreateSwapChain (driver_mask, adapter_mask, swap_chain_desc);
}

void DriverManager::CreateSwapChainAndDevice
 (const char*               driver_mask,
  const char*               adapter_mask,
  const SwapChainDesc&      swap_chain_desc,
  const char*               init_string,
  xtl::com_ptr<ISwapChain>& out_swap_chain,
  xtl::com_ptr<IDevice>&    out_device)
{
  DriverManagerSingleton::Instance ().CreateSwapChainAndDevice (driver_mask, adapter_mask, swap_chain_desc, init_string, out_swap_chain, out_device);
}

void DriverManager::CreateSwapChainAndDevice
 (const char*          driver_mask,
  const char*          adapter_mask,
  const SwapChainDesc& swap_chain_desc,
  const char*          init_string,
  ISwapChain*&         out_swap_chain,
  IDevice*&            out_device)
{
  xtl::com_ptr<ISwapChain> swap_chain;
  xtl::com_ptr<IDevice>    device;

  CreateSwapChainAndDevice (driver_mask, adapter_mask, swap_chain_desc, init_string, swap_chain, device);

  swap_chain->AddRef ();
  device->AddRef ();

  out_swap_chain = &*swap_chain;
  out_device     = &*device;
}
