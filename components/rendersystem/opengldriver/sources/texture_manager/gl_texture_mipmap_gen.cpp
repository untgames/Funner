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

/*
  быстрое масштабирование изображений по размеру кратных двойке
*/

////////////////////////////////////////////////////////////////////////////////////////////
///Фильтры
////////////////////////////////////////////////////////////////////////////////////////////

void scale_pixel (uchar& dest,uchar s1,uchar s2,uchar s3,uchar s4)
{
  dest = (s1 + s2 + s3 + s4) >> 2;
}

void scale_pixel (uint16& dest, uint16 s1, uint16 s2, uint16 s3, uint16 s4)
{
  dest = (size_t)(s1 + s2 + s3 + s4) >> 2;
}

void scale_pixel (r24g8_t& dest, const r24g8_t& s1, const r24g8_t& s2, const r24g8_t& s3, const r24g8_t& s4)
{
  //Непереносимо!!!
  dest = ((s1 & 0xffffff) + (s2 & 0xffffff) + (s3 & 0xffffff) + (s4 & 0xffffff)) >> 2;
  dest |= ((((s1 & 0xff000000) >> 24) + ((s2 & 0xff000000) >> 24) + ((s3 & 0xff000000) >> 24) + ((s4 & 0xff000000) >> 24)) >> 2) << 24;
}

void  scale_pixel (two_color8_t& dest, two_color8_t s1, two_color8_t s2, two_color8_t s3, two_color8_t s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   >> 2;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) >> 2;
}

void  scale_pixel (rgb8_t& dest,const rgb8_t& s1,const rgb8_t& s2,const rgb8_t& s3,const rgb8_t& s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   >> 2;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) >> 2;
  dest.blue  = (size_t)(s1.blue  + s2.blue  + s3.blue  + s4.blue ) >> 2;
}

void  scale_pixel (rgba8_t& dest,const rgba8_t& s1,const rgba8_t& s2,const rgba8_t& s3,const rgba8_t& s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   >> 2;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) >> 2;
  dest.blue  = (size_t)(s1.blue  + s2.blue  + s3.blue  + s4.blue ) >> 2;
  dest.alpha = (size_t)(s1.alpha + s2.alpha + s3.alpha + s4.alpha) >> 2;
}

////////////////////////////////////////////////////////////////////////////////////////////
///Собственно масштабирование
////////////////////////////////////////////////////////////////////////////////////////////

