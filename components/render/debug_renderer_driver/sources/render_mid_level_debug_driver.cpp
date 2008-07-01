#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

namespace
{

/*
    Константы
*/

const char* DRIVER_NAME           = "debug";                  //имя драйвера
const char* DRIVER_COMPONENT_NAME = "render.mid_level.debug"; //имя компонента драйвера
const char* RENDERER2D_NAME       = "renderer2d";             //имя системы двумерного рендеринга

}

/*
    Конструктор / деструктор
*/

Driver::Driver ()
{
  log.Printf ("Create debug driver");
}

Driver::~Driver ()
{
  try
  {
    log.Printf ("Destroy debug driver");
  }
  catch (...)
  {
    //подавление исключений
  }
}

/*
    Описание драйвера
*/

const char* Driver::GetDescription ()
{
  return "render::mid_level::debug::Driver";
}

/*
    Перечисление доступных систем визуализации
*/

size_t Driver::GetRenderersCount ()
{
  return 1;
}

const char* Driver::GetRendererName (size_t index)
{
  if (index)
    throw xtl::make_range_exception ("render::mid_level::debug::Driver::GetRendererName", "index", index, 1u);
    
  return RENDERER2D_NAME;
}

/*
    Создание устройства визуализации
*/

IRenderer* Driver::CreateRenderer (const char* name)
{
  static const char* METHOD_NAME = "render::mid_level::debug::Driver::CreateRenderer";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  if (xtl::xstrcmp (name, RENDERER2D_NAME))
    throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Unknown renderer");
    
  return new Renderer2d;
}

namespace
{

/*
    Компонент отладочного драйвера визуализации
*/

class DebugDriverComponent
{
  public:   
    DebugDriverComponent ()
    {
      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<DebugDriverComponent> DebugRendererDriver (DRIVER_COMPONENT_NAME);

}
