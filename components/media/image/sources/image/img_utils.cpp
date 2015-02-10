#include "shared.h"

using namespace common;
using namespace media;

#pragma pack (1) //for fill

namespace
{

/*
    «аливка
*/

struct color_rgb8   { unsigned char red, green, blue; };
struct color_rgb16  { unsigned short red, green, blue; };
struct color_bgr8   { unsigned char blue, green, red; };
struct color_rgba8  { unsigned char red, green, blue, alpha; };
struct color_rgba16 { unsigned short red, green, blue, alpha; };
struct color_bgra8  { unsigned char blue, green, red, alpha; };
struct color_l8     { unsigned char luminance; };
struct color_a8     { unsigned char alpha; };
struct color_la8    { unsigned char luminance, alpha; };

template <class SrcT, class DstT> inline void set (const SrcT& src, DstT& dst) { dst = static_cast<SrcT> (src); }

template <class Color> inline void         set_red   (unsigned int value, Color& pixel) { set (value, pixel.red); }
template <class Color> inline void         set_green (unsigned int value, Color& pixel) { set (value, pixel.green); }
template <class Color> inline void         set_blue  (unsigned int value, Color& pixel) { set (value, pixel.blue); }
template <class Color> inline void         set_alpha (unsigned int value, Color& pixel) { set (value, pixel.alpha); }
template <class Color> inline unsigned int get_red   (const Color& pixel)               { return pixel.red; }
template <class Color> inline unsigned int get_green (const Color& pixel)               { return pixel.green; }
template <class Color> inline unsigned int get_blue  (const Color& pixel)               { return pixel.blue; }
template <class Color> inline unsigned int get_alpha (const Color& pixel)               { return pixel.alpha; }

inline unsigned int get_alpha (const color_rgb8&)      { return 0; }
inline unsigned int get_alpha (const color_rgb16&)     { return 0; }
inline unsigned int get_alpha (const color_bgr8&)      { return 0; }
inline unsigned int get_alpha (const color_l8&)        { return 0; }
inline unsigned int get_red   (const color_a8&)        { return 0; }
inline unsigned int get_green (const color_a8&)        { return 0; }
inline unsigned int get_blue  (const color_a8&)        { return 0; }
inline unsigned int get_red   (const color_l8& pixel)  { return pixel.luminance; }
inline unsigned int get_green (const color_l8& pixel)  { return pixel.luminance; }
inline unsigned int get_blue  (const color_l8& pixel)  { return pixel.luminance; }
inline unsigned int get_red   (const color_la8& pixel) { return pixel.luminance; }
inline unsigned int get_green (const color_la8& pixel) { return pixel.luminance; }
inline unsigned int get_blue  (const color_la8& pixel) { return pixel.luminance; }

inline void set_alpha (unsigned int, color_rgb8&) { }
inline void set_alpha (unsigned int, color_rgb16&) { }
inline void set_alpha (unsigned int, color_bgr8&) { }
inline void set_alpha (unsigned int, color_l8&) { }
inline void set_red   (unsigned int, color_a8&) { }
inline void set_green (unsigned int, color_a8&) { }
inline void set_blue  (unsigned int, color_a8&) { }
inline void set_red   (unsigned int, color_l8&) { }
inline void set_green (unsigned int, color_l8&) { }
inline void set_blue  (unsigned int, color_l8&) { }
inline void set_red   (unsigned int, color_la8&) { }
inline void set_green (unsigned int, color_la8&) { }
inline void set_blue  (unsigned int, color_la8&) { }

template <class Color> inline unsigned char get_luminance (const Color& pixel) { return (unsigned char)(((unsigned int)get_red (pixel) + get_green (pixel) + get_blue (pixel)) / 3); }

inline unsigned char get_luminance (const color_l8& pixel)  { return pixel.luminance; }
inline unsigned char get_luminance (const color_la8& pixel) { return pixel.luminance; }

template <class Color> inline void set_luminance (unsigned int, Color&) {}

inline void set_luminance (unsigned int value, color_l8& pixel)  { set (value, pixel.luminance); }
inline void set_luminance (unsigned int value, color_la8& pixel) { set (value, pixel.luminance); }

template <class SrcT, class DstT>
inline void copy (const SrcT& src, DstT& dst)
{
  set_luminance (get_luminance (src), dst);
  set_red       (get_red (src), dst);
  set_green     (get_green (src), dst);
  set_blue      (get_blue (src), dst);
  set_alpha     (get_alpha (src), dst);
}

template <class DstT, class SrcT>
void fill (const SrcT& src_color, Image& image)
{
  size_t layer_size = image.Width () * image.Height ();

  for (unsigned int z=0, depth=image.Depth (); z<depth; z++)
  {
    DstT* dst_color = reinterpret_cast<DstT*> (image.Bitmap (z));

    for (size_t i=0; i<layer_size; i++, dst_color++)
      copy (src_color, *dst_color);
  }
}

template <class SrcT>
void fill_dispatch (const void* source_color, Image& image)
{
  const SrcT& color = *reinterpret_cast<const SrcT*> (source_color);

  switch (image.Format ())
  {
    case PixelFormat_RGB8:   fill<color_rgb8> (color, image); break;
    case PixelFormat_RGB16:  fill<color_rgb16> (color, image); break;
    case PixelFormat_BGR8:   fill<color_bgr8> (color, image); break;
    case PixelFormat_RGBA8:  fill<color_rgba8> (color, image); break;
    case PixelFormat_RGBA16: fill<color_rgba16> (color, image); break;
    case PixelFormat_BGRA8:  fill<color_bgra8> (color, image); break;
    case PixelFormat_L8:     fill<color_l8> (color, image); break;
    case PixelFormat_A8:     fill<color_a8> (color, image); break;
    case PixelFormat_LA8:    fill<color_la8> (color, image); break;
    default:                 throw xtl::format_operation_exception ("media::fill_dispatch(const void*, Image&)", "Unknown image format %d", image.Format ());
  }
}

void fill_dispatch (PixelFormat source_format, const void* source_color, Image& image)
{
  switch (source_format)
  {
    case PixelFormat_RGB8:   fill_dispatch<color_rgb8> (source_color, image); break;
    case PixelFormat_RGB16:  fill_dispatch<color_rgb16> (source_color, image); break;
    case PixelFormat_BGR8:   fill_dispatch<color_bgr8> (source_color, image); break;
    case PixelFormat_RGBA8:  fill_dispatch<color_rgba8> (source_color, image); break;
    case PixelFormat_RGBA16: fill_dispatch<color_rgba16> (source_color, image); break;
    case PixelFormat_BGRA8:  fill_dispatch<color_bgra8> (source_color, image); break;
    case PixelFormat_L8:     fill_dispatch<color_l8> (source_color, image); break;
    case PixelFormat_A8:     fill_dispatch<color_a8> (source_color, image); break;
    case PixelFormat_LA8:    fill_dispatch<color_la8> (source_color, image); break;    
    default:                 throw xtl::make_argument_exception ("media::fill_dispatch(PixelFormat, const void*, Image&)", "source_format", source_format);
  }
}

}

/*
    ѕолучение характеристик формата
*/

namespace media
{

//им€ формата
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
unsigned short get_bits_per_pixel (PixelFormat format)
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

unsigned short get_bytes_per_pixel (PixelFormat format)
{
  return get_bits_per_pixel (format) / 8;
}

///ѕоиск границ непрозрачных пикселей внутри картинки
void crop_by_alpha (const Image& image, unsigned int crop_alpha, unsigned int& crop_x, unsigned int& crop_y, unsigned int& crop_width, unsigned int& crop_height)
{
  unsigned int alpha_offset = 0,
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

  unsigned int max_x       = image.Width (),
               max_y       = image.Height ();
  bool         pixel_found = false;
  
  unsigned short step = get_bytes_per_pixel (image.Format ());

  for (unsigned int y = 0; y < image_height; y++)
  {
    const unsigned char* data = image_bitmap + (image_height - 1 - y) * image_width * step + alpha_offset;
    
    for (unsigned int x = 0; x < image_width; x++, data += step)
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

/*
    «аливка картинки
*/

void fill (PixelFormat format, const void* data, Image& image)
{
  fill_dispatch (format, data, image);
}

}
