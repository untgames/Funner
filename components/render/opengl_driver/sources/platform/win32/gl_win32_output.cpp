#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl::windows;

namespace
{

/*
    Константы
*/

const size_t MODE_ARRAY_RESERVE = 32; //резервируемое количество видео-режимов

/*
    Получение информации об видео-режиме
*/

bool get_mode_desc (const char* device_name, int mode_index, OutputModeDesc& mode_desc, Rect* screen_rect=0)
{
  DEVMODE dev_mode_desc;
  
  memset (&dev_mode_desc, 0, sizeof (dev_mode_desc));

  dev_mode_desc.dmSize  = sizeof (dev_mode_desc);

  if (!EnumDisplaySettings (device_name, mode_index, &dev_mode_desc))
    return false;

  mode_desc.width        = dev_mode_desc.dmPelsWidth;
  mode_desc.height       = dev_mode_desc.dmPelsHeight;
  mode_desc.color_bits   = dev_mode_desc.dmBitsPerPel;
  mode_desc.refresh_rate = dev_mode_desc.dmDisplayFrequency;  
  
  if (screen_rect)  
  {
    screen_rect->x      = dev_mode_desc.dmPosition.x;
    screen_rect->y      = dev_mode_desc.dmPosition.y;
    screen_rect->width  = dev_mode_desc.dmPelsWidth;
    screen_rect->height = dev_mode_desc.dmPelsHeight;
  }
  
  return true;
}

}

/*
    Описание реализации устройства вывода
*/

typedef stl::vector<OutputModeDesc> OutputModeArray;

struct Output::Impl
{
  stl::string     name;                      //имя устройства
  stl::string     win_name;                  //имя устройства в Windows
  PropertyList    properties;                //свойства устройства
  OutputModeArray modes;                     //режимы работы устройства
  HDC             hDC;                       //хэндл контекста устройства
  bool            need_restore_default_mode; //необходимо ли восстанавливать настройки при удалении устройства вывода
  
///Конструктор
  Impl () : hDC (0), need_restore_default_mode (false) {}
  
///Деструктор
  ~Impl ()
  {
    if (hDC)
      DeleteDC (hDC);
  }

///Получение дескприптора контекста устройства
  HDC GetDC ()
  {
    if (hDC)
      return hDC;

    hDC = CreateDC (win_name.c_str (), 0, 0, 0);

    if (!hDC)
      raise_error (common::format ("render::low_level::opengl::windows::Output::GetDC(device-name='%s')", name.c_str ()).c_str ());

    return hDC;    
  }
};

/*
    Конструктор / деструктор
*/

Output::Output (const char* device_name, const char* win_name)
  : impl (new Impl)
{
  static const char* METHOD_NAME = "render::low_level::opengl::windows::Output::Output";

    //проверка корректности аргументов

  if (!device_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "device_name");

  if (!win_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "win_name");

    //получение имени устройства

  impl->name     = device_name;
  impl->win_name = win_name;
  
    //отсечение завершающих пробелов в имени устройства

  stl::string::reverse_iterator iter;

  for (iter=impl->name.rbegin (); iter!=impl->name.rend () && *iter == ' '; ++iter);

  impl->name.erase (iter.base (), impl->name.end ());

    //регистрация свойств

  impl->properties.AddProperty ("win-name", impl->win_name.c_str ());
  
    //построение списка видео-режимов
    
  impl->modes.reserve (MODE_ARRAY_RESERVE);

  OutputModeDesc mode_desc;

  for (int mode_index=0; get_mode_desc (impl->win_name.c_str (), mode_index, mode_desc); mode_index++)
    impl->modes.push_back (mode_desc);
}

