#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::low_level_driver;

/*
    Конструктор
*/

ViewportFrame::ViewportFrame ()
  : need_update_normalized_viewport (true)
{
  viewport.min_depth = 0.f;
  viewport.max_depth = 1.f;
}

/*
    Установка целевых буферов отрисовки
*/

void ViewportFrame::SetRenderTargets (IRenderTarget* render_target, IRenderTarget* depth_stencil_target)
{
  BasicFrame::SetRenderTargets (render_target, depth_stencil_target);
  
  need_update_normalized_viewport = true;
}

/*
    Параметры области вывода
*/

void ViewportFrame::SetViewport (const render::mid_level::Viewport& in_viewport)
{
  viewport.x      = in_viewport.x;
  viewport.y      = in_viewport.y;
  viewport.width  = in_viewport.width;
  viewport.height = in_viewport.height;
  
  need_update_normalized_viewport = true;
}

void ViewportFrame::GetViewport (render::mid_level::Viewport& out_viewport)
{
  out_viewport.x      = viewport.x;
  out_viewport.y      = viewport.y;
  out_viewport.width  = viewport.width;
  out_viewport.height = viewport.height;
}

/*
    Обновление нормированной области вывода
*/

void ViewportFrame::UpdateNormalizedViewport ()
{
  IRenderTarget* screen = BasicFrame::GetRenderTarget ();
  
  if (!screen)
    screen = BasicFrame::GetDepthStencilTarget ();

  if (!screen)
  {
    normalized_viewport_left   = 0.0f;
    normalized_viewport_top    = 0.0f;
    normalized_viewport_width  = 1.0f;
    normalized_viewport_height = 1.0f;
  }
  else
  {
    size_t screen_width = screen->GetWidth (), screen_height = screen->GetHeight ();
    
    normalized_viewport_left   = float (viewport.x) / screen_width;
    normalized_viewport_top    = float (viewport.y) / screen_height;
    normalized_viewport_width  = float (viewport.width) / screen_width;
    normalized_viewport_height = float (viewport.height) / screen_height;
  }

  need_update_normalized_viewport = false;
}

/*
    Визуализация
*/

void ViewportFrame::SetDeviceViewportCore (render::low_level::IDevice* device, const render::low_level::Rect& device_viewport)
{
  using namespace render::low_level;

    //обновление нормированной области вывода

  if (need_update_normalized_viewport)
    UpdateNormalizedViewport ();

    //установка вьюпорта

  render::low_level::Viewport rs_viewport;

  rs_viewport.x         = int (normalized_viewport_left * device_viewport.width + device_viewport.x);
  rs_viewport.y         = int (normalized_viewport_top * device_viewport.height + device_viewport.y);
  rs_viewport.width     = size_t (normalized_viewport_width * device_viewport.width);
  rs_viewport.height    = size_t (normalized_viewport_height * device_viewport.height);
  rs_viewport.min_depth = viewport.min_depth;
  rs_viewport.max_depth = viewport.max_depth;

  device->RSSetViewport (rs_viewport);
}
