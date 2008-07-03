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
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация драйверов
///////////////////////////////////////////////////////////////////////////////////////////////////    
    void RegisterDriver   (const char* name, IDriver* driver);
    void UnregisterDriver (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск драйвера по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDriver* FindDriver (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* CreateSwapChain (const char* driver_mask, const SwapChainDesc& swap_chain_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void CreateSwapChainAndDevice (const char*               driver_mask,     //маска имени драйвера
                                   const SwapChainDesc&      swap_chain_desc, //дескриптор цепочки обмена
                                   const char*               init_string,     //строка инициализации
                                   xtl::com_ptr<ISwapChain>& out_swap_chain,  //результирующая цепочка обмена
                                   xtl::com_ptr<IDevice>&    out_device);     //результирующее устройство отрисовки

  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Загрузка драйверов по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    void LoadDefaultDrivers ();

  private:
    typedef xtl::com_ptr<IDriver>      DriverPtr;
    typedef stl::hash_map<stl::string, DriverPtr> DriverMap;
    
  private:
    DriverMap drivers; //карта драйверов
};

}

}

/*
    Загрузка драйверов по умолчанию
*/

void DriverManagerImpl::LoadDefaultDrivers ()
{
  try
  {
    static ComponentLoader loader (DRIVER_COMPONENTS_MASK);
  }
  catch (...)
  {
  }
}

/*
    Регистрация драйверов
*/

void DriverManagerImpl::RegisterDriver (const char* name, IDriver* driver)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::low_level::DriverManager::RegisterDriver", "name");

  if (!driver)
    throw xtl::make_null_argument_exception ("render::low_level::DriverManager::RegisterDriver", "driver");

  DriverMap::iterator iter = drivers.find (name);

  if (iter != drivers.end ())
    throw xtl::make_argument_exception ("render::low_level::DriverManager::RegisterDriver", "name", name,
                          "Driver with this name has been already registered");

  drivers.insert_pair (name, driver);
}

void DriverManagerImpl::UnregisterDriver (const char* name)
{
  if (!name)
    return;
    
  drivers.erase (name);
}

/*
    Поиск драйвера по имени
*/

IDriver* DriverManagerImpl::FindDriver (const char* name)
{
  if (!name)
    return 0;
    
  LoadDefaultDrivers ();
    
  DriverMap::iterator iter = drivers.find (name);
  
  return iter != drivers.end () ? get_pointer (iter->second) : 0;
}

/*
    Создание цепочки обмена
*/

ISwapChain* DriverManagerImpl::CreateSwapChain (const char* driver_mask, const SwapChainDesc& swap_chain_desc)
{
  if (!driver_mask)
    driver_mask = "*";
    
  LoadDefaultDrivers ();
    
    //поиск драйвера

  for (DriverMap::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
    if (wcimatch (iter->first.c_str (), driver_mask))
      return iter->second->CreateSwapChain (swap_chain_desc); 

  return 0;
}

/*
    Создание устройства отрисовки
*/

void DriverManagerImpl::CreateSwapChainAndDevice
 (const char*               driver_mask,      //маска имени драйвера
  const SwapChainDesc&      swap_chain_desc,  //дескриптор цепочки обмена
  const char*               init_string,      //строка инициализации
  xtl::com_ptr<ISwapChain>& out_swap_chain,   //результирующая цепочка обмена
  xtl::com_ptr<IDevice>&    out_device)       //результирующее устройство отрисовки
{
  if (!driver_mask)
    driver_mask = "*";
    
  if (!init_string)
    init_string = "";
    
  LoadDefaultDrivers ();
    
    //поиск драйвера
    
  for (DriverMap::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
    if (wcimatch (iter->first.c_str (), driver_mask))
    {
      DriverPtr driver = iter->second;

        //создание SwapChain и устройства отрисовки

      xtl::com_ptr<ISwapChain> swap_chain (driver->CreateSwapChain (swap_chain_desc), false);
      xtl::com_ptr<IDevice>    device (driver->CreateDevice (get_pointer (swap_chain), init_string), false);      

      out_swap_chain = swap_chain;
      out_device     = device;

      return;
    }

  throw xtl::format_operation_exception ("render::low_level::DriverManagerImpl::CreateSwapChainAndDevice",
    "No match driver found (driver_mask='%s')", driver_mask);
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

IDriver* DriverManager::FindDriver (const char* name)
{
  return DriverManagerSingleton::Instance ().FindDriver (name);
}

ISwapChain* DriverManager::CreateSwapChain (const char* driver_mask, const SwapChainDesc& swap_chain_desc)
{
  return DriverManagerSingleton::Instance ().CreateSwapChain (driver_mask, swap_chain_desc);
}

void DriverManager::CreateSwapChainAndDevice
 (const char*               driver_mask,
  const SwapChainDesc&      swap_chain_desc,
  const char*               init_string,
  xtl::com_ptr<ISwapChain>& out_swap_chain,
  xtl::com_ptr<IDevice>&    out_device)
{
  DriverManagerSingleton::Instance ().CreateSwapChainAndDevice (driver_mask, swap_chain_desc, init_string, out_swap_chain, out_device);
}

void DriverManager::CreateSwapChainAndDevice
 (const char*          driver_mask,
  const SwapChainDesc& swap_chain_desc,
  const char*          init_string,
  ISwapChain*&         out_swap_chain,
  IDevice*&            out_device)
{
  xtl::com_ptr<ISwapChain> swap_chain;
  xtl::com_ptr<IDevice>    device;

  CreateSwapChainAndDevice (driver_mask, swap_chain_desc, init_string, swap_chain, device);
  
  swap_chain->AddRef ();
  device->AddRef ();
  
  out_swap_chain = &*swap_chain;
  out_device     = &*device;
}
