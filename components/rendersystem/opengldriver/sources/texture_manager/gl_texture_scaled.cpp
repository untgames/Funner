#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

ScaledTexture::ScaledTexture (const TextureManager& texture_manager, const TextureDesc& original_desc, size_t scaled_width, size_t scaled_height)
{
  RaiseNotImplemented ("render::low_level::opengl::ScaledTexture::ScaledTexture");
  
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
  RaiseNotImplemented ("render::low_level::opengl::ScaledTexture::SetData");
}

void ScaledTexture::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  RaiseNotImplemented ("render::low_level::opengl::ScaledTexture::SetData");
}
