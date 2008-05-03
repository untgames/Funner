#include "shared.h"

namespace
{

typedef unsigned char  uchar;
typedef unsigned short uint16;
typedef unsigned int   r24g8_t;
typedef int            fixed;

struct two_color8_t
{
  uchar red;
  uchar green;
};

struct rgb8_t
{
  uchar red;
  uchar green;
  uchar blue;
};

struct rgba8_t
{
  uchar red;
  uchar green;
  uchar blue;
  uchar alpha;
};

struct depth24_stencil8
{
  size_t depth_component : 24;
  size_t stencil_index   : 8;
};

/*
    Быстрое масштабирование изображений по размерам кратным двойке
*/

void scale_pixel (uchar& dest,uchar s1,uchar s2,uchar s3,uchar s4)
{
  dest = (s1 + s2 + s3 + s4) >> 2;
}

void scale_pixel (uint16& dest, uint16 s1, uint16 s2, uint16 s3, uint16 s4)
{
  dest = (size_t)(s1 + s2 + s3 + s4) >> 2;
}

void scale_pixel (depth24_stencil8& dest, const depth24_stencil8& s1, const depth24_stencil8& s2, const depth24_stencil8& s3, const depth24_stencil8& s4)
{
  dest.stencil_index   = (size_t)(s1.stencil_index + s2.stencil_index + s3.stencil_index + s4.stencil_index) / 4;
  dest.depth_component = (size_t)(s1.depth_component + s2.depth_component + s3.depth_component + s4.depth_component) / 4;
}

void scale_pixel (two_color8_t& dest, two_color8_t s1, two_color8_t s2, two_color8_t s3, two_color8_t s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   / 4;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) / 4;
}

void scale_pixel (rgb8_t& dest,const rgb8_t& s1,const rgb8_t& s2,const rgb8_t& s3,const rgb8_t& s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   / 4;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) / 4;
  dest.blue  = (size_t)(s1.blue  + s2.blue  + s3.blue  + s4.blue ) / 4;
}

void scale_pixel (rgba8_t& dest,const rgba8_t& s1,const rgba8_t& s2,const rgba8_t& s3,const rgba8_t& s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   / 4;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) / 4;
  dest.blue  = (size_t)(s1.blue  + s2.blue  + s3.blue  + s4.blue ) / 4;
  dest.alpha = (size_t)(s1.alpha + s2.alpha + s3.alpha + s4.alpha) / 4;
}

template <class T>
void scale_image_2x_down_impl (size_t width, size_t height, const void* in_src, void* in_dest)
{
  const T* src  = static_cast<const T*> (in_src);
  T*       dest = static_cast<T*> (in_dest);

  const T *s1 = src,
          *s2 = src + 1,
          *s3 = src + width,
          *s4 = s3  + 1; 

  size_t  w2  = width >> 1,
          h2  = height >> 1;

  for (size_t h = 0; h < h2; h++, s1 += width, s2 += width, s3 += width, s4 += width)
    for (size_t w = 0; w < w2; w++, s1 += 2, s2 += 2, s3 += 2, s4 += 2, dest++)
      scale_pixel (*dest,*s1,*s2,*s3,*s4);

  if (height == 1)
  {
    s3 = s1; s4 = s2;
    for (size_t w = 0; w < w2; w++, s1 += 2, s2 += 2, s3 += 2, s4 += 2, dest++)
      scale_pixel (*dest,*s1,*s2,*s3,*s4);
  }
  if (width == 1)
  {
    s3 = s1; s4 = s2;
    for (size_t h = 0; h < h2; h++, s1 += 2, s2 += 2, s3 += 2, s4 += 2, dest++)
      scale_pixel (*dest,*s1,*s2,*s3,*s4);
  }
}

fixed int2fixed (int data)
{
  return data << 16;
}

int fixed2int (fixed data)
{
  return data >> 16;
}

fixed float2fixed (float data)
{
  return (fixed)(data * 65536.f);
}

template <class T>
void scale_image_impl (size_t width, size_t height, const void* in_src, size_t new_width, size_t new_height, void* in_dst)
{
  const T* src = static_cast<const T*> (in_src);
  T*       dst = static_cast<T*> (in_dst);

  fixed src_width  = int2fixed (width),
        src_height = int2fixed (height),
        dst_width  = int2fixed (new_width),
        dst_height = int2fixed (new_height),
        dx         = float2fixed (float (width) / float (new_width)),
        dy         = float2fixed (float (height) / float (new_height)),
        src_y      = 0;
        
  const T* line = src;
         
  for (size_t dst_y=0; dst_y<new_height; dst_y++, src_y += dy)
  {
    if (fixed2int (src_y))
    {
      line  += fixed2int (src_y) * width;
      src_y &= 0xffff;
    }
    
    const T* s = line;
    
    for (size_t src_x=0, dst_x=0; dst_x<new_width; dst_x++, src_x += dx, dst++)
    {
      if (fixed2int (src_x))
      {
        s     += fixed2int (src_x);
        src_x &= 0xffff;
      }
      
      *dst = *s;
    }
  }
}

}

namespace render
{

namespace low_level
{

namespace opengl
{

void scale_image_2x_down (PixelFormat format, size_t width, size_t height, const void* src, void* dest)
{
  switch (format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_S8:    scale_image_2x_down_impl<uchar>        (width, height, src, dest); break;
    case PixelFormat_LA8:   scale_image_2x_down_impl<two_color8_t> (width, height, src, dest); break;
    case PixelFormat_D16:   scale_image_2x_down_impl<uint16>       (width, height, src, dest); break;
    case PixelFormat_RGB8:  scale_image_2x_down_impl<rgb8_t>       (width, height, src, dest); break;
    case PixelFormat_RGBA8: scale_image_2x_down_impl<rgba8_t>      (width, height, src, dest); break;
    case PixelFormat_D24S8:
    case PixelFormat_D24X8: scale_image_2x_down_impl<depth24_stencil8> (width, height, src, dest); break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    default:
      common::RaiseNotSupported ("render::low_level::opengl::scale_image_2x_down", "DXT image scaling not supported");
      break;
  }
}

void scale_image (PixelFormat format, size_t width, size_t height, size_t new_width, size_t new_height, const void* src, void* dest)
{
  switch (format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_S8:    scale_image_impl <uchar>        (width, height, src, new_width, new_height, dest); break;
    case PixelFormat_LA8:   scale_image_impl <two_color8_t> (width, height, src, new_width, new_height, dest); break;
    case PixelFormat_D16:   scale_image_impl <uint16>       (width, height, src, new_width, new_height, dest); break;
    case PixelFormat_RGB8:  scale_image_impl <rgb8_t>       (width, height, src, new_width, new_height, dest); break;
    case PixelFormat_RGBA8: scale_image_impl <rgba8_t>      (width, height, src, new_width, new_height, dest); break;
    case PixelFormat_D24S8:
    case PixelFormat_D24X8: scale_image_impl <depth24_stencil8>      (width, height, src, new_width, new_height, dest); break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    default:
      common::RaiseNotSupported ("render::low_level::opengl::scale_image", "DXT image scaling not supported");
      break;
  }
}

}

}

}
