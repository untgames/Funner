#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

TextureEmulatedNPOT::TextureEmulatedNPOT  (const ContextManager& manager, const TextureDesc& tex_desc, float in_horisontal_scale, float in_vertical_scale)
  : Texture2D (manager, tex_desc), horisontal_scale (in_vertical_scale), vertical_scale (in_vertical_scale)
  {}

/*
   Работа с данными
*/

void TextureEmulatedNPOT::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  if (is_compressed_format (source_format))
    Raise <Exception> ("render::low_level::opengl::TextureEmulatedNPOT::SetData", "Compression in emulated npot textures not implemented yet.");

  size_t scaled_width   = (size_t)ceil ((float)width * horisontal_scale);
  size_t scaled_height  = (size_t)ceil ((float)height * vertical_scale);
  char*  scaled_buffer  = new char [scaled_width * scaled_height * texel_size (source_format)];

  scale_image (source_format, width, height, scaled_width, scaled_height, buffer, scaled_buffer);

  try
  {
    Texture2D::SetData (layer, mip_level, x, y, scaled_width, scaled_height, source_format, scaled_buffer);
  }
  catch (common::Exception& exception)
  {
    delete [] scaled_buffer;
    exception.Touch ("render::low_level::opengl::TextureEmulatedNPOT::SetData");
    throw;
  }

  delete [] scaled_buffer;
}

void TextureEmulatedNPOT::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  if (is_compressed_format (target_format))
    Raise <Exception> ("render::low_level::opengl::TextureEmulatedNPOT::GetData", "Compression in emulated npot textures not implemented yet.");

  size_t scaled_width   = (size_t)ceil ((float)width * horisontal_scale);
  size_t scaled_height  = (size_t)ceil ((float)height * vertical_scale);
  char*  scaled_buffer  = new char [scaled_width * scaled_height * texel_size (target_format)];

  try
  {
    Texture2D::GetData (layer, mip_level, x, y, scaled_width, scaled_height, target_format, scaled_buffer);
  }
  catch (common::Exception& exception)
  {
    delete [] scaled_buffer;
    exception.Touch ("render::low_level::opengl::TextureEmulatedNPOT::GetData");
    throw;
  }

  scale_image (target_format, scaled_width, scaled_height, width, height, scaled_buffer, buffer);

  delete [] scaled_buffer;
}
