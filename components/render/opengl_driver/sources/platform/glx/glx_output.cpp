#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

namespace
{

/*
    Константы
*/

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
  
///Конструктор
  Impl (Display* in_display, size_t in_screen_number)
    : display (in_display)
    , screen_number (in_screen_number)
  {
    DisplayLock lock (display);
    
    int event_base = 0, error_base = 0;

    if (!XRRQueryExtension (display, &event_base, &error_base))
      throw xtl::format_operation_exception ("render::low_level::opengl::glx::Output::Impl::Impl", "RandR extension missing");

    int sizes_count = 0;
    XRRScreenSize *sizes = XRRSizes (display, screen_number, &sizes_count);
    
    for (int sizeID=0; sizeID < sizes_count; sizeID++)
    {
      int rates_count = 0;
      short *rates = XRRRates (display, screen_number, sizeID, &rates_count);
      
      int depths_count = 0;
      XVisualInfo *depths = XGetVisualInfo (display, VisualDepthMask, 0, &depths_count);
      
      for (int i=0; i<depths_count; i++)
        printf ("depth = %d", depths[i].depth);
        
      XFree (depths);
      
     
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
};

/*
    Конструктор / деструктор
*/

Output::Output (Display* display, size_t screen_number)
{
  try
  {
    if (!display)
      throw xtl::make_null_argument_exception ("", "display");
      
    if (screen_number < 0)
      throw xtl::make_null_argument_exception ("", "screen_number");

    impl = new Impl (display, screen_number);
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
  stl::string name = common::format ("Screen#%02u", impl->screen_number);
  return name.c_str ();
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
