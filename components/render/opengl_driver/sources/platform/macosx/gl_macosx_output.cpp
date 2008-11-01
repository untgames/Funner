#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::macosx;

namespace
{

struct DisplayModeEntry
{
  CFDictionaryRef mode_dictionary;
  OutputModeDesc  mode_desc;
};

bool get_output_mode_desc (const CFDictionaryRef& mode_dictionary, OutputModeDesc& output_mode_desc)
{
  if (!CFNumberGetValue ((CFNumberRef)CFDictionaryGetValue (mode_dictionary, kCGDisplayWidth),
                         kCFNumberIntType, &output_mode_desc.width))
    return false;
  if (!CFNumberGetValue ((CFNumberRef)CFDictionaryGetValue (mode_dictionary, kCGDisplayHeight),
                         kCFNumberIntType, &output_mode_desc.height))
    return false;
  if (!CFNumberGetValue ((CFNumberRef)CFDictionaryGetValue (mode_dictionary, kCGDisplayBitsPerPixel),
                         kCFNumberIntType, &output_mode_desc.color_bits))
    return false;
  return CFNumberGetValue ((CFNumberRef)CFDictionaryGetValue (mode_dictionary, kCGDisplayRefreshRate),
                           kCFNumberIntType, &output_mode_desc.refresh_rate);
}

}

/*
    Описание реализации устройства вывода
*/

struct Output::Impl
{
  typedef stl::vector<DisplayModeEntry> DisplayModeArray;

  CGDirectDisplayID display_id;      //системный идентификатор устройства
  stl::string       display_name;    //имя устройства
  DisplayModeArray  display_modes;   //поддерживаемые режимы

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

      display_name = common::tostring (display_name_buffer, getted_chars);

      CFRelease (dictionary);
    }

      //получение поддерживаемых режимов

    CFArrayRef cg_display_modes = CGDisplayAvailableModes (display_id);

    if (cg_display_modes)
    {
      CFIndex modes_count = CFArrayGetCount (cg_display_modes);

      display_modes.reserve (modes_count);

      for (int i = 0; i < modes_count; i++)
      {
        DisplayModeEntry mode_entry;

        mode_entry.mode_dictionary = (CFDictionaryRef)CFArrayGetValueAtIndex (cg_display_modes, i);

        memset (&mode_entry.mode_desc, 0, sizeof (mode_entry.mode_desc));

        if (!get_output_mode_desc (mode_entry.mode_dictionary, mode_entry.mode_desc))
          continue;

        display_modes.push_back (mode_entry);
      }
    }
  }
};

/*
    Конструктор / деструктор
*/

Output::Output (CGDirectDisplayID display_id)
  : impl (new Impl (display_id))
{
}

Output::~Output ()
{
}

/*
   Получение имени устройства
*/

const char* Output::GetName ()
{
  return impl->display_name.c_str ();
}

/*
   Получение списка видео-режимов
*/

size_t Output::GetModesCount ()
{
  return impl->display_modes.size ();
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  if (mode_index >= GetModesCount ())
    throw xtl::make_range_exception ("render::low_level::opengl::macosx::Output::GetModeDesc", "mode_index", mode_index, GetModesCount ());

  mode_desc = impl->display_modes[mode_index].mode_desc;
}

/*
   Установка текущего видео-режима
*/

void Output::SetCurrentMode (const OutputModeDesc& new_mode)
{
  Impl::DisplayModeArray::iterator mode_iter = impl->display_modes.end ();

  for (Impl::DisplayModeArray::iterator iter = impl->display_modes.begin (), end = impl->display_modes.end (); iter != end; ++iter)
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

      if ((mode_iter == impl->display_modes.end ()) || (mode_iter->mode_desc.refresh_rate < iter->mode_desc.refresh_rate))
        mode_iter = iter;
    }
    else                      //поиск подходящего по остальным параметрам режима с максимальной цветностью
    {
      if (iter->mode_desc.refresh_rate != new_mode.refresh_rate)
        continue;

      if ((mode_iter == impl->display_modes.end ()) || (mode_iter->mode_desc.color_bits < iter->mode_desc.color_bits))
        mode_iter = iter;
    }
  }

  try
  {
    if (mode_iter == impl->display_modes.end ())
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

void Output::GetCurrentMode (OutputModeDesc& current_mode)
{
  static const char* METHOD_NAME = "render::low_level::opengl::macosx::Output::GetCurrentMode";

  CFDictionaryRef cf_current_mode = CGDisplayCurrentMode (impl->display_id);

  if (!cf_current_mode)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get current mode, unknown exception");

  OutputModeDesc temp_mode_desc;

  if (!get_output_mode_desc (cf_current_mode, temp_mode_desc))
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get current mode, unknown exception");

  current_mode = temp_mode_desc;
}

/*
   Управление гамма-коррекцией
*/

void Output::SetGammaRamp (const Color3f table [256])
{
  CGGammaValue red_table[256];
  CGGammaValue green_table[256];
  CGGammaValue blue_table[256];

  for (size_t i = 0; i < 256; i++)
  {
    red_table[i]   = table[i].red;
    green_table[i] = table[i].green;
    blue_table[i]  = table[i].blue;
  }

  check_quartz_error (CGSetDisplayTransferByTable (impl->display_id, 256, red_table, green_table, blue_table),
                      "render::low_level::opengl::macosx::Output::SetGammaRamp", "Can't set gamma ramp, ::CGSetDisplayTransferByTable error");
}

void Output::GetGammaRamp (Color3f table [256])
{
  static const char* METHOD_NAME = "render::low_level::opengl::macosx::Output::GetGammaRamp";

  CGGammaValue red_table[256];
  CGGammaValue green_table[256];
  CGGammaValue blue_table[256];

  CGTableCount copied_count;

  check_quartz_error (CGGetDisplayTransferByTable (impl->display_id, 256, red_table, green_table, blue_table, &copied_count),
                      METHOD_NAME, "Can't get gamma ramp, ::CGGetDisplayTransferByTable error");

  if (copied_count != 256)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get 256 table size gamma ramp");

  for (size_t i = 0; i < 256; i++)
  {
    table[i].red   = red_table[i];
    table[i].green = green_table[i];
    table[i].blue  = blue_table[i];
  }
}

/*
   Возвращение внутреннего дескриптора дисплея
*/

CGDirectDisplayID Output::GetDisplayID ()
{
  return impl->display_id;
}

