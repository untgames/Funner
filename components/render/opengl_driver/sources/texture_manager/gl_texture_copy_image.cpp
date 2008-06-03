#include "shared.h"

using namespace render::low_level::opengl;
using namespace render::low_level;
using namespace common;

namespace
{

/*
    Поддерживаемые форматы пикселей
*/

#pragma pack(1)

struct PixelRgb8
{
  unsigned char red;
  unsigned char green;
  unsigned char blue;  
};

struct PixelA8
{
  unsigned char alpha;
};

struct PixelL8
{
  unsigned char luminance;
};

struct PixelRgba8: public PixelRgb8, public PixelA8 {};

struct PixelLa8: public PixelL8, PixelA8 {};

/*
    Копирование данных пикселя
*/

inline unsigned char get_red       (const void*) { return 0; }
inline unsigned char get_green     (const void*) { return 0; }
inline unsigned char get_blue      (const void*) { return 0; }
inline unsigned char get_alpha     (const void*) { return 0xff; }
inline unsigned char get_luminance (const void*) { return 0; }

inline void set_red       (void*, unsigned char) {}
inline void set_green     (void*, unsigned char) {}
inline void set_blue      (void*, unsigned char) {}
inline void set_alpha     (void*, unsigned char) {}
inline void set_luminance (void*, unsigned char) {}

inline unsigned char get_red       (const PixelRgb8* pixel) { return pixel->red; }
inline unsigned char get_red       (const PixelL8*   pixel) { return pixel->luminance; }
inline unsigned char get_green     (const PixelRgb8* pixel) { return pixel->green; }
inline unsigned char get_blue      (const PixelRgb8* pixel) { return pixel->blue; }
inline unsigned char get_alpha     (const PixelA8*   pixel) { return pixel->alpha; }
inline unsigned char get_luminance (const PixelL8*   pixel) { return pixel->luminance; }
inline unsigned char get_luminance (const PixelRgb8* pixel) { return pixel->red; }

inline void set_red       (PixelRgb8* pixel, unsigned char value) { pixel->red       = value; }
inline void set_green     (PixelRgb8* pixel, unsigned char value) { pixel->green     = value; }
inline void set_blue      (PixelRgb8* pixel, unsigned char value) { pixel->blue      = value; }
inline void set_alpha     (PixelA8*   pixel, unsigned char value) { pixel->alpha     = value; }
inline void set_luminance (PixelL8*   pixel, unsigned char value) { pixel->luminance = value; }

/*
    Копирование образов в различных форматах
*/

template <class SrcT, class DstT>
void copy_image_dispatch2 (size_t pixels_count, const void* source_buffer, void* destination_buffer)
{
  const SrcT* src = reinterpret_cast<const SrcT*> (source_buffer);
  DstT*       dst = reinterpret_cast<DstT*> (destination_buffer);

  for (;pixels_count--; src++, dst++)
  {
    set_red       (dst, get_red       (src));
    set_green     (dst, get_green     (src));
    set_blue      (dst, get_blue      (src));
    set_luminance (dst, get_luminance (src));
    set_alpha     (dst, get_alpha     (src));
  }
}

template <class SrcT>
void copy_image_dispatch (size_t pixels_count, const void* src_buffer, PixelFormat dst_format, void* dst_buffer)
{
  switch (dst_format)
  {
    case PixelFormat_RGB8:  copy_image_dispatch2<SrcT, PixelRgb8>  (pixels_count, src_buffer, dst_buffer); break;
    case PixelFormat_RGBA8: copy_image_dispatch2<SrcT, PixelRgba8> (pixels_count, src_buffer, dst_buffer); break;
    case PixelFormat_A8:    copy_image_dispatch2<SrcT, PixelA8>    (pixels_count, src_buffer, dst_buffer); break;
    case PixelFormat_L8:    copy_image_dispatch2<SrcT, PixelL8>    (pixels_count, src_buffer, dst_buffer); break;
    case PixelFormat_LA8:   copy_image_dispatch2<SrcT, PixelLa8>   (pixels_count, src_buffer, dst_buffer); break;
    default:
      raise_not_supported ("render::low_level::opengl::copy_image_dispatch", "Unsupported destination format %s", get_name (dst_format));
      break;
  }
}

}

namespace render
{

namespace low_level
{

namespace opengl
{

void copy_image (size_t pixels_count, PixelFormat src_format, const void* src_buffer, PixelFormat dst_format, void* dst_buffer)
{
  switch (src_format)
  {
    case PixelFormat_RGB8:  copy_image_dispatch<PixelRgb8>  (pixels_count, src_buffer, dst_format, dst_buffer); break;
    case PixelFormat_RGBA8: copy_image_dispatch<PixelRgba8> (pixels_count, src_buffer, dst_format, dst_buffer); break;
    case PixelFormat_A8:    copy_image_dispatch<PixelA8>    (pixels_count, src_buffer, dst_format, dst_buffer); break;
    case PixelFormat_L8:    copy_image_dispatch<PixelL8>    (pixels_count, src_buffer, dst_format, dst_buffer); break;
    case PixelFormat_LA8:   copy_image_dispatch<PixelLa8>   (pixels_count, src_buffer, dst_format, dst_buffer); break;
    default:
      raise_not_supported ("render::low_level::opengl::copy_image", "Unsupported source format %s", get_name (src_format));
      break;
  }
}

}

}

}
