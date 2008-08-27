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

void BasicFrame::BindViewport (render::low_level::IDevice* device)
{
  device->RSSetViewport (viewport);
}

/*
    Визуализация
*/

void BasicFrame::Draw (render::low_level::IDevice* device)
{
  using namespace render::low_level;

  if (!render_target && !depth_stencil_target)
    return;
    
    //установка целей отрисовки
    
  render::low_level::IView *render_target_view = render_target ? render_target->GetView () : 0,
                           *depth_stencil_view = depth_stencil_target ? depth_stencil_target->GetView () : 0;
                           
  device->OSSetRenderTargets (render_target_view, depth_stencil_view);  

    //собственно отрисовка

  DrawCore (device);
}
