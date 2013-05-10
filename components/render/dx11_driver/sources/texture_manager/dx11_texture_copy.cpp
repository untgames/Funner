#include "shared.h"

#pragma pack(1)

namespace render
{

namespace low_level
{

namespace dx11
{

namespace
{

/*
    Форматы пикселей
*/

struct pixel_rgb       { unsigned char red, green, blue; };
struct pixel_alpha     { unsigned char alpha; };
struct pixel_luminance { unsigned char luminance; };

struct pixel_rgba: public pixel_rgb, public pixel_alpha {};
struct pixel_la: public pixel_luminance, public pixel_alpha {};

template <class T, size_t DepthBits = sizeof (T) * 8, size_t StencilBits = sizeof (T) * 8 - DepthBits>
struct pixel_ds
{
  enum
  {
    DEPTH_MASK   = (1ull << DepthBits) - 1,
    STENCIL_MASK = ((1ull << StencilBits) - 1) << DepthBits,
  };

  T depth_stencil;

  unsigned int depth   () const { return (((unsigned int)src.depth_stencil) & DEPTH_MASK) << (32 - DepthBits); }
  unsigned int stencil () const { return (((((unsigned int)src.depth_stencil) & STENCIL_MASK)) >> DepthBits) << (32 - StencilBits); }

  void set (unsigned int depth, unsigned int stencil) 
  {
    depth_stencil = ((T)(depth >> (32 - DepthBits))) | (((T)(stencil >> (32 - StencilBits))) << DepthBits);
  }

  void set (const pixel_ds& value) { depth_stencil = value.depth_stencil; }

