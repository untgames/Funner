#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;
using namespace media;

/*
===================================================================================================
   Двумерная текстура
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

ImageTexture::ImageTexture (render::low_level::ITexture* in_texture)
  : texture (in_texture)
{
  if (!in_texture)
    throw xtl::make_null_argument_exception ("render::mid_level::low_level_driver::ImageTexture::ImageTexture", "texture");
}

/*
   Размеры текстуры
*/

size_t ImageTexture::GetWidth ()
{
  render::low_level::TextureDesc texture_desc;

  texture->GetDesc (texture_desc);

  return texture_desc.width;
}

size_t ImageTexture::GetHeight ()
{
  render::low_level::TextureDesc texture_desc;

  texture->GetDesc (texture_desc);

  return texture_desc.height;
}

/*
   Формат
*/

media::PixelFormat ImageTexture::GetFormat ()
{
  render::low_level::TextureDesc texture_desc;

  texture->GetDesc (texture_desc);

  switch (texture_desc.format)
  {
    case render::low_level::PixelFormat_RGB8:  return media::PixelFormat_RGB8;
    case render::low_level::PixelFormat_RGBA8: return media::PixelFormat_RGBA8;
    default:
      throw xtl::format_not_supported_exception ("render::mid_level::low_level_driver::ImageTexture::GetFormat", "Texture uses incompatible format %s", render::low_level::get_name (texture_desc.format));
  }
}

/*
   Копирование образа текстуры в картинку
*/

void ImageTexture::CaptureImage (media::Image& image)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::ImageTexture::CaptureImage";
  
  render::low_level::TextureDesc texture_desc;

  texture->GetDesc (texture_desc);

  media::Image (texture_desc.width, texture_desc.height, 1, GetFormat ()).Swap (image);

  texture->GetData (0, 0, 0, 0, texture_desc.width, texture_desc.height, texture_desc.format, image.Bitmap ());
}

/*
   Получение текстуры
*/

render::low_level::ITexture* ImageTexture::GetTexture ()
{
  return texture.get ();
}

/*
===================================================================================================
   Двумерная текстура с возможностью рендеринга
===================================================================================================
*/

/*
   Конструктор / деструктор
*/

RenderTargetTexture::RenderTargetTexture (render::low_level::IView* render_target_view)
  : RenderTarget (render_target_view, RenderTargetType_Color)
{
}
