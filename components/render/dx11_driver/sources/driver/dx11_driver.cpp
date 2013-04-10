#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

namespace
{

/*
    Константы
*/

const size_t ADAPTERS_RESERVE_SIZE = 8;

}

/*
    Конструктор / деструктор  
*/

Driver::Driver ()
{
  try
  {
      //создание фабрики

    IDXGIFactory* factory_ptr = 0;

    check_errors ("::CreateDXGIFactory", CreateDXGIFactory (__uuidof (IDXGIFactory), (void**)&factory_ptr));

    if (!factory_ptr)
      throw xtl::format_operation_exception ("", "::CreateDXGIFactory failed");

    factory = factory_ptr;

      //резервирование места для хранения адаптеров

    registered_adapters.reserve (ADAPTERS_RESERVE_SIZE);
    adapters.reserve (ADAPTERS_RESERVE_SIZE);

      //перечисление адаптеров

    IDXGIAdapter* dx_adapter = 0;

    for (UINT i=0; factory->EnumAdapters (i, &dx_adapter) != DXGI_ERROR_NOT_FOUND; i++)
    { 
      AdapterPtr adapter (new Adapter (DxAdapterPtr (dx_adapter)), false);

      RegisterAdapter (&*adapter);

      adapters.push_back (adapter);
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Driver::Driver");
    throw;
  }
}

Driver::~Driver ()
{
}
  
/*
    Описание драйвера
*/

const char* Driver::GetDescription ()
{
  return "render::low_level::dx11::Driver";
}

/*
    Получение возможностей драйвера
*/

void Driver::GetCaps (DriverCaps& caps)
{
  memset (&caps, 0, sizeof (caps));
}

/*
    Перечисление доступных адаптеров
*/

size_t Driver::GetAdaptersCount ()
{
  return registered_adapters.size ();
}

IAdapter* Driver::GetAdapter (size_t index)
{
  if (index >= registered_adapters.size ())
    throw xtl::make_range_exception ("render::low_level::dx11::Driver::GetAdapter", "index", index, registered_adapters.size ());

  return registered_adapters [index];
}

/*
    Регистрация адаптеров
*/

void Driver::RegisterAdapter (IAdapter* adapter)
{
  registered_adapters.push_back (adapter);

  try
  {      
    adapter->RegisterDestroyHandler (xtl::bind (&Driver::UnregisterAdapter, this, adapter), GetTrackable ());
  }
  catch (...)
  {
    registered_adapters.pop_back ();
    throw;
  }
}

void Driver::UnregisterAdapter (IAdapter* adapter)
{
  registered_adapters.erase (stl::remove (registered_adapters.begin (), registered_adapters.end (), adapter), registered_adapters.end ());
}

/*
    Создание адаптера
*/

IAdapter* Driver::CreateAdapter (const char* name, const char* path, const char* init_string)
{
  try
  {
      //проверка корректности аргументов
      
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    if (!path)
      throw xtl::make_null_argument_exception ("", "path");

      //создание адаптера

    typedef xtl::com_ptr<IAdapter> AdapterPtr;
    
    AdapterPtr adapter (new Adapter (factory, name, path, init_string), false);
    
      //регистрация адаптера в драйвере

    RegisterAdapter (&*adapter);

      //возвращение адаптера
      
    adapter->AddRef ();

    return &*adapter;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Driver::CreateAdapter");
    throw;
  }
}

/*
    Создание цепочки обмена
*/

ISwapChain* Driver::CreateSwapChain (size_t prefered_adapters_count, IAdapter** prefered_adapters, const SwapChainDesc& desc)
{
  throw xtl::format_operation_exception ("render::low_level::dx11::Driver::CreateSwapChain(size_t,IAdapter*,const SwapChainDesc&)", "Can't create swap chain without device");
}

ISwapChain* Driver::CreateSwapChain (IDevice* in_device, const SwapChainDesc& desc)
{
  try
  {
    if (!in_device)
      throw xtl::make_null_argument_exception ("", "device");

    Device* device = cast_object<Device> (in_device, "", "device");

    return new SwapChain (factory, device->GetAdapter (), device->GetDevice (), desc);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Driver::CreateSwapChain(IDevice*,const SwapChainDesc&)");
    throw;
  }
}

/*
    Создание устройства отрисовки
*/

IDevice* Driver::CreateDevice (ISwapChain* in_swap_chain, const char* init_string)
{
  try
  {
    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    SwapChain* swap_chain = cast_object<SwapChain> (in_swap_chain, "", "swap_chain");

    IAdapter* adapter = swap_chain->GetAdapter ();

    return CreateDevice (1, &adapter, init_string);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Driver::CreateDevice(ISwapChain*,const char*)");
    throw;
  }
}

IDevice* Driver::CreateDevice (size_t prefered_adapters_count, IAdapter** prefered_adapters, const char* init_string)
{
  static const char* METHOD_NAME = "render::low_level::dx11::Driver::CreateDevice(ISwapChain*,const char*)";

  try
  {
    if (!prefered_adapters_count)
      throw xtl::make_null_argument_exception ("", "prefered_adapters_count");

    if (!prefered_adapters)
      throw xtl::make_null_argument_exception ("", "prefered_adapters");    

    for (size_t i=0; i<prefered_adapters_count; i++)
    {
      IAdapter* src_adapter = prefered_adapters [i];

      if (!src_adapter)
        throw xtl::format_exception<xtl::null_argument_exception> ("", "Null argument prefered_adapters[%u]", i);

      try
      {
        AdapterPtr adapter = cast_object<Adapter> (src_adapter, "", "adapter");

        return new Device (adapter, init_string);
      }
      catch (std::exception& e)
      {
        log.Printf ("Attempt to create device for adapter '%s' failed. %s\n    at %s", src_adapter->GetName (), e.what (), METHOD_NAME);

        if (i == prefered_adapters_count-1)
        {
          if (i == 0)
            throw;

          throw xtl::format_operation_exception ("", "Can't create DX11 device");
        }
      }
      catch (...)
      {
        log.Printf ("Attempt to create device for adapter '%s' failed. Unknown exception\n    at %s", src_adapter->GetName (), METHOD_NAME);

        if (i == prefered_adapters_count-1)
        {
          if (i == 0)
            throw;

          throw xtl::format_operation_exception ("", "Can't create DX11 device");
        }
      }
    }

    throw xtl::format_operation_exception ("", "Can't create DX11 device. No adapters");
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
    throw;
  }
}

/*
    Установка функции отладочного протоколирования драйвера
*/

void Driver::SetDebugLog (const LogFunction& fn)
{
  dummy_log_fn = fn;
}

const LogFunction& Driver::GetDebugLog ()
{
  return dummy_log_fn;
}
