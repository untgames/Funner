#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::window_driver;
using namespace render::mid_level::window_driver::renderer2d;
using namespace media;

/*
===================================================================================================
    Утилиты
===================================================================================================
*/

namespace
{

render::low_level::PixelFormat get_pixel_format (media::PixelFormat format)
{
  switch (format)
  {
    case media::PixelFormat_RGB8:  return render::low_level::PixelFormat_RGB8;
    case media::PixelFormat_RGBA8: return render::low_level::PixelFormat_RGBA8;
    case media::PixelFormat_A8:
    case media::PixelFormat_L8:    return render::low_level::PixelFormat_A8;
    case media::PixelFormat_LA8:   return render::low_level::PixelFormat_LA8;
    default:
      throw xtl::format_not_supported_exception ("render::mid_level::window_driver::renderer2d::get_pixel_format",
        "Unsupported image format %s", media::get_format_name (format));
  }
}

}

/*
===================================================================================================
   Двумерная текстура
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

ImageTexture::ImageTexture (render::low_level::IDevice& device, const media::Image& image)
{
  try
  {
    TextureDesc tex_desc;

    memset (&tex_desc, 0, sizeof (tex_desc));

    tex_desc.dimension            = TextureDimension_2D;
    tex_desc.width                = image.Width ();
    tex_desc.height               = image.Height ();
    tex_desc.layers               = 1;
    tex_desc.format               = get_pixel_format (image.Format ());
    tex_desc.generate_mips_enable = true;
    tex_desc.bind_flags           = BindFlag_Texture;
    tex_desc.access_flags         = AccessFlag_ReadWrite;

    texture = TexturePtr (device.CreateTexture (tex_desc), false);

    texture->SetData (0, 0, 0, 0, tex_desc.width, tex_desc.height, tex_desc.format, image.Bitmap ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::ImageTexture::ImageTexture");
    throw;
  }
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
      throw xtl::format_not_supported_exception ("render::mid_level::window_driver::ImageTexture::GetFormat", "Texture uses incompatible format %s", render::low_level::get_name (texture_desc.format));
  }
}

/*
   Копирование образа текстуры в картинку
*/

void ImageTexture::CaptureImage (media::Image& image)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::ImageTexture::CaptureImage";
  
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

RenderTargetTexture::RenderTargetTexture (render::low_level::IDevice& device, size_t width, size_t height, media::PixelFormat format)
  : RenderTarget (CreateView (device, width, height, format).get (), RenderTargetType_Color)
{
}

/*
    Создание отображения
*/

RenderTargetTexture::ViewPtr RenderTargetTexture::CreateView
 (render::low_level::IDevice& device,
  size_t                      width,
  size_t                      height,
  media::PixelFormat          format)
{
  try
  {
      //срздание текстуры
    
    TextureDesc tex_desc;

    memset (&tex_desc, 0, sizeof (tex_desc));
    
    tex_desc.dimension            = TextureDimension_2D;
    tex_desc.width                = width;
    tex_desc.height               = height;
    tex_desc.layers               = 1;
    tex_desc.format               = get_pixel_format (format);
    tex_desc.generate_mips_enable = true;
    tex_desc.bind_flags           = BindFlag_Texture | BindFlag_RenderTarget;
    tex_desc.access_flags         = AccessFlag_ReadWrite;    
    
    typedef xtl::com_ptr<render::low_level::ITexture> TexturePtr;
    
    TexturePtr texture (device.CreateTexture (tex_desc), false);
    
      //создание отображения

    ViewDesc view_desc;

    memset (&view_desc, 0, sizeof (view_desc));

    return ViewPtr (device.CreateView (texture.get (), view_desc), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::renderer2d::RenderTargetTexture::CreateView");
    throw;
  }  
}
