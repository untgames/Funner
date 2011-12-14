#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

/*
    Конструктор / деструктор
*/

FrameBuffer::FrameBuffer ()
{
  color_buffer         = RenderTargetPtr (new RenderBuffer (SCREEN_WIDTH, SCREEN_HEIGHT, RenderTargetType_Color), false);
  depth_stencil_buffer = RenderTargetPtr (new RenderBuffer (SCREEN_WIDTH, SCREEN_HEIGHT, RenderTargetType_DepthStencil), false);
  
  log.Printf ("Create frame-buffer (id=%u)", Id ());
}

FrameBuffer::~FrameBuffer ()
{
  log.Printf ("Destroy frame-buffer (id=%u)", Id ());
}

/*
    Получение буфера цвета и буфера попиксельного отсечения
*/

IRenderTarget* FrameBuffer::GetColorBuffer ()
{
  return color_buffer.get ();
}

IRenderTarget* FrameBuffer::GetDepthStencilBuffer ()
{
  return depth_stencil_buffer.get ();
}
