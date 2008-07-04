#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::low_level_driver;

/*
    Константы
*/

namespace
{

const char* DRIVER_NAME = "LowLevel"; //имя драйвера

}

/*
    Вхождение системы рендеринга в список драйвера
*/

struct Driver::RendererEntry
{
  stl::string                                 renderer_name;
  xtl::com_ptr<render::low_level::IDevice>    device;
  xtl::com_ptr<render::low_level::ISwapChain> swap_chain;
  xtl::com_ptr<BasicRenderer>                 renderer;

  RendererEntry (const char* in_renderer_name, render::low_level::IDevice* in_device, render::low_level::ISwapChain* in_swap_chain)
    : renderer_name (in_renderer_name),
      device (in_device),
      swap_chain (in_swap_chain),
      renderer (new renderer2d::Renderer (in_device, in_swap_chain))
    {}
};

/*
    Конструктор / деструктор
*/

Driver::Driver ()
{
}

Driver::~Driver ()
{
  UnregisterDriver ();
}

/*
    Описание драйвера
*/

const char* Driver::GetDescription ()
{
  return "render::mid_level::low_level_driver::Driver";
}

/*
    Перечисление доступных систем визуализации
*/

size_t Driver::GetRenderersCount ()
{
  return renderer_entries.size ();
}

const char* Driver::GetRendererName (size_t index)
{
  if (index >= GetRenderersCount ())
    throw xtl::make_range_exception ("render::mid_level::low_level_driver::Driver::GetRendererName", "index", index, 0u, GetRenderersCount ());

  return renderer_entries[index]->renderer_name.c_str ();
}

/*
    Установка области вывода для системы рендеринга
*/

void Driver::SetViewport (const char* name, const render::low_level::Rect& viewport)
{
  try
  {
    if (!name)
      throw xtl::make_null_argument_exception ("", "name");
      
    for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
      if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
      {
        (*iter)->renderer->SetViewport (viewport);
        return;
      }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::low_level_driver::Driver::SetViewport");
    throw;
  }
}

/*
    Создание устройства визуализации
*/

IRenderer* Driver::CreateRenderer (const char* name)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::Driver::CreateRenderer";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
      return (*iter)->renderer.get ();

  throw xtl::make_argument_exception (METHOD_NAME, "name", name);
}

/*
   Регистрация систем рендернинга
*/

void Driver::RegisterRenderer (const char* name, render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::Driver::RegisterRenderer";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");

  if (!device)
    throw xtl::make_null_argument_exception (METHOD_NAME, "device");

  if (!swap_chain)
    throw xtl::make_null_argument_exception (METHOD_NAME, "swap_chain");

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
      throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Name already registered");

  renderer_entries.push_back (RendererEntryPtr (new RendererEntry (name, device, swap_chain)));

  if (renderer_entries.size () == 1)
    DriverManager::RegisterDriver (DRIVER_NAME, this);
}

void Driver::UnregisterRenderer (const char* name)
{
  if (!name)
    return;

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
    {
      renderer_entries.erase (iter);

      if (renderer_entries.empty ())
        UnregisterDriver ();

      return;
    }
}

void Driver::UnregisterAllRenderers ()
{
  renderer_entries.clear ();
  UnregisterDriver ();
}

void Driver::UnregisterDriver ()
{
  DriverManager::UnregisterDriver (DRIVER_NAME);
}

namespace
{

/*
   Синглтон драйвера низкоуровневой системы рендернинга
*/

typedef common::Singleton<Driver> LowLevelDriverSingleton;

}

/*
   Драйвер низкоуровневой системы рендернинга
*/

/*
   Имя драйвера
*/

const char* LowLevelDriver::Name ()
{
  return DRIVER_NAME;
}
    
/*
   Получение драйвера
*/

IDriver* LowLevelDriver::Driver ()
{
  return LowLevelDriverSingleton::InstancePtr ();
}

/*
    Установка области вывода для системы рендеринга
*/

void LowLevelDriver::SetViewport (const char* name, const render::low_level::Rect& viewport)
{
  LowLevelDriverSingleton::Instance ().SetViewport (name, viewport);
}
    
/*
   Регистрация систем рендернинга
*/

void LowLevelDriver::RegisterRenderer (const char* name, render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain)
{
  LowLevelDriverSingleton::Instance ().RegisterRenderer (name, device, swap_chain);
}

void LowLevelDriver::UnregisterRenderer (const char* name)
{
  LowLevelDriverSingleton::Instance ().UnregisterRenderer (name);
}

void LowLevelDriver::UnregisterAllRenderers ()
{
  LowLevelDriverSingleton::Instance ().UnregisterAllRenderers ();
}
