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

bool get_screen_mode_desc (const CGDisplayModeRef& mode, ScreenModeDesc& mode_desc)
{
  mode_desc.width        = CGDisplayModeGetWidth (mode);
  mode_desc.height       = CGDisplayModeGetHeight (mode);
  mode_desc.refresh_rate = (size_t)CGDisplayModeGetRefreshRate (mode);
  mode_desc.color_bits   = 0;

  CFStringRef pixel_encoding = CGDisplayModeCopyPixelEncoding (mode);

  if (!pixel_encoding)
    return false;

  if (CFStringCompare (pixel_encoding, CFSTR (IO32BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
    mode_desc.color_bits = 32;
  else if (CFStringCompare (pixel_encoding, CFSTR (IO16BitDirectPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
    mode_desc.color_bits = 16;
  else if (CFStringCompare (pixel_encoding, CFSTR (IO8BitIndexedPixels), kCFCompareCaseInsensitive) == kCFCompareEqualTo)
    mode_desc.color_bits = 8;
  else
    return false;

  CFRelease (pixel_encoding);

  return true;
}

typedef stl::vector<ScreenModeEntry> ScreenModeArray;

const char* get_gestalt_manager_error_name (OSErr error)
{
  switch (error)
  {
    case gestaltUndefSelectorErr: return "Undefined selector was passed to the Gestalt Manager";
    case gestaltDupSelectorErr:   return "Tried to add an entry that already existed";
    case gestaltLocationErr:      return "Gestalt function ptr was not in the system heap";
    default:                      return "Unknown error";
  }
}

void check_gestalt_manager_error (OSErr error_code, const char* source, const char* message)
{
  if (error_code != noErr)
    throw xtl::format_operation_exception (source, "%s. Gestalt error: %s (code %d)", message,
                                           get_gestalt_manager_error_name (error_code), error_code);
}

}

/*
   Устройство вывода
*/

struct CarbonScreen::Impl : public xtl::reference_counter
{
  CGDirectDisplayID display_id;
  stl::string       name;
  ScreenModeArray   modes;        //поддерживаемые режимы
  ScreenModeDesc    default_mode; //режим по умолчанию

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
    static const char* METHOD_NAME = "syslib::macosx::CarbonScreen::GetCurrentMode";

    SInt32 os_version_major, os_version_minor;

    try
    {
      check_gestalt_manager_error (Gestalt (gestaltSystemVersionMajor, &os_version_major), "::Gestalt", "Can't get OS major version");
      check_gestalt_manager_error (Gestalt (gestaltSystemVersionMinor, &os_version_minor), "::Gestalt", "Can't get OS minor version");
    }
    catch (xtl::exception& e)
    {
      e.touch (METHOD_NAME);
      throw;
    }

    if ((os_version_major == 10 && os_version_minor > 5) || os_version_major > 10)
    {
      CGDisplayModeRef cg_current_mode = CGDisplayCopyDisplayMode (display_id);

      if (!cg_current_mode)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't get current mode, unknown exception");

      if (!get_screen_mode_desc (cg_current_mode, current_mode))
        throw xtl::format_operation_exception (METHOD_NAME, "Can't get current mode, unknown exception");

      CGDisplayModeRelease (cg_current_mode);
    }
    else
    {
      CFDictionaryRef cf_current_mode = CGDisplayCurrentMode (display_id);

      if (!cf_current_mode)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't get current mode, unknown exception");

      if (!get_screen_mode_desc (cf_current_mode, current_mode))
        throw xtl::format_operation_exception (METHOD_NAME, "Can't get current mode, unknown exception");
    }
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
  mode_desc = impl->default_mode;
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
