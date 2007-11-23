#include <render/low_level/system.h>

#include <common/singleton.h>
#include <common/strlib.h>
#include <common/exception.h>

#include <stl/hash_map>
#include <stl/string>

#include <xtl/intrusive_ptr.h>

using namespace render::low_level;
using namespace common;

namespace render
{

namespace low_level
{

/*
    Описание реализации системы управления низкоуровневыми драйверами отрисовки
*/

class RenderSystemImpl
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
///Создание устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    IDevice* CreateDevice (const char*          driver_mask,       //маска имени драйвера
                           const SwapChainDesc& swap_chain,        //дескриптор цепочки обмена
                           const char*          init_string = ""); //строка инициализации

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

void RenderSystemImpl::RegisterDriver (const char* name, IDriver* driver)
{
  if (!name)
    RaiseNullArgument ("render::low_level::RenderSystem::RegisterDriver", "name");

  if (!driver)
    RaiseNullArgument ("render::low_level::RenderSystem::RegisterDriver", "driver");

  DriverMap::iterator iter = drivers.find (name);

  if (iter != drivers.end ())
    RaiseInvalidArgument ("render::low_level::RenderSystem::RegisterDriver", "name", name,
                          "Driver with this name has been already registered");

  drivers.insert_pair (name, driver);
}

void RenderSystemImpl::UnregisterDriver (const char* name)
{
  if (!name)
    return;
    
  drivers.erase (name);
}

/*
    Поиск драйвера по имени
*/

IDriver* RenderSystemImpl::FindDriver (const char* name)
{
  if (!name)
    return 0;
    
  DriverMap::iterator iter = drivers.find (name);
  
  return iter != drivers.end () ? get_pointer (iter->second) : 0;
}

/*
    Создание устройства отрисовки
*/

IDevice* RenderSystemImpl::CreateDevice
 (const char*          driver_mask,      //маска имени драйвера
  const SwapChainDesc& swap_chain_desc,  //дескриптор цепочки обмена
  const char*          init_string)      //строка инициализации
{
  if (!driver_mask)
    return 0;
    
  if (!init_string)
    init_string = "";
    
    //поиск драйвера
    
  for (DriverMap::iterator iter=drivers.begin (), end=drivers.end (); iter != end; ++iter)
    if (wcimatch (iter->first.c_str (), driver_mask))
    {
      DriverPtr driver = iter->second;

        //создание SwapChain и устройства отрисовки

      xtl::com_ptr<ISwapChain> swap_chain (driver->CreateSwapChain (swap_chain_desc), false);

      return driver->CreateDevice (get_pointer (swap_chain), init_string);
    }

  return 0;
}

/*
    Обёртки над обращениями к системе управления низкоуровневыми драйверами отрисовки
*/

typedef Singleton<RenderSystemImpl> RenderSystemSingleton;

void RenderSystem::RegisterDriver (const char* name, IDriver* driver)
{
  RenderSystemSingleton::Instance ().RegisterDriver (name, driver);
}

void RenderSystem::UnregisterDriver (const char* name)
{
  RenderSystemSingleton::Instance ().UnregisterDriver (name);
}

IDriver* RenderSystem::FindDriver (const char* name)
{
  return RenderSystemSingleton::Instance ().FindDriver (name);
}

IDevice* RenderSystem::CreateDevice (const char* driver_mask, const SwapChainDesc& swap_chain, const char* init_string)
{
  return RenderSystemSingleton::Instance ().CreateDevice (driver_mask, swap_chain, init_string);
}
