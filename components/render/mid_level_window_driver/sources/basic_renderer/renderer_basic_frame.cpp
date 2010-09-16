#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::window_driver;

/*
    Конструктор
*/

BasicFrame::BasicFrame ()
{
  memset (&viewport, 0, sizeof viewport);

  viewport.min_depth = 0.f;
  viewport.max_depth = 1.f;
}

/*
    Целевые буферы отрисовки
*/

void BasicFrame::SetRenderTargets (IRenderTarget* in_render_target, IRenderTarget* in_depth_stencil_target)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::BasicFrame::SetRenderTargets";

  if (in_render_target)
  {
    RenderTarget* casted_render_target = dynamic_cast<RenderTarget*> (in_render_target);
    
    if (!casted_render_target)
      throw xtl::make_argument_exception (METHOD_NAME, "render_target", typeid (in_render_target).name (),
        "Render target type incompatible with render::mid_level::window_driver::RenderTarget");
        
    render_target = casted_render_target;
  }
  else render_target = 0;
  
  if (in_depth_stencil_target)
  {
    RenderTarget* casted_depth_stencil_target = dynamic_cast<RenderTarget*> (in_depth_stencil_target);
    
    if (!casted_depth_stencil_target)
      throw xtl::make_argument_exception (METHOD_NAME, "depth_stencil_target", typeid (in_depth_stencil_target).name (),
        "Depth-stencil target type incompatible with render::mid_level::window_driver::RenderTarget");
        
    depth_stencil_target = casted_depth_stencil_target;
  }
  else depth_stencil_target = 0;  
}

//целевой буфер цвета
IRenderTarget* BasicFrame::GetRenderTarget ()
{
  return render_target.get ();
}

//целевой буфер попиксельного отсечения
IRenderTarget* BasicFrame::GetDepthStencilTarget ()
{
  return depth_stencil_target.get ();
}

/*
    Параметры области вывода
*/

void BasicFrame::SetViewport (const render::mid_level::Viewport& in_viewport)
{
  viewport.x      = in_viewport.x;
  viewport.y      = in_viewport.y;
  viewport.width  = in_viewport.width;
  viewport.height = in_viewport.height;
}

void BasicFrame::GetViewport (render::mid_level::Viewport& out_viewport)
{
  out_viewport.x      = viewport.x;
  out_viewport.y      = viewport.y;
  out_viewport.width  = viewport.width;
  out_viewport.height = viewport.height;
}

/*
    Установка области вывода
*/

void BasicFrame::BindViewport (render::low_level::IDevice* device, int& viewport_offset_x, int& viewport_offset_y)
{
  if (render_target && depth_stencil_target && (render_target->ViewportX () != depth_stencil_target->ViewportX () ||
    render_target->ViewportY () != depth_stencil_target->ViewportY ()))
  {
    throw xtl::format_operation_exception ("render::mid_level::window_driver::BasicFrame::BindViewport", "Different viewport offsets for render target and depth-stencil target");
  }
  
  render::low_level::Viewport fixed_viewport = viewport;  

  viewport_offset_x = render_target->ViewportX ();
  viewport_offset_y = render_target->ViewportY ();
  fixed_viewport.x += viewport_offset_x;
  fixed_viewport.y += viewport_offset_y; 

  device->RSSetViewport (fixed_viewport);
}

/*
    Визуализация
*/

void BasicFrame::Draw (render::low_level::IDevice* device, size_t& draw_frames_count)
{
  using namespace render::low_level;

  if (!render_target && !depth_stencil_target)
    return;
    
    //установка целей отрисовки
    
  render::low_level::IView *render_target_view        = render_target ? render_target->GetView () : 0,
                           *depth_stencil_view        = depth_stencil_target ? depth_stencil_target->GetView () : 0,
                           *device_render_target_view = device->OSGetRenderTargetView (),
                           *device_depth_stencil_view = device->OSGetDepthStencilView ();

  if (render_target_view != device_render_target_view || depth_stencil_view != device_depth_stencil_view)
  {                           
    if (draw_frames_count && (device_render_target_view || device_depth_stencil_view))
      device->Flush ();

    device->OSSetRenderTargets (render_target_view, depth_stencil_view);
  }  

    //собственно отрисовка

  DrawCore (device);
  
    //увеличение числа количества нарисованных кадров
  
  draw_frames_count++;  
}
