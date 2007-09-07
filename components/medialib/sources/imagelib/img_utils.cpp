#include "shared.h"

using namespace common;

/*
    Получение характеристик формата
*/

namespace media
{

//имя формата
const char* get_format_name (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:   return "rgb8";
    case PixelFormat_RGB16:  return "rgb16";
    case PixelFormat_BGR8:   return "bgr8";
    case PixelFormat_RGBA8:  return "rgba8";
    case PixelFormat_RGBA16: return "rgba16";
    case PixelFormat_BGRA8:  return "bgra8";
    case PixelFormat_L8:     return "l8";
    case PixelFormat_A8:     return "a8";
    case PixelFormat_LA8:    return "la8";
    default:                 RaiseInvalidArgument ("media::get_format_name(PixelFormat)", "format", format);
  }

  return "";
}

//количество бит на пиксель
size_t get_bits_per_pixel (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_RGB8:   return 24;
    case PixelFormat_RGB16:  return 48;
    case PixelFormat_BGR8:   return 24;
    case PixelFormat_RGBA8:  return 32;
    case PixelFormat_RGBA16: return 64;
    case PixelFormat_BGRA8:  return 32;
    case PixelFormat_L8:     return 8;
    case PixelFormat_A8:     return 8;
    case PixelFormat_LA8:    return 16;
    default:                 RaiseInvalidArgument ("media::get_bits_per_pixel(PixelFormat)", "format", format);
  }

  return 0; 
}

size_t get_bytes_per_pixel (PixelFormat format)
{
  return get_bits_per_pixel (format) / 8;
}

}
