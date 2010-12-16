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
typedef stl::vector<OutputModeDesc> OutputModeArray;

struct Output::Impl
{
  Log               log;                         //протокол графического драйвера
  OutputModeArray   modes;                       //режимы работы устройства
  Display*          display;                     //соединение с дисплеем
  int               screen_number;               //номер экрана дисплея
  char              name [OUTPUT_MAX_NAME_SIZE]; //имя цепочки обмена
  
///Конструктор
  Impl (Display* in_display, const int in_screen_number)
    : display (in_display)
    , screen_number (in_screen_number)
  {
    DisplayLock lock (display);

    *name = 0;
    
    int sizes_count = 0;
    int rates_count = 0;
    
    XRRScreenSize *sizes = XRRSizes (display, screen_number, &sizes_count);
    short         *rates = 0;
    
    for (int sizeID=0; sizeID < sizes_count; sizeID++)
    {
      rates = XRRRates (display, screen_number, sizeID, &rates_count);
      
      for (int rateID=0; rateID < rates_count; rateID++)
      {
        OutputModeDesc mode_desc;
        
        mode_desc.width        = sizes [sizeID].width;
        mode_desc.height       = sizes [sizeID].height;
        mode_desc.color_bits   = 0;
        mode_desc.refresh_rate = rates [rateID];
        
        modes.push_back (mode_desc);
      }
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

/*
    Конструктор / деструктор
*/

Output::Output (Display* display, const int output_number)
{
  try
  {
    if (!display)
      throw xtl::make_null_argument_exception ("", "display");
      
    if (output_number < 0)
      throw xtl::make_null_argument_exception ("", "output_number");

    impl = new Impl (display, output_number);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::Output::Output");
    throw;
  }
}

Output::~Output ()
{

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
  return impl->modes.size ();
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  if (mode_index >= impl->modes.size ())
    throw xtl::make_range_exception ("render::low_level::opengl::glx::Output::GetModeDesc", "mode_index", mode_index, impl->modes.size ());
    
  mode_desc = impl->modes [mode_index];
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
