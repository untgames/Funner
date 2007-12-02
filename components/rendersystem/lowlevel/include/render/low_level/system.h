#ifndef RENDER_LOW_LEVEL_SYSTEM_HEADER
#define RENDER_LOW_LEVEL_SYSTEM_HEADER

#include <render/low_level/driver.h>

namespace render
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Система низкоуровневой отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
class RenderSystem
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация драйверов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static void RegisterDriver   (const char* name, IDriver* driver);
    static void UnregisterDriver (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск драйвера по имени
///////////////////////////////////////////////////////////////////////////////////////////////////
    static IDriver* FindDriver (const char* name);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
    static ISwapChain* CreateSwapChain (const char* driver_mask, const SwapChainDesc& swap_chain_desc);
    static ISwapChain* CreateSwapChain (const char* driver_mask, const char* output_mask, const SwapChainDesc& swap_chain_desc);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание устройства отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    static bool CreateSwapChainAndDevice (const char*          driver_mask,     //маска имени драйвера
                                          const SwapChainDesc& swap_chain_desc, //дескриптор цепочки обмена
                                          const char*          init_string,     //строка инициализации
                                          ISwapChain*&         out_swap_chain,  //результирующая цепочка обмена
                                          IDevice*&            out_device);     //результирующее устройство отрисовки
    static bool CreateSwapChainAndDevice (const char*          driver_mask,     //маска имени драйвера
                                          const char*          output_mask,     //маска имени устройства вывода
                                          const SwapChainDesc& swap_chain_desc, //дескриптор цепочки обмена
                                          const char*          init_string,     //строка инициализации
                                          ISwapChain*&         out_swap_chain,  //результирующая цепочка обмена
                                          IDevice*&            out_device);     //результирующее устройство отрисовки                                          
};

}

}

#endif