  template <class T1, size_t DepthBits1, size_t StencilBits1>
  void set (const pixel_ds<T1, DepthBits1, StencilBits1>& value)
  {
    set (value.depth (), value.stencil ());
  }
};

/*
    Проверка возможности преобразования
*/

template <class Src, class Dst> struct has_conversion { enum { value = 0 }; };

template <class T> struct has_conversion<T, T>    { enum { value = 1 }; };

template <> struct has_conversion<pixel_rgb, pixel_rgba>      { enum { value = 1 }; };
template <> struct has_conversion<pixel_rgb, pixel_luminance> { enum { value = 1 }; };
template <> struct has_conversion<pixel_rgb, pixel_alpha>     { enum { value = 1 }; };
template <> struct has_conversion<pixel_rgb, pixel_la>        { enum { value = 1 }; };

template <> struct has_conversion<pixel_rgba, pixel_rgb>       { enum { value = 1 }; };
template <> struct has_conversion<pixel_rgba, pixel_luminance> { enum { value = 1 }; };
template <> struct has_conversion<pixel_rgba, pixel_alpha>     { enum { value = 1 }; };
template <> struct has_conversion<pixel_rgba, pixel_la>        { enum { value = 1 }; };

template <> struct has_conversion<pixel_alpha, pixel_rgb>       { enum { value = 1 }; };
template <> struct has_conversion<pixel_alpha, pixel_rgba>      { enum { value = 1 }; };
template <> struct has_conversion<pixel_alpha, pixel_luminance> { enum { value = 1 }; };
template <> struct has_conversion<pixel_alpha, pixel_la>        { enum { value = 1 }; };

template <> struct has_conversion<pixel_luminance, pixel_rgb>    { enum { value = 1 }; };
template <> struct has_conversion<pixel_luminance, pixel_rgba>   { enum { value = 1 }; };
template <> struct has_conversion<pixel_luminance, pixel_alpha>  { enum { value = 1 }; };
template <> struct has_conversion<pixel_luminance, pixel_la>     { enum { value = 1 }; };

template <> struct has_conversion<pixel_la, pixel_rgb>       { enum { value = 1 }; };
template <> struct has_conversion<pixel_la, pixel_rgba>      { enum { value = 1 }; };
template <> struct has_conversion<pixel_la, pixel_alpha>     { enum { value = 1 }; };
template <> struct has_conversion<pixel_la, pixel_luminance> { enum { value = 1 }; };

template <class T1, size_t DepthBits1, size_t StencilBits1, class T2, size_t DepthBits2, size_t StencilBits2>
struct has_conversion<pixel_ds<T1, DepthBits1, StencilBits1>, pixel_ds<T2, DepthBits2, StencilBits2> > { enum { value = 1 }; };

/*
    Копирование пикселя
*/

/// Same types
template <class T> inline void copy_pixel (const T& src, T& dst) { dst = src; }

/// Luminance alpha special case
template <class Src>
inline void copy_pixel (const Src& src, pixel_la& dst)
{
  copy_pixel (src, static_cast<pixel_luminance&> (dst));
  copy_pixel (src, static_cast<pixel_alpha&> (dst));
}

template <class Dst>
inline void copy_pixel (const pixel_la& src, Dst& dst)
{
  copy_pixel (static_cast<const pixel_luminance&> (src), dst);
  copy_pixel (static_cast<const pixel_alpha&> (src), dst);
}

inline void copy_pixel (const pixel_la& src, pixel_la& dst)
{
  dst = src;
}

///RGBA special case
template <class Src>
inline void copy_pixel (const Src& src, pixel_rgba& dst)
{
  copy_pixel (src, static_cast<pixel_rgb&> (dst));
  copy_pixel (src, static_cast<pixel_alpha&> (dst));
}

template <class Dst>
inline void copy_pixel (const pixel_rgba& src, Dst& dst)
{
  copy_pixel (static_cast<const pixel_rgb&> (src), dst);
  copy_pixel (static_cast<const pixel_alpha&> (src), dst);
}

inline void copy_pixel (const pixel_rgba& src, pixel_rgba& dst)
{
  dst = src;
}

///RGB ->
inline void copy_pixel (const pixel_rgb& src, pixel_luminance& dst) { dst.luminance = (unsigned char)((unsigned short)src.red + (unsigned short)src.green + (unsigned short)src.blue) / 3; }
inline void copy_pixel (const pixel_rgb& src, pixel_alpha& dst)     { dst.alpha = 0; }

///Alpha ->
inline void copy_pixel (const pixel_alpha&, pixel_rgb& dst)       { dst.red = dst.green = dst.blue = 0; }
inline void copy_pixel (const pixel_alpha&, pixel_luminance& dst) { dst.luminance = 0; }

///Luminance ->
inline void copy_pixel (const pixel_luminance& src, pixel_rgb& dst) { dst.red = dst.green = dst.blue = src.luminance; }
inline void copy_pixel (const pixel_luminance&, pixel_alpha& dst)   { dst.alpha = 0; }

///Depth/stencil
template <class T1, size_t DepthBits1, size_t StencilBits1, class T2, size_t DepthBits2, size_t StencilBits2>
inline void copy_pixel (const pixel_ds<T1, DepthBits1, StencilBits1>& src, pixel_ds<T2, DepthBits2, StencilBits2>& dst)
{
  dst.set (src);
}

/*
    Копирование образов
*/

inline void copy_direct (PixelFormat format, size_t width, size_t height, size_t depth, size_t src_row_pitch, size_t src_depth_pitch, const void* src_buffer, size_t dst_row_pitch, size_t dst_depth_pitch, void* dst_buffer)
{
  if (src_row_pitch != dst_row_pitch)
    throw xtl::format_operation_exception ("", "Can't copy data in %s with src_row_pitch=%u and dst_row_pitch=%u", get_name (format), src_row_pitch, dst_row_pitch);

  size_t layer_size = src_depth_pitch < dst_depth_pitch ? src_depth_pitch : dst_depth_pitch;

  const char* src = (const char*)src_buffer;
  char*       dst = (char*)dst_buffer;
  
  for (size_t count=depth; count--; src += src_depth_pitch, dst += dst_depth_pitch) 
    memcpy (dst, src, layer_size);
}

template <bool Value> struct selector {};

template <class Src, class Dst>
inline bool copy_dispatch (size_t width, size_t height, size_t depth, size_t src_row_pitch, size_t src_depth_pitch, const Src* src, size_t dst_row_pitch, size_t dst_depth_pitch, Dst* dst, selector<false>)
{
  return false;
}

template <class Src, class Dst>
inline bool copy_dispatch (size_t width, size_t height, size_t depth, size_t src_row_pitch, size_t src_depth_pitch, const Src* src_buffer, size_t dst_row_pitch, size_t dst_depth_pitch, Dst* dst_buffer, selector<true>)
{
  const size_t src_row_skip   = src_row_pitch - width * sizeof (Src),
               dst_row_skip   = dst_row_pitch - width * sizeof (Dst),
               src_depth_skip = src_depth_pitch - height * src_row_pitch,
               dst_depth_skip = dst_depth_pitch - height * dst_row_pitch;

  const char* src = (const char*)src_buffer;
  char*       dst = (char*)dst_buffer;
  
  for (size_t count=depth; count--; src += src_depth_skip, dst += dst_depth_skip)  
    for (size_t count=height; count--; src += src_row_skip, dst += dst_row_skip)
      for (size_t count=width; count--; src += sizeof (Src), dst += sizeof (Dst))
        copy_pixel (*reinterpret_cast<Src*> (src), *reinterpret_cast<Dst*> (dst));

  return true;
}

template <class Src, class Dst>
inline bool copy_dispatch (size_t width, size_t height, size_t depth, size_t src_row_pitch, size_t src_depth_pitch, const Src* src, size_t dst_row_pitch, size_t dst_depth_pitch, Dst* dst)
{
  return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src, dst_row_pitch, dst_depth_pitch, dst, selector<has_conversion<Src, Dst>::value> ());
}

template <class Src>
inline bool copy_dispatch (size_t width, size_t height, size_t depth, size_t src_row_pitch, size_t src_depth_pitch, const Src* src_buffer, size_t dst_row_pitch, size_t dst_depth_pitch, PixelFormat dst_format, void* dst_buffer)
{
  switch (dst_format)
  {
    case PixelFormat_RGB8:    return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, reinterpret_cast<const pixel_rgb*> (dst_buffer));
    case PixelFormat_RGBA8:   return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, reinterpret_cast<const pixel_rgba*> (dst_buffer));
    case PixelFormat_L8:      return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, reinterpret_cast<const pixel_luminance*> (dst_buffer));
    case PixelFormat_A8:      return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, reinterpret_cast<const pixel_alpha*> (dst_buffer));
    case PixelFormat_LA8:     return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, reinterpret_cast<const pixel_la*> (dst_buffer));
    case PixelFormat_D16:     return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, reinterpret_cast<const pixel_ds<unsigned short>*> (dst_buffer));
    case PixelFormat_D24X8:   return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, reinterpret_cast<const pixel_ds<unsigned int, 24, 0>*> (dst_buffer));
    case PixelFormat_D24S8:   return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, reinterpret_cast<const pixel_ds<unsigned int, 24, 8>*> (dst_buffer));
    case PixelFormat_D32:     return copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, reinterpret_cast<const pixel_ds<unsigned int>*> (dst_buffer));
    default:                  return false;
  }
}

}

