#include "shared.h"

using namespace render;

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

bool Rect::operator == (const Rect& area) const
{
  if (x      != area.x)      return false;
  if (y      != area.y)      return false;
  if (width  != area.width)  return false;
  if (height != area.height) return false;
  
  return true;
}

bool Rect::operator != (const Rect& area) const
{
  return !(*this == area);
}

/*
    Получение имени формата
*/

namespace render
{

const char* get_name (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:        return "rgb8";
    case PixelFormat_RGBA8:       return "rgba8";
    case PixelFormat_L8:          return "l8";
    case PixelFormat_A8:          return "a8";
    case PixelFormat_LA8:         return "la8";
    case PixelFormat_DXT1:        return "dxt1";
    case PixelFormat_DXT3:        return "dxt3";
    case PixelFormat_DXT5:        return "dxt5";
    case PixelFormat_RGB_PVRTC2:  return "rgb_pvrtc2";
    case PixelFormat_RGB_PVRTC4:  return "rgb_pvrtc4";
    case PixelFormat_RGBA_PVRTC2: return "rgba_pvrtc2";
    case PixelFormat_RGBA_PVRTC4: return "rgba_pvrtc4";    
    default:
      throw xtl::make_argument_exception ("render::get_name(render::PixelFormat)", "format", format);
  }
}

}
