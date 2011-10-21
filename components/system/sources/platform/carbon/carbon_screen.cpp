#include "shared.h"

using namespace syslib;
using namespace syslib::macosx;

namespace
{

struct ScreenModeEntry
{
  CFDictionaryRef mode_dictionary;
  ScreenModeDesc  mode_desc;
};

bool get_screen_mode_desc (const CFDictionaryRef& mode_dictionary, ScreenModeDesc& mode_desc)
{
  if (!CFNumberGetValue ((CFNumberRef)CFDictionaryGetValue (mode_dictionary, kCGDisplayWidth),
                         kCFNumberIntType, &mode_desc.width))
    return false;
  if (!CFNumberGetValue ((CFNumberRef)CFDictionaryGetValue (mode_dictionary, kCGDisplayHeight),
                         kCFNumberIntType, &mode_desc.height))
    return false;
  if (!CFNumberGetValue ((CFNumberRef)CFDictionaryGetValue (mode_dictionary, kCGDisplayBitsPerPixel),
                         kCFNumberIntType, &mode_desc.color_bits))
    return false;

  return CFNumberGetValue ((CFNumberRef)CFDictionaryGetValue (mode_dictionary, kCGDisplayRefreshRate),
                           kCFNumberIntType, &mode_desc.refresh_rate);
}

typedef stl::vector<ScreenModeEntry> ScreenModeArray;

}

/*
   Устройство вывода
*/

struct CarbonScreen::Impl : public xtl::reference_counter
{
  CGDirectDisplayID display_id;
  stl::string       name;
  ScreenModeArray   modes;        //поддерживаемые режимы
  ScreenModeEntry   default_mode; //режим по умолчанию

  Impl (CGDirectDisplayID in_display_id)
    : display_id (in_display_id)
  {
      //получение имени устройства

    io_connect_t display_port = CGDisplayIOServicePort (display_id);

    if (display_port != MACH_PORT_NULL)
    {
      CFDictionaryRef dictionary = IODisplayCreateInfoDictionary (display_port, 0);
      CFDictionaryRef names      = (CFDictionaryRef)CFDictionaryGetValue (dictionary, CFSTR(kDisplayProductName));

      CFIndex names_count = CFDictionaryGetCount (names);

      xtl::uninitialized_storage<void*> keys (names_count);

      CFDictionaryGetKeysAndValues (names, (const void**)keys.data (), 0);

      CFStringRef cf_display_name = (CFStringRef)CFDictionaryGetValue (names, *keys.data ());

      CFRange cf_display_name_range = { 0, CFStringGetLength (cf_display_name) };

      wchar_t display_name_buffer [256];

      CFIndex getted_chars = CFStringGetBytes (cf_display_name, cf_display_name_range, kCFStringEncodingUTF32, '?', false,
          (UInt8*)display_name_buffer, 255 * sizeof (wchar_t), 0);

      name = common::tostring (display_name_buffer, getted_chars);

      CFRelease (dictionary);
    }

      //получение поддерживаемых режимов

    CFArrayRef cg_display_modes = CGDisplayAvailableModes (display_id);

    if (cg_display_modes)
    {
      CFIndex modes_count = CFArrayGetCount (cg_display_modes);

      modes.reserve (modes_count);

      for (int i = 0; i < modes_count; i++)
      {
        ScreenModeEntry mode_entry;

        mode_entry.mode_dictionary = (CFDictionaryRef)CFArrayGetValueAtIndex (cg_display_modes, i);

        memset (&mode_entry.mode_desc, 0, sizeof (mode_entry.mode_desc));

        if (!get_screen_mode_desc (mode_entry.mode_dictionary, mode_entry.mode_desc))
          continue;

        modes.push_back (mode_entry);
      }
    }

    GetCurrentMode (default_mode);
  }

  void GetCurrentMode (ScreenModeDesc& current_mode)
  {
    ScreenModeEntry mode_entry;

    GetCurrentMode (mode_entry);

    current_mode = mode_entry.mode_desc;
  }

