#include "shared.h"

using namespace common;
using namespace nv;

namespace
{

typedef unsigned char  uchar;

struct rgb8_t
{
  uchar red;
  uchar green;
  uchar blue;
};

}

/*
   Работа с DXT форматом при остутствии аппаратной поддержки
*/

namespace render
{

namespace low_level
{

namespace opengl
{

void unpack_dxt (PixelFormat format, size_t width, size_t height, const void* dxt_data, void* unpacked_data)
{
  switch (format)
  {
    case PixelFormat_DXT1: 
    {
      ColorBlock color_block;
      BlockDXT1  dxt_block, *dxt_data_pointer = (BlockDXT1*)dxt_data;
      rgb8_t     rgb8_pixel;

      for (size_t i = 0; i < height; i += 4)
        for (size_t j = 0; j < width; j += 4, dxt_data_pointer++)
        {
          dxt_block = *dxt_data_pointer;

          dxt_block.decodeBlock (&color_block);

          for (size_t k = 0; k < 4; k++)
            for (size_t l = 0; l < 4; l++)
            {
              rgb8_pixel.red   = color_block.color (l, k).r;
              rgb8_pixel.green = color_block.color (l, k).g;
              rgb8_pixel.blue  = color_block.color (l, k).b;
              ((rgb8_t*)unpacked_data) [(i + k) * width + j + l] = rgb8_pixel;
            }
        }

      break;
    }
    case PixelFormat_DXT3: 
    {
      ColorBlock color_block;
      BlockDXT3  dxt_block, *dxt_data_pointer = (BlockDXT3*)dxt_data;

      for (size_t i = 0; i < height; i += 4)
        for (size_t j = 0; j < width; j += 4, dxt_data_pointer++)
        {
          dxt_block = *dxt_data_pointer;

          dxt_block.decodeBlock (&color_block);

          for (size_t k = 0; k < 4; k++)
            for (size_t l = 0; l < 4; l++)
              ((Color32*)unpacked_data) [(i + k) * width + j + l] = color_block.color (l, k);
        }

      break;
    }
    case PixelFormat_DXT5:
    {
      ColorBlock color_block;
      BlockDXT5  dxt_block, *dxt_data_pointer = (BlockDXT5*)dxt_data;

      for (size_t i = 0; i < height; i += 4)
        for (size_t j = 0; j < width; j += 4, dxt_data_pointer++)
        {
          dxt_block = *dxt_data_pointer;

          dxt_block.decodeBlock (&color_block);

          for (size_t k = 0; k < 4; k++)
            for (size_t l = 0; l < 4; l++)
              ((Color32*)unpacked_data) [(i + k) * width + j + l] = color_block.color (l, k);
        }

      break;
    }
    default: throw xtl::make_argument_exception ("render::low_level::opengl::unpack_dxt", "format"); return;
  }
}

}

}

}
