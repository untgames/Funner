#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;

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
      throw xtl::format_operation_exception ("get_pixel_format", "Can't convert media format %s to render format.", media::get_format_name (format));
  }
}

}

/*
    Конструктор
*/

Renderer::Renderer (render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain)
  : BasicRenderer (device, swap_chain)
{
  try
  {
    common_resources = CommonResourcesPtr (new CommonResources (device), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level:low_level_driver::renderer2d::Renderer::Renderer");
    throw;
  }
}

/*
    Создание ресурсов
*/

render::mid_level::renderer2d::ITexture* Renderer::CreateTexture (const media::Image& image)
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

    xtl::com_ptr<render::low_level::ITexture> texture (device->CreateTexture (tex_desc), false);

    texture->SetData (0, 0, 0, 0, tex_desc.width, tex_desc.height, tex_desc.format, image.Bitmap ());

    return new ImageTexture (texture.get ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::low_level_driver::renderer2d::Renderer::CreateTexture(const media::Image&)");
    throw;
  }
}

render::mid_level::renderer2d::ITexture* Renderer::CreateTexture (size_t width, size_t height, media::PixelFormat pixel_format)
{
  try
  {
    TextureDesc tex_desc;

    memset (&tex_desc, 0, sizeof (tex_desc));
    
    tex_desc.dimension            = TextureDimension_2D;
    tex_desc.width                = width;
    tex_desc.height               = height;
    tex_desc.layers               = 1;
    tex_desc.format               = get_pixel_format (pixel_format);
    tex_desc.generate_mips_enable = true;
    tex_desc.bind_flags           = BindFlag_Texture | BindFlag_RenderTarget;
    tex_desc.access_flags         = AccessFlag_ReadWrite;    

    ViewDesc view_desc;
    
    memset (&view_desc, 0, sizeof (view_desc));
    
    return new RenderTargetTexture (device->CreateView (device->CreateTexture (tex_desc), view_desc));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::low_level_driver::renderer2d::Renderer::CreateTexture(size_t, size_t, media::PixelFormat)");
    throw;
  }
}

render::mid_level::renderer2d::IPrimitive* Renderer::CreatePrimitive ()
{
  return new Primitive;
}

render::mid_level::renderer2d::IFrame* Renderer::CreateFrame ()
{
  return new Frame (common_resources.get (), device.get ());
}
