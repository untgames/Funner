#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

/*
    Конструктор
*/

RenderTarget::RenderTarget (size_t in_width, size_t in_height, RenderTargetType in_type)
  : width (in_width), height (in_height), type (in_type)
{
  switch (type)
  {
    case RenderTargetType_Color:
    case RenderTargetType_DepthStencil:
      break;
    default:
      throw xtl::make_argument_exception ("render::mid_level::debug::RenderTarget::RenderTarget", "type", type);
  }
}

/*
    Размеры буфера
*/

size_t RenderTarget::GetWidth ()
{
  return width;
}

size_t RenderTarget::GetHeight ()
{
  return height;
}

/*
    Копирование образа текстуры в картинку
*/

void RenderTarget::CaptureImage (media::Image& image)
{
  switch (type)
  {
    case RenderTargetType_Color:
      log.Printf ("Capture image from render-target #%u", Id ());
      media::Image (width, height, 1, media::PixelFormat_RGBA8).Swap (image);
      break;
    case RenderTargetType_DepthStencil:
      throw xtl::format_not_supported_exception ("render::mid_level::debug::RenderTarget::CaptureImage",
        "Can't capture depth-stencil render-targets");
    default:
      break;
  }
}

/*
    Буфер рендеринга
*/

RenderBuffer::RenderBuffer (size_t width, size_t height, RenderTargetType type)
  : RenderTarget (width, height, type)
{
  log.Printf ("Create %s %ux%u (id=%u)", type == RenderTargetType_Color ? "color-buffer" : "depth-stencil-buffer", width, height, Id ());
}
    
RenderBuffer::~RenderBuffer ()
{
  try
  {
    log.Printf ("Destroy %s (id=%u)", GetType () == RenderTargetType_Color ? "color-buffer" : "depth-stencil-buffer", Id ());
  }
  catch (...)
  {
    //подавление всех исключений
  }      
}
