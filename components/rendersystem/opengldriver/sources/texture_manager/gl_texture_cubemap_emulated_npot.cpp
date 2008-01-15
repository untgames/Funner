#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

TextureCubemapEmulatedNPOT::TextureCubemapEmulatedNPOT  (const ContextManager& manager, const TextureDesc& tex_desc, float in_scale_factor)
  : TextureCubemap (manager, tex_desc), scale_factor (in_scale_factor)
  {}

/*
   Работа с данными
*/

void TextureCubemapEmulatedNPOT::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  size_t scaled_width   = (size_t)ceil ((float)width * scale_factor);
  size_t scaled_height  = (size_t)ceil ((float)height * scale_factor);
  char*  scaled_buffer  = new char [scaled_width * scaled_height * texel_size (source_format)];

  scale_image (source_format, width, height, scaled_width, scaled_height, buffer, scaled_buffer);

  try
  {
    TextureCubemap::SetData (layer, mip_level, x, y, scaled_width, scaled_height, source_format, scaled_buffer);
  }
  catch (common::Exception& exception)
  {
    delete [] scaled_buffer;
    exception.Touch ("render::low_level::opengl::TextureCubemapEmulatedNPOT::SetData");
    throw;
  }

  delete [] scaled_buffer;
}

void TextureCubemapEmulatedNPOT::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  size_t scaled_width   = (size_t)ceil ((float)width * scale_factor);
  size_t scaled_height  = (size_t)ceil ((float)height * scale_factor);
  char*  scaled_buffer  = new char [scaled_width * scaled_height * texel_size (target_format)];

  try
  {
    TextureCubemap::GetData (layer, mip_level, x, y, scaled_width, scaled_height, target_format, scaled_buffer);
  }
  catch (common::Exception& exception)
  {
    delete [] scaled_buffer;
    exception.Touch ("render::low_level::opengl::TextureCubemapEmulatedNPOT::GetData");
    throw;
  }

  scale_image (target_format, scaled_width, scaled_height, width, height, scaled_buffer, buffer);

  delete [] scaled_buffer;
}