template <class T>
void scale_image_2x_down_impl (size_t width, size_t height, const T* src,T* dest)
{
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

////////////////////////////////////////////////////////////////////////////////////////////
///Полное мастштабирование (не более чем в два раза)
////////////////////////////////////////////////////////////////////////////////////////////
template <class T>
void scale_image_impl (size_t width, size_t height, const T* src, size_t newWidth, size_t newHeight, T* res)
{
  fixed    sw  = int2fixed (width),
           sh  = int2fixed (height),
           rw  = int2fixed (newWidth),
           rh  = int2fixed (newHeight);
  fixed    dkx = float2fixed ((float)width / (float)newWidth), 
           dky = float2fixed ((float)height / (float)newHeight);
  fixed    sx  = 0, 
           sy  = 0;
  const T* line = src;

  for (size_t ry = 0; ry < newHeight; ry++, sy += dky)
  {
    if (fixed2int (sy) > 0)
    {
      line += fixed2int (sy) * width;
      sy   &= 0xFFFF;
    }

    sx         = 0;
    const T* s = line;

    for (size_t rx = 0; rx < newWidth; rx++, sx += dkx, res++)
    {
      if (fixed2int (sx) > 0)
      {
        s   += fixed2int (sx);
        sx  &= 0xFFFF;
      }  

      *res = *s;   
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
    case PixelFormat_S8:    scale_image_2x_down_impl <uchar>        (width, height, (uchar*)       src, (uchar*)       dest); break;
    case PixelFormat_LA8:   scale_image_2x_down_impl <two_color8_t> (width, height, (two_color8_t*)src, (two_color8_t*)dest); break;
    case PixelFormat_D16:   scale_image_2x_down_impl <uint16>       (width, height, (uint16*)      src, (uint16*)      dest); break;
    case PixelFormat_RGB8:  scale_image_2x_down_impl <rgb8_t>       (width, height, (rgb8_t*)      src, (rgb8_t*)      dest); break;
    case PixelFormat_RGBA8: scale_image_2x_down_impl <rgba8_t>      (width, height, (rgba8_t*)     src, (rgba8_t*)     dest); break;
    case PixelFormat_D24S8:
    case PixelFormat_D24X8: scale_image_2x_down_impl <r24g8_t>      (width, height, (r24g8_t*)     src, (r24g8_t*)     dest); break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    default: common::RaiseNotSupported ("render::low_level::opengl::scale_image_2x_down", "DXT image scaling not supported.");;
  }
}

void scale_image (PixelFormat format, size_t width, size_t height, 
                                             size_t new_width, size_t new_height, const void* src, void* dest)
{
  switch (format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_S8:    scale_image_impl <uchar>        (width, height, (uchar*)       src, new_width, new_height, (uchar*)       dest); break;
    case PixelFormat_LA8:   scale_image_impl <two_color8_t> (width, height, (two_color8_t*)src, new_width, new_height, (two_color8_t*)dest); break;
    case PixelFormat_D16:   scale_image_impl <uint16>       (width, height, (uint16*)      src, new_width, new_height, (uint16*)      dest); break;
    case PixelFormat_RGB8:  scale_image_impl <rgb8_t>       (width, height, (rgb8_t*)      src, new_width, new_height, (rgb8_t*)      dest); break;
    case PixelFormat_RGBA8: scale_image_impl <rgba8_t>      (width, height, (rgba8_t*)     src, new_width, new_height, (rgba8_t*)     dest); break;
    case PixelFormat_D24S8:
    case PixelFormat_D24X8: scale_image_impl <r24g8_t>      (width, height, (r24g8_t*)     src, new_width, new_height, (r24g8_t*)     dest); break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    default: common::RaiseNotSupported ("render::low_level::opengl::scale_image", "DXT image scaling not supported.");
  }
}

namespace
{

size_t get_next_mip_size (size_t size)
{
  switch (size)
  {
    case 0:
    case 1:   return size;
    default:  return size / 2;
  }
}

}

namespace
{

//убрать!!!!

void generate_mips_internal
 (size_t       x,
  size_t       y,
  size_t       z,
  size_t       width,
  size_t       height,
  PixelFormat  format,
  const void*  data,
  SetTexDataFn fn,
  bool         is_cubemap)
{
  if (!fn)
    common::RaiseNullArgument ("render::low_level::opengl::generate_mips", "fn");

  const GLenum gl_texformat = gl_format (format),
               gl_textype   = gl_type (format);

  const size_t buffer_size = get_next_mip_size (width) * get_next_mip_size (height) * texel_size (format);

  xtl::uninitialized_storage<char> dst_buffer (buffer_size),
                                   src_buffer (buffer_size / 2); //на 4 делить не нужно!

  const char* src = reinterpret_cast<const char*> (data);
  char*       dst = dst_buffer.data ();
    
  for (size_t mip_level=1; width > 1 || height > 1; mip_level++)
  {
    scale_image_2x_down (format, width, height, src, dst);

    width   = get_next_mip_size (width);
    height  = get_next_mip_size (height);
    x      /= 2;
    y      /= 2;    

    if (!is_cubemap)
      z /= 2;

    fn (mip_level, x, y, z, width, height, gl_texformat, gl_textype, dst);

    dst_buffer.swap (src_buffer);

    dst = dst_buffer.data ();
    src = src_buffer.data ();
  }
}

}

void generate_mips
 (size_t       x,
  size_t       y,
  size_t       z,
  size_t       width,
  size_t       height,
  PixelFormat  format,
  const void*  data,
  SetTexDataFn fn)
{
  generate_mips_internal (x, y, z, width, height, format, data, fn, false);
}

void generate_cubemap_mips
 (size_t       x,
  size_t       y,
  size_t       z,
  size_t       width,
  size_t       height,
  PixelFormat  format,
  const void*  data,
  SetTexDataFn fn)
{
  generate_mips_internal (x, y, z, width, height, format, data, fn, true);
}

}

}

}

