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
    case PixelFormat_Default: return "default";
    case PixelFormat_RGB8:    return "rgb8";
    case PixelFormat_RGB16:   return "rgb16";
    case PixelFormat_BGR8:    return "bgr8";
    case PixelFormat_RGBA8:   return "rgba8";
    case PixelFormat_RGBA16:  return "rgba16";
    case PixelFormat_BGRA8:   return "bgra8";
    case PixelFormat_L8:      return "l8";
    case PixelFormat_A8:      return "a8";
    case PixelFormat_LA8:     return "la8";
    default:                  throw xtl::make_argument_exception ("media::get_format_name(PixelFormat)", "format", format);
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
    default:                 throw xtl::make_argument_exception ("media::get_bits_per_pixel(PixelFormat)", "format", format);
  }

  return 0; 
}

size_t get_bytes_per_pixel (PixelFormat format)
{
  return get_bits_per_pixel (format) / 8;
}

///Поиск границ непрозрачных пикселей внутри картинки
void crop_by_alpha (const Image& image, size_t crop_alpha, size_t& crop_x, size_t& crop_y, size_t& crop_width, size_t& crop_height)
{
  size_t alpha_offset = 0,
         image_width  = image.Width (),
         image_height = image.Height ();

  switch (image.Format ())
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGB16:
    case PixelFormat_BGR8:
    case PixelFormat_L8:
      crop_x      = 0;
      crop_y      = 0;
      crop_width  = image_width;
      crop_height = image_height;
      return;
    case PixelFormat_RGBA8:
    case PixelFormat_BGRA8:  alpha_offset = 3; break;
    case PixelFormat_A8:     alpha_offset = 0; break;
    case PixelFormat_LA8:    alpha_offset = 1; break;
    case PixelFormat_RGBA16:
      throw xtl::format_not_supported_exception ("media::crop_by_alpha", "Can't crop image with format %s (not supported)", get_format_name (image.Format ()));
    default:
      throw xtl::format_operation_exception ("media::crop_by_alpha", "Unknown image pixel format %d", image.Format ());
  }

  const unsigned char* image_bitmap = (const unsigned char*)image.Bitmap ();

  size_t max_x       = image.Width (),
         max_y       = image.Height ();
  bool   pixel_found = false;
  
  size_t step = get_bytes_per_pixel (image.Format ());

  for (size_t y = 0; y < image_height; y++)
  {
    const unsigned char* data = image_bitmap + (image_height - 1 - y) * image_width * step + alpha_offset;
    
    for (size_t x = 0; x < image_width; x++, data += step)
    {
      if (*data < crop_alpha)
        continue;

      if (!pixel_found)
      {
        crop_x      = x;
        crop_y      = y;
        max_x       = x;
        max_y       = y;
        pixel_found = true;        
        
        continue;
      }

      if (x < crop_x) crop_x = x;
      if (y < crop_y) crop_y = y;
      if (x > max_x)  max_x  = x;
      if (y > max_y)  max_y  = y;
    }
  }
  
  if (!pixel_found)
  {
    crop_x      = 0;
    crop_y      = 0;
    crop_width  = 0;
    crop_height = 0;

    return;  
  }
  
  crop_width  = max_x - crop_x + 1;
  crop_height = max_y - crop_y + 1;
}

}
