#ifndef RENDER_GL_DRIVER_SCALE
#define RENDER_GL_DRIVER_SCALE

#include "shared.h"
#include <memory.h>

typedef unsigned char uchar;

struct data16bit_t
{
  uchar data[2];
};

struct data24bit_t
{
  uchar data[3];
};

struct data32bit_t
{
  uchar data[4];
};

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

__forceinline  
void __fastcall ScalePixel (uchar& dest,uchar s1,uchar s2,uchar s3,uchar s4)
{
  dest = (s1 + s2 + s3 + s4) >> 2;
}

__forceinline  
void __fastcall ScalePixel (data16bit_t& dest, data16bit_t s1, data16bit_t s2, data16bit_t s3, data16bit_t s4)
{
  size_t data[4];

  memcpy (&data[0], &s1, 2);
  memcpy (&data[1], &s2, 2);
  memcpy (&data[2], &s3, 2);
  memcpy (&data[3], &s4, 2);

  data [0] = (data[0] + data[1] + data[2] + data[3]) >> 2;

  memcpy (&dest, &data[0], 2);
}

__forceinline  
void __fastcall ScalePixel (data24bit_t& dest, const data24bit_t& s1, const data24bit_t& s2, const data24bit_t& s3, const data24bit_t& s4)
{
  size_t data[4];

  memcpy (&data[0], &s1, 3);
  memcpy (&data[1], &s2, 3);
  memcpy (&data[2], &s3, 3);
  memcpy (&data[3], &s4, 3);

  data [0] = (data[0] + data[1] + data[2] + data[3]) >> 2;

  memcpy (&dest, &data[0], 3);
}

__forceinline  
void __fastcall ScalePixel (data32bit_t& dest, const data32bit_t& s1, const data32bit_t& s2, const data32bit_t& s3, const data32bit_t& s4)
{
  size_t data[4];

  memcpy (&data[0], &s1, 4);
  memcpy (&data[1], &s2, 4);
  memcpy (&data[2], &s3, 4);
  memcpy (&data[3], &s4, 4);

  data [0] = (data[0] + data[1] + data[2] + data[3]) >> 2;

  memcpy (&dest, &data[0], 4);
}

__forceinline 
void __fastcall  ScalePixel (two_color8_t& dest, two_color8_t s1, two_color8_t s2, two_color8_t s3, two_color8_t s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   >> 2;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) >> 2;
}

__forceinline 
void __fastcall  ScalePixel (rgb8_t& dest,const rgb8_t& s1,const rgb8_t& s2,const rgb8_t& s3,const rgb8_t& s4)
{
  dest.red   = (size_t)(s1.red   + s2.red   + s3.red   + s4.red)   >> 2;
  dest.green = (size_t)(s1.green + s2.green + s3.green + s4.green) >> 2;
  dest.blue  = (size_t)(s1.blue  + s2.blue  + s3.blue  + s4.blue ) >> 2;
}

__forceinline 
void __fastcall  ScalePixel (rgba8_t& dest,const rgba8_t& s1,const rgba8_t& s2,const rgba8_t& s3,const rgba8_t& s4)
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
void ScaleImage2XDownImpl (size_t width, size_t height, const T* src,T* dest)
{
  const T *s1 = src,
          *s2 = src + 1,
          *s3 = src + width,
          *s4 = s3  + 1; 

  size_t  w2  = width >> 1,
          h2  = height >> 1;

  for (size_t h = 0; h < h2; h++, s1 += width, s2 += width, s3 += width, s4 += width)
    for (size_t w = 0; w < w2; w++, s1 += 2, s2 += 2, s3 += 2, s4 += 2, dest++)
      ScalePixel (*dest,*s1,*s2,*s3,*s4);

  if (height == 1)
  {
    s3 = s1; s4 = s2;
    for (size_t w = 0; w < w2; w++, s1 += 2, s2 += 2, s3 += 2, s4 += 2, dest++)
      ScalePixel (*dest,*s1,*s2,*s3,*s4);
  }
  if (width == 1)
  {
    s3 = s1; s4 = s2;
    for (size_t h = 0; h < h2; h++, s1 += 2, s2 += 2, s3 += 2, s4 += 2, dest++)
      ScalePixel (*dest,*s1,*s2,*s3,*s4);
  }
}

void render::low_level::opengl::ScaleImage2XDown (PixelFormat format, size_t width, size_t height, const void* src, void* dest)
{
  switch (format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_S8:    ScaleImage2XDownImpl <uchar>        (width, height, (uchar*)       src, (uchar*)       dest); break;
    case PixelFormat_LA8:   ScaleImage2XDownImpl <two_color8_t> (width, height, (two_color8_t*)src, (two_color8_t*)dest); break;
    case PixelFormat_D16:   ScaleImage2XDownImpl <data16bit_t>  (width, height, (data16bit_t*) src, (data16bit_t*) dest); break;
    case PixelFormat_RGB8:  ScaleImage2XDownImpl <rgb8_t>       (width, height, (rgb8_t*)      src, (rgb8_t*)      dest); break;
    case PixelFormat_D24:   ScaleImage2XDownImpl <data24bit_t>  (width, height, (data24bit_t*) src, (data24bit_t*) dest); break;
    case PixelFormat_RGBA8: ScaleImage2XDownImpl <rgba8_t>      (width, height, (rgba8_t*)     src, (rgba8_t*)     dest); break;
    case PixelFormat_D32:   ScaleImage2XDownImpl <data32bit_t>  (width, height, (data32bit_t*) src, (data32bit_t*) dest); break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    default: return;
  }
}


#endif