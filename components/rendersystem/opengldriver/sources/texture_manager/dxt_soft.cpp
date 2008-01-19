#include "shared.h"
#include "nvidia_texture_tools/BlockDXT.h"
#include "nvidia_texture_tools/FastCompressDXT.h"
#include "nvidia_texture_tools/squish/colourset.h"
#include "nvidia_texture_tools/squish/fastclusterfit.h"
#include "nvidia_texture_tools/squish/weightedclusterfit.h"

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

namespace nv
{

void doPrecomputation()
{
  static bool done = false;
  
  if (!done)
  {
    done = true;
    squish::FastClusterFit::doPrecomputation();
  }
}

}

/*
   Работа с DXT форматом при остутствии аппаратной поддержки
*/

void render::low_level::opengl::unpack_dxt (PixelFormat format, size_t width, size_t height, const void* dxt_data, void* unpacked_data)
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
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_dxt", "format"); return;
  }
}

void render::low_level::opengl::pack_dxt (PixelFormat format, size_t width, size_t height, const void* unpacked_data, void* dxt_data)
{
  switch (format)
  {
    case PixelFormat_DXT1: 
    {
      ColorBlock rgba;
      BlockDXT1  block, *dxt_data_pointer = (BlockDXT1*) dxt_data;

      doPrecomputation();

      for (size_t i = 0; i < height; i += 4)
        for (size_t j = 0; j < width; j += 4, dxt_data_pointer++) 
        {          
          for (size_t k = 0; k < 4; k++)
            for (size_t l = 0; l < 4; l++)
            {
              rgba.color (l, k).r = ((rgb8_t*)unpacked_data)[(i + k) * width + j + l].red;
              rgba.color (l, k).g = ((rgb8_t*)unpacked_data)[(i + k) * width + j + l].green;
              rgba.color (l, k).b = ((rgb8_t*)unpacked_data)[(i + k) * width + j + l].blue;
            }

          // Compress color.
          squish::ColourSet colours((uint8 *)rgba.colors(), 0);
          squish::FastClusterFit fit(&colours, squish::kDxt1);
          //squish::WeightedClusterFit fit(&colours, squish::kDxt1);
          //squish::ClusterFit fit(&colours, squish::kDxt1);
          fit.setMetric(1.f, 1.f, 1.f);
          fit.Compress(&block);
          
          *dxt_data_pointer = block;
        }

      break;
    }
    case PixelFormat_DXT3: 
    {
      ColorBlock rgba;
      BlockDXT3  block, *dxt_data_pointer = (BlockDXT3*) dxt_data;

      doPrecomputation();

      for (size_t i = 0; i < height; i += 4)
        for (size_t j = 0; j < width; j += 4, dxt_data_pointer++) 
        {          
          for (size_t k = 0; k < 4; k++)
            for (size_t l = 0; l < 4; l++)
              rgba.color (l, k) = ((Color32*)unpacked_data)[(i + k) * width + j + l];
          
          // Compress explicit alpha.
          compressBlock(rgba, &block.alpha);

          // Compress color.
          squish::ColourSet colours((uint8 *)rgba.colors(), squish::kWeightColourByAlpha);
          squish::WeightedClusterFit fit(&colours, 0);
          //squish::WeightedClusterFit fit(&colours, squish::kDxt1);
          //squish::ClusterFit fit(&colours, squish::kDxt1);
          fit.setMetric(1.f, 1.f, 1.f);
          fit.Compress(&block);
          
          *dxt_data_pointer = block;
        }

      break;
    }
    case PixelFormat_DXT5:
    {
      ColorBlock rgba;
      BlockDXT5  block, *dxt_data_pointer = (BlockDXT5*) dxt_data;

      doPrecomputation();

      for (size_t i = 0; i < height; i += 4)
        for (size_t j = 0; j < width; j += 4, dxt_data_pointer++) 
        {          
          for (size_t k = 0; k < 4; k++)
            for (size_t l = 0; l < 4; l++)
              rgba.color (l, k) = ((Color32*)unpacked_data)[(i + k) * width + j + l];
          
          // Compress alpha.
          compressBlock_Iterative(rgba, &block.alpha);

          // Compress color.
          squish::ColourSet colours((uint8 *)rgba.colors(), squish::kWeightColourByAlpha);
          squish::WeightedClusterFit fit(&colours, 0);
          //squish::WeightedClusterFit fit(&colours, squish::kDxt1);
          //squish::ClusterFit fit(&colours, squish::kDxt1);
          fit.setMetric(1.f, 1.f, 1.f);
          fit.Compress(&block);
          
          *dxt_data_pointer = block;
        }

      break;
    }
    default: common::RaiseInvalidArgument ("render::low_level::opengl::pack_dxt", "format"); return;
  }
}

GLint render::low_level::opengl::unpack_internal_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1: return GL_RGB8;
    case PixelFormat_DXT3: 
    case PixelFormat_DXT5: return GL_RGBA8;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_internal_format", "format"); return 1;
  }
}

GLenum render::low_level::opengl::unpack_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1: return GL_RGB;
    case PixelFormat_DXT3:
    case PixelFormat_DXT5: return GL_RGBA;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_format", "format"); return 1;
  }
}

render::low_level::PixelFormat render::low_level::opengl::unpack_pf (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1: return PixelFormat_RGB8;
    case PixelFormat_DXT3:
    case PixelFormat_DXT5: return PixelFormat_RGBA8;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_format", "format"); return PixelFormat_RGB8;
  }
}

GLenum render::low_level::opengl::unpack_type (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5: return GL_UNSIGNED_BYTE;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_type", "format"); return 1;
  }
}

size_t render::low_level::opengl::unpack_texel_size (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1: return 3;
    case PixelFormat_DXT3:
    case PixelFormat_DXT5: return 4;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::unpack_texel_size", "format"); return 1;
  }
}
