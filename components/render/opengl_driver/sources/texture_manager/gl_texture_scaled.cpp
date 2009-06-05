#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

ScaledTexture::ScaledTexture
 (const ContextManager& context_manager,
  TextureManager&       texture_manager,
  const TextureDesc&    original_desc,
  size_t                scaled_width,
  size_t                scaled_height)
    : BindableTexture (context_manager),
      original_width (original_desc.width),
      original_height (original_desc.height)
{
  const ContextCaps& caps = GetCaps ();

  if (!scaled_width)
  {
    if (original_desc.width > caps.max_texture_size)
      scaled_width = caps.max_texture_size;
    else
      scaled_width = get_next_higher_power_of_two (original_desc.width);
  }
  if (!scaled_height)
  {
    if (original_desc.height > caps.max_texture_size)
      scaled_height = caps.max_texture_size;
    else
      scaled_height = get_next_higher_power_of_two (original_desc.height);
  }

  TextureDesc temp_desc = original_desc;

  temp_desc.width  = scaled_width;
  temp_desc.height = scaled_height;

  shadow_texture = TexturePtr (dynamic_cast<BindableTexture*> (texture_manager.CreateTexture (temp_desc)), false);

  if (!shadow_texture.get ())
    throw xtl::format_operation_exception ("render::low_level::opengl::ScaledTexture::ScaledTexture", "TextureManager::CreateTexture returned texture with incompatible type");

  horisontal_scale = (float)scaled_width  / (float)original_desc.width;
  vertical_scale   = (float)scaled_height / (float)original_desc.height;

  LogPrintf ("Warning: scaled texture %ux%ux%u has created for original texture %ux%ux%u@%s", scaled_width, scaled_height, temp_desc.layers,
             original_desc.width, original_desc.height, original_desc.layers, get_name (original_desc.format));
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

void ScaledTexture::GetDesc (RenderTargetTextureDesc& out_desc)
{
  shadow_texture->GetDesc (out_desc);
}

void ScaledTexture::GetMipLevelDesc (size_t level, MipLevelDesc& out_desc)
{
  shadow_texture->GetMipLevelDesc (level, out_desc);
}

PixelFormat ScaledTexture::GetFormat ()
{
  return shadow_texture->GetFormat ();
}

/*
    Установка / получение хэша дескриптора прикрепленного сэмплера
*/

void ScaledTexture::SetSamplerHash (size_t hash)
{
  shadow_texture->SetSamplerHash (hash);
}

size_t ScaledTexture::GetSamplerHash ()
{
  return shadow_texture->GetSamplerHash ();
}

/*
    Установка текстуры в контекст OpenGL
*/

void ScaledTexture::Bind ()
{
  shadow_texture->Bind ();
}

/*
    Работа с данными
*/

void ScaledTexture::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  size_t scaled_width  = (size_t)ceil ((float)width * horisontal_scale),
         scaled_height = (size_t)ceil ((float)height * vertical_scale);

  x = (size_t)ceil ((float)x * horisontal_scale);
  y = (size_t)ceil ((float)y * vertical_scale);

  xtl::uninitialized_storage <char> scaled_buffer;

  try
  {
    if (is_compressed (source_format))
    {
      xtl::uninitialized_storage <char> unpacked_buffer (get_uncompressed_image_size (width, height, source_format));

      scaled_buffer.resize (get_uncompressed_image_size (scaled_width, scaled_height, source_format));

      unpack_dxt  (source_format, width, height, buffer, unpacked_buffer.data ());

      PixelFormat uncompressed_format = get_uncompressed_format (source_format);

      scale_image (uncompressed_format, width, height, scaled_width, scaled_height, unpacked_buffer.data (), scaled_buffer.data ());

      shadow_texture->SetData (layer, mip_level, x, y, scaled_width, scaled_height, uncompressed_format, scaled_buffer.data ());
    }
    else
    {
      scaled_buffer.resize (get_image_size (scaled_width, scaled_height, source_format));

      scale_image (source_format, width, height, scaled_width, scaled_height, buffer, scaled_buffer.data ());

      shadow_texture->SetData (layer, mip_level, x, y, scaled_width, scaled_height, source_format, scaled_buffer.data ());
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::ScaledTexture::SetData");
    throw;
  }
}

void ScaledTexture::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  const char* METHOD_NAME = "render::low_level::opengl::TextureEmulatedNPOT::GetData";

  if (is_compressed (target_format))
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't get data in format %s from scaled texture", get_name (target_format));

  size_t scaled_width  = (size_t)ceil ((float)width * horisontal_scale),
         scaled_height = (size_t)ceil ((float)height * vertical_scale);

  xtl::uninitialized_storage <char> scaled_buffer (get_image_size (scaled_width, scaled_height, target_format));

  try
  {
    shadow_texture->GetData (layer, mip_level, x, y, scaled_width, scaled_height, target_format, scaled_buffer.data ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);
    throw;
  }

  scale_image (target_format, scaled_width, scaled_height, width, height, scaled_buffer.data (), buffer);
}
