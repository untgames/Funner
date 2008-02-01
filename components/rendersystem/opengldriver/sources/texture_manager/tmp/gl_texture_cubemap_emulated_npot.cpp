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
  x                     = (size_t)ceil ((float)x * scale_factor);
  y                     = (size_t)ceil ((float)y * scale_factor);    
  xtl::uninitialized_storage <char> scaled_buffer;

  if (is_compressed_format (source_format))
  {
    xtl::uninitialized_storage <char> unpacked_buffer (width * height * unpack_texel_size (source_format));
    scaled_buffer.resize (scaled_width * scaled_height * unpack_texel_size (source_format));

    unpack_dxt  (source_format, width, height, buffer, unpacked_buffer.data ());
    scale_image (unpack_pf (source_format), width, height, scaled_width, scaled_height, unpacked_buffer.data (), scaled_buffer.data ());
  }
  else
  {
    scaled_buffer.resize (scaled_width * scaled_height * texel_size (source_format));

    scale_image (source_format, width, height, scaled_width, scaled_height, buffer, scaled_buffer.data ());
  }

  try
  {
    if (is_compressed_format (source_format))  
      TextureCubemap::SetData (layer, mip_level, x, y, scaled_width, scaled_height, unpack_pf (source_format), scaled_buffer.data ());
    else
      TextureCubemap::SetData (layer, mip_level, x, y, scaled_width, scaled_height, source_format, scaled_buffer.data ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::TextureCubemapEmulatedNPOT::SetData");
    throw;
  }
}

void TextureCubemapEmulatedNPOT::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  size_t scaled_width   = (size_t)ceil ((float)width * scale_factor);
  size_t scaled_height  = (size_t)ceil ((float)height * scale_factor);
  xtl::uninitialized_storage <char> scaled_buffer;

  if (is_compressed_format (target_format))
    scaled_buffer.resize (((scaled_width * scaled_height) >> 4) * compressed_quad_size (target_format));
  else
    scaled_buffer.resize (scaled_width * scaled_height * texel_size (target_format));

  try
  {
    Texture::GetData (layer, mip_level, x, y, scaled_width, scaled_height, target_format, scaled_buffer.data ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::TextureCubemapEmulatedNPOT::GetData");
    throw;
  }

  if (is_compressed_format (target_format))
  {
    xtl::uninitialized_storage <char> unpacked_buffer (scaled_width * scaled_height * unpack_texel_size (target_format));

    unpack_dxt  (target_format, scaled_width, scaled_height, scaled_buffer.data (), unpacked_buffer.data ());
    scale_image (unpack_pf (target_format), scaled_width, scaled_height, width, height, unpacked_buffer.data (), buffer);
  }
  else   
    scale_image (target_format, scaled_width, scaled_height, width, height, scaled_buffer.data (), buffer);
}