  void GetCurrentMode (ScreenModeEntry& current_mode)
  {
    static const char* METHOD_NAME = "syslib::macosx::CarbonWindow::GetCurrentMode";

    CFDictionaryRef cf_current_mode = CGDisplayCurrentMode (display_id);

    if (!cf_current_mode)
      throw xtl::format_operation_exception (METHOD_NAME, "Can't get current mode, unknown exception");

    ScreenModeDesc temp_mode_desc;

    if (!get_screen_mode_desc (cf_current_mode, temp_mode_desc))
      throw xtl::format_operation_exception (METHOD_NAME, "Can't get current mode, unknown exception");

    current_mode.mode_desc       = temp_mode_desc;
    current_mode.mode_dictionary = cf_current_mode;
  }
};

/*
   Конструктор / деструктор / копирование
*/

CarbonScreen::CarbonScreen (CGDirectDisplayID display_id)
  : impl (new Impl (display_id))
  {}

CarbonScreen::CarbonScreen (const CarbonScreen& source)
  : impl (source.impl)
{
  addref (impl);
}

CarbonScreen::~CarbonScreen ()
{
  release (impl);
}

CarbonScreen& CarbonScreen::operator = (const CarbonScreen& source)
{
  CarbonScreen (source).Swap (*this);

  return *this;
}

/*
   Получение имени устройства
*/

const char* CarbonScreen::GetName ()
{
  return impl->name.c_str ();
}

/*
   Получение списка видео-режимов
*/

size_t CarbonScreen::GetModesCount ()
{
  return impl->modes.size ();
}

void CarbonScreen::GetMode (size_t mode_index, ScreenModeDesc& mode_desc)
{
  if (mode_index >= impl->modes.size ())
    throw xtl::make_range_exception ("syslib::CarbonScreen::GetMode", "mode_index", mode_index, 0u, impl->modes.size ());

  mode_desc = impl->modes [mode_index].mode_desc;
}

/*
   Установка текущего видео-режима
*/

void CarbonScreen::SetCurrentMode (const ScreenModeDesc& new_mode)
{
  ScreenModeArray::iterator mode_iter = impl->modes.end ();

  for (ScreenModeArray::iterator iter = impl->modes.begin (), end = impl->modes.end (); iter != end; ++iter)
  {
    if (iter->mode_desc.width != new_mode.width || iter->mode_desc.height != new_mode.height)
      continue;

    if (new_mode.color_bits && new_mode.refresh_rate)
      if (iter->mode_desc.color_bits != new_mode.color_bits || iter->mode_desc.refresh_rate != new_mode.refresh_rate)
        continue;

    if (new_mode.color_bits)  //поиск подходящего по остальным параметрам режима с максимальной частотой обновления
    {
      if (iter->mode_desc.color_bits != new_mode.color_bits)
        continue;

      if ((mode_iter == impl->modes.end ()) || (mode_iter->mode_desc.refresh_rate < iter->mode_desc.refresh_rate))
        mode_iter = iter;
    }
    else                      //поиск подходящего по остальным параметрам режима с максимальной цветностью
    {
      if (iter->mode_desc.refresh_rate != new_mode.refresh_rate)
        continue;

      if ((mode_iter == impl->modes.end ()) || (mode_iter->mode_desc.color_bits < iter->mode_desc.color_bits))
        mode_iter = iter;
    }
  }

  try
  {
    if (mode_iter == impl->modes.end ())
      throw xtl::format_operation_exception ("", "Can't set output mode, you must use only modes, received by Output::GetModeDesc");

    check_quartz_error (CGDisplaySwitchToMode (impl->display_id, mode_iter->mode_dictionary),
                        "::CGDisplaySwitchToMode", "Can't change mode");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::Output::SetCurrentMode");
    throw;
  }
}

void CarbonScreen::GetCurrentMode (ScreenModeDesc& mode_desc)
{
  impl->GetCurrentMode (mode_desc);
}

void CarbonScreen::GetDefaultMode (ScreenModeDesc& mode_desc)
{
  mode_desc = impl->default_mode.mode_desc;
}

/*
   Возвращение внутреннего дескриптора дисплея
*/

CGDirectDisplayID CarbonScreen::GetDisplayID ()
{
  return impl->display_id;
}

/*
   Обмен
*/

void CarbonScreen::Swap (CarbonScreen& screen)
{
  stl::swap (impl, screen.impl);
}

namespace syslib
{

namespace macosx
{

//обмен
void swap (CarbonScreen& screen1, CarbonScreen& screen2)
{
  screen1.Swap (screen2);
}

}

}