Output::~Output ()
{
  try
  {
      //восстанавливаем исходный видеорежим    
    
    RestoreDefaultMode ();
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Получение имени / описания устройства
*/

const char* Output::GetName ()
{
  return impl->name.c_str ();
}

const char* Output::GetWinName ()
{
  return impl->win_name.c_str ();
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
    throw xtl::make_range_exception ("render::low_level::opengl::windows::Output::GetModeDesc", "mode_index", mode_index, impl->modes.size ());
    
  mode_desc = impl->modes [mode_index];
}

/*
    Установка текущего видео-режима
*/

namespace
{

void set_mode_desc (const char* device_name, DEVMODE* mode)
{
  LONG result = ChangeDisplaySettingsEx (device_name, mode, 0, 0, 0);

  if (result == DISP_CHANGE_SUCCESSFUL)
    return;

  const char* msg = "";
  
  switch (result)
  {
    case DISP_CHANGE_BADDUALVIEW: msg = "System is DualView capable"; break;
    case DISP_CHANGE_BADFLAGS:    msg = "Invalid set of flags"; break;
    case DISP_CHANGE_BADMODE:     msg = "The graphics mode is not supported"; break;
    case DISP_CHANGE_BADPARAM:    msg = "Invalid parameter"; break;
    case DISP_CHANGE_FAILED:      msg = "The display driver failed the specified graphics mode"; break;
    case DISP_CHANGE_NOTUPDATED:  msg = "Unable to write settings to the registry"; break;
    case DISP_CHANGE_RESTART:     msg = "The computer must be restarted in order for the graphics mode to work"; break;
    default:
      raise_error ("ChangeDisplaySettingsEx");
      break;
  }

  throw xtl::format_operation_exception ("ChangeDisplaySettingsEx", msg);
}

}

void Output::SetCurrentMode (const OutputModeDesc& mode_desc)
{
  DEVMODE dev_mode_desc;

  memset (&dev_mode_desc, 0, sizeof (dev_mode_desc));
  
  dev_mode_desc.dmSize             = sizeof (DEVMODE);
  dev_mode_desc.dmPelsWidth        = mode_desc.width;
  dev_mode_desc.dmPelsHeight       = mode_desc.height;
  dev_mode_desc.dmBitsPerPel       = mode_desc.color_bits;
  dev_mode_desc.dmDisplayFrequency = mode_desc.refresh_rate;
  dev_mode_desc.dmFields           = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | (mode_desc.refresh_rate ? DM_DISPLAYFREQUENCY : 0);

  try
  {
    set_mode_desc (impl->win_name.c_str (), &dev_mode_desc);

    impl->need_restore_default_mode = true;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::Output::SetCurrentMode(device-name='%s', mode='%ux%ux%u @ %uHz')",
      impl->name.c_str (), mode_desc.width, mode_desc.height, mode_desc.color_bits, mode_desc.refresh_rate);
    throw;
  }
}

void Output::RestoreDefaultMode ()
{
  if (!impl->need_restore_default_mode)
    return;

  try
  {
    set_mode_desc (impl->win_name.c_str (), 0);
    
    impl->need_restore_default_mode = false;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::Output::RestoreDefaultMode(device-name='%s')", impl->name.c_str ());
    throw;
  }
}

void Output::GetCurrentMode (OutputModeDesc& mode_desc)
{
  get_mode_desc (impl->win_name.c_str (), ENUM_CURRENT_SETTINGS, mode_desc);
}

/*
    Управление гамма-коррекцией
*/

void Output::SetGammaRamp (const Color3f table [256])
{
  WORD gamma_ramp_table[3][256];

  for (size_t i=0; i<256; i++)
  {
    gamma_ramp_table [0][i] = (WORD)(table [i].red * 65535.f);
    gamma_ramp_table [1][i] = (WORD)(table [i].green * 65535.f);
    gamma_ramp_table [2][i] = (WORD)(table [i].blue * 65535.f);
  }

  if (!SetDeviceGammaRamp (impl->GetDC (), gamma_ramp_table))
    raise_error (common::format ("render::low_level::opengl::windows::Output::SetGammaRamp(device-name='%s')", impl->name.c_str ()).c_str ());
}

void Output::GetGammaRamp (Color3f table [256])
{
  WORD gamma_ramp_table [3][256];

  if (!GetDeviceGammaRamp (impl->GetDC (), gamma_ramp_table))
    raise_error (common::format ("render::low_level::opengl::windows::Output::GetGammaRamp(device-name='%s')", impl->name.c_str ()).c_str ());

  for (size_t i=0; i<256; i++)
  {
    table [i].red   = (float)gamma_ramp_table[0][i] / 65535.f;
    table [i].green = (float)gamma_ramp_table[1][i] / 65535.f;
    table [i].blue  = (float)gamma_ramp_table[2][i] / 65535.f;
  }
}

/*
    Получение экранных координат и размеров устройства вывода
*/

void Output::GetScreenRect (Rect& rect)
{
  OutputModeDesc mode_desc;

  get_mode_desc (impl->win_name.c_str (), ENUM_CURRENT_SETTINGS, mode_desc, &rect);
}

/*
    Список свойств устройства вывода
*/

IPropertyList* Output::GetProperties ()
{
  return &impl->properties;
}
