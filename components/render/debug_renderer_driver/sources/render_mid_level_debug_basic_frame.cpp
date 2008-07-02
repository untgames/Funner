#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::debug;

/*
    Конструктор
*/

BasicFrame::BasicFrame ()
  : need_clear_render_target (false),
    need_clear_depth_stencil_target (false),
    clear_depth_value (0.0f),
    clear_stencil_index (0)
{
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
    Параметры очистки целевых буферов
*/

void BasicFrame::SetClearBuffers (bool clear_render_target, bool clear_depth_stencil_target)
{
  need_clear_render_target        = clear_render_target;
  need_clear_depth_stencil_target = clear_depth_stencil_target;
}

void BasicFrame::GetClearBuffers (bool& clear_render_target, bool& clear_depth_stencil_target)
{
  clear_render_target        = need_clear_render_target;
  clear_depth_stencil_target = need_clear_depth_stencil_target;  
}

void BasicFrame::SetClearColor (const math::vec4f& color)
{
  clear_color = color;
}

void BasicFrame::SetClearDepthStencil (float depth_value, unsigned char stencil_index)
{
  clear_depth_value   = depth_value;
  clear_stencil_index = stencil_index;
}  

void BasicFrame::GetClearColor (math::vec4f& color)
{
  color = clear_color;
}

void BasicFrame::GetClearDepthStencil (float& depth_value, unsigned char& stencil_index)
{
  depth_value   = clear_depth_value;
  stencil_index = clear_stencil_index;
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

    //очистка целевых буферов отрисовки  

  if (need_clear_render_target && render_target)
    log.Printf ("Clear render target [%.2f %.2f %.2f %.2f]", clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    
  if (need_clear_depth_stencil_target && depth_stencil_target)
    log.Printf ("Clear depth-stencil target [%.3f %u]", clear_depth_value, clear_stencil_index);

    //собственно отрисовка

  DrawCore ();

    //конец кадра

  log.Printf ("End draw frame (id=%u)", Id ());  
}
