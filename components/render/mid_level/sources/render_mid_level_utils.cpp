#include <xtl/common_exceptions.h>

#include <render/mid_level/common.h>

namespace render
{

namespace mid_level
{

/*
    Утилиты
*/

//получение имени формата
const char* get_name (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:  return "PixelFormat_RGB8";
    case PixelFormat_RGBA8: return "PixelFormat_RGBA8";
    case PixelFormat_L8:    return "PixelFormat_L8";
    case PixelFormat_A8:    return "PixelFormat_A8";
    case PixelFormat_LA8:   return "PixelFormat_LA8";
    default:
      throw xtl::make_argument_exception ("render::mid_level::get_name(PixelFormat)", "format", format);
  }
}

}

}
