#include "shared.h"

namespace render
{

namespace mid_level
{

namespace debug
{

/*
    Утилиты
*/

//получение эквивалента формата
media::PixelFormat get_image_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:  return media::PixelFormat_RGB8;
    case PixelFormat_RGBA8: return media::PixelFormat_RGBA8;
    case PixelFormat_L8:    return media::PixelFormat_L8;
    case PixelFormat_A8:    return media::PixelFormat_A8;
    case PixelFormat_LA8:   return media::PixelFormat_LA8;
    default:
      throw xtl::make_argument_exception ("render::mid_level::debug::get_image_format", "format", format);
  }
}

//получение эквивалента формата
PixelFormat get_format (media::PixelFormat format)
{
  static const char* METHOD_NAME = "render::mid_level::debug::get_format";

  switch (format)
  {
    case media::PixelFormat_RGB8:  return PixelFormat_RGB8;
    case media::PixelFormat_RGBA8: return PixelFormat_RGBA8;
    case media::PixelFormat_L8:    return PixelFormat_L8;
    case media::PixelFormat_A8:    return PixelFormat_A8;
    case media::PixelFormat_LA8:   return PixelFormat_LA8;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format", format);
  }
}

}

}

}
