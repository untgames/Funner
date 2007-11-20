#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

const size_t MODE_ARRAY_RESERVE = 32; //резервируемое количество видео-режимов

namespace
{

/*
    ѕолучение информации об видео-режиме
*/

bool get_mode_desc (const char* device_name, int mode_index, OutputModeDesc& mode_desc)
{
  DEVMODE dev_mode_desc;
  
  memset (&dev_mode_desc, 0, sizeof (dev_mode_desc));
  
  dev_mode_desc.dmSize = sizeof (dev_mode_desc);
  
  if (!EnumDisplaySettings (device_name, mode_index, &dev_mode_desc))
    return false;

  mode_desc.width        = dev_mode_desc.dmPelsWidth;
  mode_desc.height       = dev_mode_desc.dmPelsHeight;
  mode_desc.color_bits   = dev_mode_desc.dmBitsPerPel;
  mode_desc.refresh_rate = dev_mode_desc.dmDisplayFrequency;
  
  return true;
}

}

/*
     онструктор / деструктор
*/

Output::Output (const DISPLAY_DEVICE& device_info)
{
    //получение имени устройства

  name     = device_info.DeviceString;
  win_name = device_info.DeviceName;
  
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

    //регистраци€ свойств

  properties.AddProperty ("win-name",     device_info.DeviceName);
  properties.AddProperty ("flags",        flags_string.c_str ());
  properties.AddProperty ("pci-id",       device_info.DeviceID);
  properties.AddProperty ("registry-key", device_info.DeviceKey);
  
    //построение списка видео-режимов
    
  modes.reserve (MODE_ARRAY_RESERVE);

  OutputModeDesc mode_desc;

  for (int mode_index=0; get_mode_desc (win_name.c_str (), mode_index, mode_desc); mode_index++)
    modes.push_back (mode_desc);
}

Output::~Output ()
{
}

/*
    ѕолучение имени / описани€ устройства
*/

const char* Output::GetName ()
{
  return name.c_str ();
}

/*
    ѕолучение списка видео-режимов
*/

size_t Output::GetModesCount ()
{
  return modes.size ();
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  if (mode_index >= modes.size ())
    RaiseOutOfRange ("render::low_level::opengl::Output::GetModeDesc", "mode_index", mode_index, modes.size ());
    
  mode_desc = modes [mode_index];
}

/*
    ”становка текущего видео-режима
*/

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

  LONG result = ChangeDisplaySettings (&dev_mode_desc, CDS_FULLSCREEN);

  if (result != DISP_CHANGE_SUCCESSFUL)
    RaiseInvalidOperation ("render::low_level::opengl::Output::SetCurrentMode",
    "ChangeDisplaySettings fail: name='%s', win-name='%s', mode='%ux%ux%u (%uHz)'",
    name.c_str (), win_name.c_str (), mode_desc.width, mode_desc.height, mode_desc.color_bits, mode_desc.refresh_rate);      
}

void Output::GetCurrentMode (OutputModeDesc& mode_desc)
{
  get_mode_desc (win_name.c_str (), ENUM_CURRENT_SETTINGS, mode_desc);    
}

/*
    ”правление гамма-коррекцией
*/

void Output::SetGammaRamp (const Color3f table [256])
{
  HDC hDC = CreateDC (win_name.c_str (), NULL, NULL, NULL);

  if (!hDC)
    Raise <Exception> ("render::low_level::opengl::Output::GetGammaRamp", "Can't create device context.");

  WORD gamma_ramp_table[256][3];

  for (size_t i = 0; i < 256; i++)
  {
    gamma_ramp_table[i][0] = (WORD)(table[i].red * 65535.f);
    gamma_ramp_table[i][1] = (WORD)(table[i].green * 65535.f);
    gamma_ramp_table[i][2] = (WORD)(table[i].blue * 65535.f);
  }

  if (!SetDeviceGammaRamp (hDC, gamma_ramp_table))
  {
    DeleteDC (hDC);
    Raise <Exception> ("render::low_level::opengl::Output::SetGammaRamp", "Can't set gamma ramp (SetDeviceGammaRamp error).");
  }
  DeleteDC (hDC);
}

void Output::GetGammaRamp (Color3f table [256])
{
  HDC hDC = CreateDC (win_name.c_str (), NULL, NULL, NULL);

  if (!hDC)
    Raise <Exception> ("render::low_level::opengl::Output::GetGammaRamp", "Can't create device context.");

  printf ("HDC = %p\n", hDC);

  WORD gamma_ramp_table[256][3];

  if (!GetDeviceGammaRamp (hDC, gamma_ramp_table))
  {
    DeleteDC (hDC);
    Raise <Exception> ("render::low_level::opengl::Output::GetGammaRamp", "Can't get gamma ramp (GetDeviceGammaRamp error).");
  }
  DeleteDC (hDC);

  for (size_t i = 0; i < 256; i++)
  {
    table[i].red   = (float)gamma_ramp_table[i][0] / 65535.f;
    table[i].green = (float)gamma_ramp_table[i][1] / 65535.f;
    table[i].blue  = (float)gamma_ramp_table[i][2] / 65535.f;
  }
}

/*
    —писок свойств устройства вывода
*/

IPropertyList* Output::GetProperties ()
{
  return &properties;
}
