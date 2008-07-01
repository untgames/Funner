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
  log.Printf ("Capture image from render-target #%u", Id ());
  
  media::Image ().Swap (image);
}
