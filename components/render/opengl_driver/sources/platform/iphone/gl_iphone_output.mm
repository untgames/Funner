#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::iphone;

/*
    Описание реализации устройства вывода
*/

struct Output::Impl
{
  OutputModeDesc mode_desc;  //поддерживаемый режим

  Impl ()
  {
    memset (&mode_desc, 0, sizeof (mode_desc));

    CGRect screen_bounds = [[UIScreen mainScreen] bounds];

    mode_desc.width        = screen_bounds.size.width;
    mode_desc.height       = screen_bounds.size.height;
    mode_desc.color_bits   = 24;
    mode_desc.refresh_rate = 0;
  }
};

/*
    Конструктор / деструктор
*/

Output::Output ()
  : impl (new Impl ())
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
  return "Main Screen";
}

/*
   Получение списка видео-режимов
*/

size_t Output::GetModesCount ()
{
  return 1;
}

void Output::GetModeDesc (size_t mode_index, OutputModeDesc& mode_desc)
{
  if (mode_index >= GetModesCount ())
    throw xtl::make_range_exception ("render::low_level::opengl::iphone::Output::GetModeDesc", "mode_index", mode_index, GetModesCount ());

  mode_desc = impl->mode_desc;
}

/*
   Установка текущего видео-режима
*/

void Output::SetCurrentMode (const OutputModeDesc& new_mode)
{
  if ((impl->mode_desc.width != new_mode.width) || (impl->mode_desc.height != new_mode.height) ||
      new_mode.refresh_rate || (new_mode.color_bits && (impl->mode_desc.color_bits != new_mode.color_bits)))
    throw xtl::format_operation_exception ("render::low_level::opengl::iphone::Output::SetCurrentMode", "Can't set output mode, you must use only modes, received by Output::GetModeDesc");
}

void Output::GetCurrentMode (OutputModeDesc& current_mode)
{
  current_mode = impl->mode_desc;
}

/*
   Управление гамма-коррекцией
*/

void Output::SetGammaRamp (const Color3f table [256])
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::iphone::Output::SetGammaRamp");
}

void Output::GetGammaRamp (Color3f table [256])
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::iphone::Output::GetGammaRamp");
}
