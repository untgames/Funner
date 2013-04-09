#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Константы
*/

const size_t MODES_RESERVE_SIZE = 256;

/*
    Конструктор / деструктор
*/

Output::Output (const DxOutputPtr& in_output)
{
  try
  {
      //проверка корректности аргументов

    if (!in_output)
      throw xtl::make_null_argument_exception ("", "output");

    output = in_output;

    DXGI_OUTPUT_DESC desc;

    check_errors ("IDXGIOutput::GetDesc", output->GetDesc (&desc));

      //сохранение параметров
      
    name = common::tostring (desc.DeviceName);
    
    properties.AddProperty ("AttachedToDesktop", desc.AttachedToDesktop ? "true" : "false");

    const char* rotation_string = "";

    switch (desc.Rotation)
    {
      default:
      case DXGI_MODE_ROTATION_UNSPECIFIED: rotation_string = "unspecified"; break;
      case DXGI_MODE_ROTATION_IDENTITY:    rotation_string = "identity"; break;
      case DXGI_MODE_ROTATION_ROTATE90:    rotation_string = "rotate90"; break;
      case DXGI_MODE_ROTATION_ROTATE180:   rotation_string = "rotate180"; break;
      case DXGI_MODE_ROTATION_ROTATE270:   rotation_string = "rotate270"; break;
    }

    properties.AddProperty ("Rotation", rotation_string);
    properties.AddProperty ("Monitor", common::format ("%08x", desc.Monitor).c_str ());

      //перечисление режимов

    modes.reserve (MODES_RESERVE_SIZE);

    struct Format2ColorBits
    {
      DXGI_FORMAT format;
      size_t      color_bits;
    };

    static const Format2ColorBits formats [] = {
      {DXGI_FORMAT_R8G8B8A8_UNORM, 32},
    };

    static const size_t formats_count = sizeof (formats) / sizeof (*formats);

    for (size_t i=0; i<formats_count; i++)
    {
      DXGI_FORMAT format     = formats [i].format;
      UINT        flags      = 0, num_modes = 0;
      size_t      color_bits = formats [i].color_bits;

      check_errors ("IDXGIOutput::GetDisplayModeList[1]", output->GetDisplayModeList (format, flags, &num_modes, 0));
     
      if (num_modes)
      {
        typedef stl::vector<DXGI_MODE_DESC> ModeDescArray;

        ModeDescArray descs (num_modes);

        check_errors ("IDXGIOutput::GetDisplayModeList[2]", output->GetDisplayModeList (format, flags, &num_modes, &descs [0]));

        modes.reserve (modes.size () + num_modes);

        for (ModeDescArray::const_iterator iter=descs.begin (), end=descs.end (); iter!=end; ++iter)
        {
          const DXGI_MODE_DESC& src_desc = *iter;

          OutputModeDesc dst_desc;

          memset (&dst_desc, 0, sizeof (dst_desc));

          dst_desc.width        = src_desc.Width;
          dst_desc.height       = src_desc.Height;
          dst_desc.color_bits   = color_bits;
          dst_desc.refresh_rate = (size_t)ceil ((double)src_desc.RefreshRate.Numerator / src_desc.RefreshRate.Denominator);

          modes.push_back (dst_desc);
        }
      }
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Output::Output");
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
  try
  {
    return name.c_str ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Output::GetName");
    throw;
  }
}

/*
    Получение списка видео-режимов
*/

size_t Output::GetModesCount ()
{
  try
  {
    return modes.size ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Output::GetModesCount");
    throw;
  }
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  try
  {
    if (mode_index >= modes.size ())
      throw xtl::make_range_exception ("", "mode_index", mode_index, modes.size ());

    mode_desc = modes [mode_index];
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Output::GetModeDesc");
    throw;
  }
}

/*
    Установка текущего видео-режима
*/

void Output::SetCurrentMode (const OutputModeDesc& desc)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Output::SetCurrentMode");
    throw;
  }
}

void Output::GetCurrentMode (OutputModeDesc& mode_desc)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Output::GetCurrentMode");
    throw;
  }
}

/*
    Управление гамма-коррекцией
*/

void Output::SetGammaRamp (const render::low_level::Color3f table [256])
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Output::SetGammaRamp");
    throw;
  }
}

void Output::GetGammaRamp (render::low_level::Color3f table [256])
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Output::GetGammaRamp");
    throw;
  }
}

/*
    Список свойств устройства вывода
*/

IPropertyList* Output::GetProperties ()
{
  return &properties;
}
