#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

Output::Output (const DISPLAY_DEVICE& device_info)
{
    //получение имени устройства

  name = device_info.DeviceString;
  
    //отсечение завершающих пробелов в имени устройства

  stl::string::reverse_iterator iter;

  for (iter=name.rbegin (); iter!=name.rend () && *iter == ' '; ++iter);

  name.erase (iter.base (), name.end ());
  
    //формирование строки флагов устройства
    
  stl::string flags_string;
  
  struct Tag2Flag
  {
    const char* tag;
    size_t      flag;
  };
  
  static const Tag2Flag flags [] = {
    {"ATTACHED_TO_DESKTOP", DISPLAY_DEVICE_ATTACHED_TO_DESKTOP},
    {"MIRRORING_DRIVER",    DISPLAY_DEVICE_MIRRORING_DRIVER},
    {"MODESPRUNED",         DISPLAY_DEVICE_MODESPRUNED},
    {"PRIMARY_DEVICE",      DISPLAY_DEVICE_PRIMARY_DEVICE},
    {"REMOVABLE",           DISPLAY_DEVICE_REMOVABLE},
    {"VGA_COMPATIBLE",      DISPLAY_DEVICE_VGA_COMPATIBLE},
    {0, 0}
  };
  
  for (const Tag2Flag* i=flags; i->tag; ++i)
    if (i->flag & device_info.StateFlags)
    {
      if (!flags_string.empty ())
        flags_string += " | ";
        
      flags_string += i->tag;
    }

    //регистрация свойств

  properties.AddProperty ("win-name",     device_info.DeviceName);
  properties.AddProperty ("flags",        flags_string.c_str ());
  properties.AddProperty ("pci-id",       device_info.DeviceID);
  properties.AddProperty ("registry-key", device_info.DeviceKey);
}

Output::~Output ()
{
}

/*
    Получение имени / описания устройства
*/

const char* Output::GetName ()
{
  return name.c_str ();
}

/*
    Получение списка видео-режимов
*/

size_t Output::GetModesCount ()
{
  return 0;
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  RaiseNotImplemented ("render::low_level::opengl::Output::GetModeDesc");
}

/*
    Установка текущего видео-режима
*/

void Output::SetCurrentMode (const OutputModeDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Output::SetCurrentMode");
}

void Output::GetCurrentMode (OutputModeDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Output::GetCurrentMode");
}

/*
    Управление гамма-коррекцией
*/

void Output::SetGammaRamp (const Color3f table [256])
{
  RaiseNotImplemented ("render::low_level::opengl::Output::SetGammaRamp");
}

void Output::GetGammaRamp (Color3f table [256])
{
  RaiseNotImplemented ("render::low_level::opengl::Output::GetGammaRamp");
}

/*
    Список свойств устройства вывода
*/

IPropertyList* Output::GetProperties ()
{
  return &properties;
}
