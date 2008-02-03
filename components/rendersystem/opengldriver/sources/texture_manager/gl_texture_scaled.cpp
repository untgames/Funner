#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

ScaledTexture::ScaledTexture (TextureManager& texture_manager, const TextureDesc& original_desc, size_t scaled_width, size_t scaled_height)
  : original_width (original_desc.width), original_height (original_desc.height)
{
  if (!scaled_width)  scaled_width = next_higher_power_of_two (original_desc.width);
  if (!scaled_height) scaled_height = next_higher_power_of_two (original_desc.height);

  TextureDesc temp_desc = original_desc;

  temp_desc.width  = scaled_width;
  temp_desc.height = scaled_height;

  shadow_texture = reinterpret_cast <IBindableTexture*> (texture_manager.CreateTexture (temp_desc));
  
  horisontal_scale = (float)scaled_width  / (float)original_desc.width;
  vertical_scale   = (float)scaled_height / (float)original_desc.height;

//      LogPrintf ("Non power of two textures not supported by hardware. Scaled texture created.\n");  
}

/*
    Получение дескриптора
*/

void ScaledTexture::GetDesc (TextureDesc& out_desc)
{
  shadow_texture->GetDesc (out_desc);

  out_desc.width  = original_width;
  out_desc.height = original_height;
}

void ScaledTexture::GetDesc (BindableTextureDesc& out_desc)
{
  shadow_texture->GetDesc (out_desc);
}

/*
    Работа с данными
*/

void ScaledTexture::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  size_t scaled_width   = (size_t)ceil ((float)width * horisontal_scale);
  size_t scaled_height  = (size_t)ceil ((float)height * vertical_scale);
  x                     = (size_t)ceil ((float)x * horisontal_scale);
  y                     = (size_t)ceil ((float)y * vertical_scale);    
  xtl::uninitialized_storage <char> scaled_buffer;
  
  try
  {
    if (is_compressed_format (source_format))
    {
      xtl::uninitialized_storage <char> unpacked_buffer (width * height * unpack_texel_size (source_format));

      scaled_buffer.resize (scaled_width * scaled_height * unpack_texel_size (source_format));

      unpack_dxt  (source_format, width, height, buffer, unpacked_buffer.data ());
      scale_image (unpack_pf (source_format), width, height, scaled_width, scaled_height, unpacked_buffer.data (), scaled_buffer.data ());

      shadow_texture->SetData (layer, mip_level, x, y, scaled_width, scaled_height, unpack_pf (source_format), scaled_buffer.data ());
    }
    else
    {
      scaled_buffer.resize (scaled_width * scaled_height * texel_size (source_format));

      scale_image (source_format, width, height, scaled_width, scaled_height, buffer, scaled_buffer.data ());

      shadow_texture->SetData (layer, mip_level, x, y, scaled_width, scaled_height, source_format, scaled_buffer.data ());
    }  
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::ScaledTexture::SetData");
    throw;
  }
}

void ScaledTexture::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  const char* METHOD_NAME = "render::low_level::opengl::TextureEmulatedNPOT::GetData";

  if (is_compressed_format (target_format))
    RaiseNotSupported (METHOD_NAME, "Can't get data from scaled texture");

  size_t scaled_width   = (size_t)ceil ((float)width * horisontal_scale);
  size_t scaled_height  = (size_t)ceil ((float)height * vertical_scale);
  xtl::uninitialized_storage <char> scaled_buffer (scaled_width * scaled_height * texel_size (target_format));

  try
  {
    shadow_texture->GetData (layer, mip_level, x, y, scaled_width, scaled_height, target_format, scaled_buffer.data ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch (METHOD_NAME);
    throw;
  }

  scale_image (target_format, scaled_width, scaled_height, width, height, scaled_buffer.data (), buffer);
}
