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
      throw xtl::format_not_supported_exception ("render::mid_level::window_driver::renderer2d::get_pixel_format(media::PixelFormat)",
        "Unsupported image format %s", media::get_format_name (format));
  }
}

render::low_level::PixelFormat get_pixel_format (render::mid_level::PixelFormat format)
{
  switch (format)
  {
    case render::mid_level::PixelFormat_RGB8:  return render::low_level::PixelFormat_RGB8;
    case render::mid_level::PixelFormat_RGBA8: return render::low_level::PixelFormat_RGBA8;
    case render::mid_level::PixelFormat_A8:
    case render::mid_level::PixelFormat_L8:    return render::low_level::PixelFormat_A8;
    case render::mid_level::PixelFormat_LA8:   return render::low_level::PixelFormat_LA8;
    default:
      throw xtl::make_argument_exception ("render::mid_level::window_driver::renderer2d::get_pixel_format(render::mid_level::PixelFormat)",
        "format", format);
  }
}

render::low_level::PixelFormat get_pixel_format (const char* name)
{
  if      (!strcmp (name, "rgb_pvrtc2"))  return PixelFormat_RGB_PVRTC2;
  else if (!strcmp (name, "rgb_pvrtc4"))  return PixelFormat_RGB_PVRTC4;
  else if (!strcmp (name, "rgba_pvrtc2")) return PixelFormat_RGBA_PVRTC2;
  else if (!strcmp (name, "rgba_pvrtc4")) return PixelFormat_RGBA_PVRTC4;
  else if (!strcmp (name, "dxt1"))        return PixelFormat_DXT1;
  else if (!strcmp (name, "dxt3"))        return PixelFormat_DXT3;
  else if (!strcmp (name, "dxt5"))        return PixelFormat_DXT5;

  throw xtl::format_not_supported_exception ("render::mid_level::window_driver::renderer2d::get_pixel_format", "Unsupported compression format '%s'", name);
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

ImageTexture::ImageTexture (render::low_level::IDevice& device, const media::CompressedImage& image)
{
  try
  {
    memset (&tex_desc, 0, sizeof (tex_desc));

    tex_desc.dimension            = TextureDimension_2D;
    tex_desc.width                = image.Width ();
    tex_desc.height               = image.Height ();
    tex_desc.layers               = 1;
    tex_desc.format               = get_pixel_format (image.Format ());
    tex_desc.generate_mips_enable = false;
    tex_desc.bind_flags           = BindFlag_Texture;
    tex_desc.access_flags         = AccessFlag_ReadWrite;
    
    stl::vector<size_t> sizes (image.BlocksCount ());
    const media::CompressedImageBlockDesc* blocks = image.Blocks ();

    for (size_t i=0, count=sizes.size (); i<count; i++)
      sizes [i] = blocks [i].size;

    TextureData data;

    memset (&data, 0, sizeof (TextureData));

    data.data  = image.Data ();
    data.sizes = &sizes [0];

    texture = TexturePtr (device.CreateTexture (tex_desc, data), false);
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
   Копирование образа текстуры в картинку
*/

void ImageTexture::CaptureImage (media::Image& image)
{
  try
  {  
    render::low_level::TextureDesc texture_desc;

    texture->GetDesc (texture_desc);    
    
    media::PixelFormat image_format;

    switch (texture_desc.format)
    {
      case render::low_level::PixelFormat_RGB8:
        image_format = media::PixelFormat_RGB8;
        break;
      case render::low_level::PixelFormat_RGBA8:
        image_format = media::PixelFormat_RGBA8;
        break;
      default:
        throw xtl::format_not_supported_exception ("render::mid_level::window_driver::ImageTexture::CaptureImage(media::Image&)",
          "Texture uses incompatible format %s", render::low_level::get_name (texture_desc.format));
    }    

    media::Image (texture_desc.width, texture_desc.height, 1, image_format).Swap (image);

    texture->GetData (0, 0, 0, 0, texture_desc.width, texture_desc.height, texture_desc.format, image.Bitmap ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::renderer2d::ImageTexture::CaptureImage");
    throw;
  }
}

/*
    Обновление текстуры
*/

void ImageTexture::Update (media::Image& image)
{
  try
  {
    if (image.Width () != tex_desc.width)
      throw xtl::format_operation_exception ("", "Image width %u mismatch texture width %u", image.Width (), tex_desc.width);
      
    if (image.Height () != tex_desc.height)
      throw xtl::format_operation_exception ("", "Image height %u mismatch texture height %u", image.Height (), tex_desc.height);
      
    if (tex_desc.format != get_pixel_format (image.Format ()))
      throw xtl::format_operation_exception ("", "Image format %s mismatch texture format %s", get_name (get_pixel_format (image.Format ())), get_name (tex_desc.format));
    
    texture->SetData (0, 0, 0, 0, tex_desc.width, tex_desc.height, tex_desc.format, image.Bitmap ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::window_driver::renderer2d::ImageTexture::Update");
    throw;
  }
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

RenderTargetTexture::RenderTargetTexture (render::low_level::IDevice& device, size_t width, size_t height, render::mid_level::PixelFormat format)
  : RenderTarget (CreateView (device, width, height, format).get (), RenderTargetType_Color)
{
}

/*
    Создание отображения
*/

RenderTargetTexture::ViewPtr RenderTargetTexture::CreateView
 (render::low_level::IDevice&   device,
  size_t                         width,
  size_t                         height,
  render::mid_level::PixelFormat format)
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

/*
    Обновление текстуры
*/

void RenderTargetTexture::Update (media::Image&)
{
  throw xtl::format_not_supported_exception ("render::mid_level::window_driver::renderer2d::RenderTargetTexture::Update",
    "Render target textures can't be updated from image");
}
