#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::low_level_driver;

/*
    Конструктор
*/

ViewportFrame::ViewportFrame ()
{
  memset (&viewport, 0, sizeof viewport);
  
  viewport.min_depth = 0.f;
  viewport.max_depth = 1.f;
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
}

void ViewportFrame::GetViewport (render::mid_level::Viewport& out_viewport)
{
  out_viewport.x      = viewport.x;
  out_viewport.y      = viewport.y;
  out_viewport.width  = viewport.width;
  out_viewport.height = viewport.height;
}

/*
    Визуализация
*/

void ViewportFrame::DrawCore (render::low_level::IDevice* device)
{
    //установка вьюпорта

  device->RSSetViewport (viewport);
}