void copy 
 (size_t      width,
  size_t      height,
  size_t      depth,
  size_t      src_row_pitch,
  size_t      src_depth_pitch,
  PixelFormat src_format,
  const void* src_buffer,
  size_t      dst_row_pitch,
  size_t      dst_depth_pitch,
  PixelFormat dst_format,
  void*       dst_buffer)
{
  if (src_format == dst_format)
  {
    copy_direct (src_format, width, height, depth, src_row_pitch, src_depth_pitch, src_buffer, dst_row_pitch, dst_depth_pitch, dst_buffer);
    return;
  }

  bool result = false;

  switch (src_format)
  {
    case PixelFormat_RGB8:    result = copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, reinterpret_cast<const pixel_rgb*> (src_buffer),                     dst_row_pitch, dst_depth_pitch, dst_format, dst_buffer); break;
    case PixelFormat_RGBA8:   result = copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, reinterpret_cast<const pixel_rgba*> (src_buffer),                    dst_row_pitch, dst_depth_pitch, dst_format, dst_buffer); break;
    case PixelFormat_L8:      result = copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, reinterpret_cast<const pixel_luminance*> (src_buffer),               dst_row_pitch, dst_depth_pitch, dst_format, dst_buffer); break;
    case PixelFormat_A8:      result = copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, reinterpret_cast<const pixel_alpha*> (src_buffer),                   dst_row_pitch, dst_depth_pitch, dst_format, dst_buffer); break;
    case PixelFormat_LA8:     result = copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, reinterpret_cast<const pixel_la*> (src_buffer),                      dst_row_pitch, dst_depth_pitch, dst_format, dst_buffer); break;
    case PixelFormat_D16:     result = copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, reinterpret_cast<const pixel_ds<unsigned short>*> (src_buffer),      dst_row_pitch, dst_depth_pitch, dst_format, dst_buffer); break;
    case PixelFormat_D24X8:   result = copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, reinterpret_cast<const pixel_ds<unsigned int, 24, 0>*> (src_buffer), dst_row_pitch, dst_depth_pitch, dst_format, dst_buffer); break;
    case PixelFormat_D24S8:   result = copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, reinterpret_cast<const pixel_ds<unsigned int, 24, 8>*> (src_buffer), dst_row_pitch, dst_depth_pitch, dst_format, dst_buffer); break;
    case PixelFormat_D32:     result = copy_dispatch (width, height, depth, src_row_pitch, src_depth_pitch, reinterpret_cast<const pixel_ds<unsigned int>*> (src_buffer),        dst_row_pitch, dst_depth_pitch, dst_format, dst_buffer); break;
    default:
      break;
  }

  if (!result)
    throw xtl::format_operation_exception ("", "There is not conversion for image %s to %s (src_row_pitch=%u, src_depth_pitch=%u, dst_row_pitch=%u, dst_depth_pitch=%u)",
      get_name (src_format), get_name (dst_format), src_row_pitch, src_depth_pitch, dst_row_pitch, dst_depth_pitch);
}

}

}

}
