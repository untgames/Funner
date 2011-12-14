#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::window_driver;

/*
    Конструктор
*/

RenderTarget::RenderTarget (render::low_level::IView* in_render_target_view, RenderTargetType in_type)
  : render_target_view (in_render_target_view)
  , type (in_type)
  , viewport_x (0)
  , viewport_y (0)
{
  switch (type)
  {
    case RenderTargetType_Color:
    case RenderTargetType_DepthStencil:
      break;
    default:
      throw xtl::make_argument_exception ("render::mid_level::window_driver::RenderTarget::RenderTarget", "type", type);
  }

  render::low_level::TextureDesc render_texture_desc;

  render_target_view->GetTexture ()->GetDesc (render_texture_desc);

  width  = render_texture_desc.width;
  height = render_texture_desc.height;
}

/*
    Копирование образа текстуры в картинку
*/

void RenderTarget::CaptureImage (media::Image& image)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::RenderTarget::CaptureImage";
  
  switch (type)
  {
    case RenderTargetType_Color:
    {
      render::low_level::ITexture*   render_target_texture (render_target_view->GetTexture ());
      render::low_level::TextureDesc render_texture_desc;

      render_target_texture->GetDesc (render_texture_desc);

      media::PixelFormat image_pixel_format;

      switch (render_texture_desc.format)
      {
        case render::low_level::PixelFormat_RGB8:  image_pixel_format = media::PixelFormat_RGB8;  break;
        case render::low_level::PixelFormat_RGBA8: image_pixel_format = media::PixelFormat_RGBA8; break;
        default:
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't capture render-targets with format %s", render::low_level::get_name (render_texture_desc.format));
      }

      media::Image (render_texture_desc.width, render_texture_desc.height, 1, image_pixel_format).Swap (image);

      render_target_texture->GetData (0, 0, 0, 0, render_texture_desc.width, render_texture_desc.height, render_texture_desc.format, image.Bitmap ());

      break;
    }
    case RenderTargetType_DepthStencil:
      throw xtl::format_not_supported_exception ("render::mid_level::window_driver::RenderTarget::CaptureImage",
        "Can't capture depth-stencil render-targets");
    default:
      break;
  }
}

/*
   Установка размера
*/

void RenderTarget::SetSize (size_t in_width, size_t in_height)
{
  width  = in_width;
  height = in_height;
}

/*
    Смещение для области вывода
*/

void RenderTarget::SetViewportOffset (int x, int y)
{
  viewport_x = x;
  viewport_y = y;
}
