#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;

/*
    Конструктор
*/

BasicFrame::BasicFrame ()
{
  memset (&viewport, 0, sizeof viewport);
}

/*
    Целевые буферы отрисовки
*/

void BasicFrame::SetRenderTargets (IRenderTarget* in_render_target, IRenderTarget* in_depth_stencil_target)
{
  static const char* METHOD_NAME = "render::mid_level::debug::BasicFrame::SetRenderTargets";

  if (in_render_target)
  {
    RenderTarget* casted_render_target = dynamic_cast<RenderTarget*> (in_render_target);
    
    if (!casted_render_target)
      throw xtl::make_argument_exception (METHOD_NAME, "render_target", typeid (in_render_target).name (),
        "Render target type incompatible with render::mid_level::debug::RenderTarget");
        
    render_target = casted_render_target;
  }
  else render_target = 0;
  
  if (in_depth_stencil_target)
  {
    RenderTarget* casted_depth_stencil_target = dynamic_cast<RenderTarget*> (in_depth_stencil_target);
    
    if (!casted_depth_stencil_target)
      throw xtl::make_argument_exception (METHOD_NAME, "depth_stencil_target", typeid (in_depth_stencil_target).name (),
        "Depth-stencil target type incompatible with render::mid_level::debug::RenderTarget");
        
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

void BasicFrame::SetViewport (const Viewport& in_viewport)
{
  viewport = in_viewport;
}

void BasicFrame::GetViewport (Viewport& out_viewport)
{
  out_viewport = viewport;
}

/*
    Визуализация
*/

void BasicFrame::Draw ()
{
    //начало кадра

  log.Printf ("Begin draw frame (id=%u)", Id ());
  
    //установка целевых буферов отрисовки
    
  if (render_target && depth_stencil_target)
  {
    log.Printf ("Set render targets: render-target=%u, depth-stencil-target=%u", render_target->Id (), depth_stencil_target->Id ());
  }
  else if (!render_target && depth_stencil_target)
  {
    log.Printf ("Set render targets: render-target=none, depth-stencil-target=%u", depth_stencil_target->Id ());    
  }  
  else if (render_target && !depth_stencil_target)
  {
    log.Printf ("Set render targets: render-target=%u, depth-stencil-target=none", render_target->Id ());
  }
  else
  {
    log.Printf ("Set render targets: render-target=none, depth-stencil-target=none");
  }

    //собственно отрисовка

  DrawCore ();

    //конец кадра

  log.Printf ("End draw frame (id=%u)", Id ());  
}
