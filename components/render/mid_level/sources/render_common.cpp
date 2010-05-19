#include "shared.h"

using namespace render::mid_level;

/*
    Прямоугольная область
*/

Rect::Rect ()
{
  x      = 0;
  y      = 0;
  width  = 0;
  height = 0;
}
  
Rect::Rect (int in_x, int in_y, size_t in_width, size_t in_height)
{
  x      = in_x;
  y      = in_y;
  width  = in_width;
  height = in_height;
}

/*
    Получение имени формата
*/

namespace render
{

namespace mid_level
{

const char* get_name (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:  return "rgb8";
    case PixelFormat_RGBA8: return "rgba8";
    case PixelFormat_L8:    return "l8";
    case PixelFormat_A8:    return "a8";
    case PixelFormat_LA8:   return "la8";
    default:
      throw xtl::make_argument_exception ("render::mid_level::get_name(render::mid_level::PixelFormat)", "format", format);
  }
}

}

}
