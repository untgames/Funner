#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    Константы
*/

const char*  DRIVER_NAME                = "OpenGL";                  //имя драйвера
const char*  COMPONENT_NAME             = "render.low_level.opengl"; //имя компонента
const size_t ADAPTER_ARRAY_RESERVE_SIZE = 4;                         //резервируемое количество адаптеров отрисовки

}

/*
    Конструктор / деструктор
*/

Driver::Driver ()
{
  try
  {
      //резервирование места для адаптера

    adapters.reserve (ADAPTER_ARRAY_RESERVE_SIZE);

      //регистрация адаптеров "по умолчанию"

    PlatformManager::EnumDefaultAdapters (xtl::bind (&Driver::RegisterAdapter, this, _1));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Driver::Driver");
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
  return "render::low_level::opengl::Driver";
}

/*
    Перечисление доступных адаптеров
*/

size_t Driver::GetAdaptersCount ()
{
  return adapters.size ();
}

IAdapter* Driver::GetAdapter (size_t index)
{
  if (index >= adapters.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::Driver::GetAdapter", "index", index, adapters.size ());
    
  return adapters [index];
}

/*
    Регистрация адаптеров
*/

void Driver::RegisterAdapter (IAdapter* adapter)
{
  adapters.push_back (adapter);

  try
  {      
    adapter->RegisterDestroyHandler (xtl::bind (&Driver::UnregisterAdapter, this, adapter), GetTrackable ());
  }
  catch (...)
  {
    adapters.pop_back ();
    throw;
  }
}

void Driver::UnregisterAdapter (IAdapter* adapter)
{
  adapters.erase (stl::remove (adapters.begin (), adapters.end (), adapter), adapters.end ());
}

/*
    Создание адаптера
*/

IAdapter* Driver::CreateAdapter (const char* name, const char* path)
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
    
    AdapterPtr adapter (PlatformManager::CreateAdapter (name, path), false);
    
      //регистрация адаптера в драйвере

    RegisterAdapter (&*adapter);

      //возвращение адаптера
      
    adapter->AddRef ();

    return &*adapter;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Driver::CreateAdapter");
    throw;
  }
}

/*
    Создание цепочки обмена
*/

ISwapChain* Driver::CreateSwapChain (size_t prefered_adapters_count, IAdapter** prefered_adapters, const SwapChainDesc& swap_chain_desc)
{
  try
  {
    return PlatformManager::CreateSwapChain (prefered_adapters_count, prefered_adapters, swap_chain_desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Driver::CreateSwapChain");
    throw;
  }
}

/*
    Создание устройства отрисовки
*/

IDevice* Driver::CreateDevice (ISwapChain* swap_chain, const char* init_string)
{
  if (!swap_chain)
    throw xtl::make_null_argument_exception ("render::low_level::opengl::Driver::CreateDevice", "swap_chain");
    
  if (!init_string)
    init_string = "";
  
  try
  {
    return new Device (this, swap_chain, init_string);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Driver::CreateDevice");
    throw;
  }
}

/*
    Установка функции отладочного протоколирования драйвера
*/

void Driver::SetDebugLog (const LogFunction& in_log_fn)
{
  log_fn = in_log_fn;
}

const LogFunction& Driver::GetDebugLog ()
{
  return log_fn;
}

/*
    Протоколирование
*/

void Driver::LogMessage (const char* message) const
{
  if (!message)
    return;
    
  try
  {
    log_fn (message);
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Компонент драйвера
*/

namespace
{

class OpenGLDriverComponent
{
  public:
    OpenGLDriverComponent ()
    {
      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<OpenGLDriverComponent> OpenGLDriver (COMPONENT_NAME);

}
