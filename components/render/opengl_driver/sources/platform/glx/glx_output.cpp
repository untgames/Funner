#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
    Константы
*/

const size_t OUTPUT_MAX_NAME_SIZE = 128; //максимальный размер имени устройства вывода

}

/*
    Описание реализации устройства вывода
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;

struct Output::Impl
{
  Log               log;                         //протокол графического драйвера
  AdapterPtr        adapter;                     //адаптер отрисовки
  char              name [OUTPUT_MAX_NAME_SIZE]; //имя цепочки обмена
  
///Конструктор
  Impl (Adapter* in_adapter, const void* window_handle)
    : adapter (in_adapter)
  {
    *name = 0;

    try
    {

        //создание дисплея

      log.Printf ("...create display");
    }
    catch (...)
    {
      throw;
    }
  }
  
///Деструктор
  ~Impl ()
  {
    try
    {

    }
    catch (...)
    {
    }
  }  
};

/*
    Конструктор / деструктор
*/

Output::Output (Adapter* adapter, const void* window_handle)
{
  try
  {
    if (!window_handle)
      throw xtl::make_null_argument_exception ("", "window_handle");
      
    if (!adapter)
      throw xtl::make_null_argument_exception ("", "adapter");

    impl = new Impl (adapter, window_handle);
    
    impl->adapter->RegisterOutput (this);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Output::Output");
    throw;
  }
}

Output::~Output ()
{
  impl->adapter->UnregisterOutput (this);
}

/*
    Получение имени
*/

const char* Output::GetName ()
{
  return impl->name;
}

/*
    Получение списка видео-режимов
*/

size_t Output::GetModesCount ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Output::GetModesCount");
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Output::GetModeDesc");
}

/*
    Установка текущего видео-режима
*/

void Output::SetCurrentMode (const OutputModeDesc&)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Output::SetCurrentMode");
}

void Output::GetCurrentMode (OutputModeDesc&)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Output::GetCurrentMode");
}

/*
    Управление гамма-коррекцией
*/

void Output::SetGammaRamp (const Color3f [256])
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::glx::Output::SetGammaRamp", "Gamma ramp not supported in EGL");
}

void Output::GetGammaRamp (Color3f [256])
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::glx::Output::GetGammaRamp", "Gamma ramp not supported in EGL");
}
