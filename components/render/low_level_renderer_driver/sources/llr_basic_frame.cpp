#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;

/*
    Конструктор
*/

BasicFrame::BasicFrame ()
  : need_clear_render_target (false),
    need_clear_depth_stencil_target (false),
    clear_depth_value (0.0f),
    clear_stencil_index (0)
{
  viewport.min_depth = 0.f;
  viewport.max_depth = 1.f;
}

/*
    Целевые буферы отрисовки
*/

void BasicFrame::SetRenderTargets (IRenderTarget* in_render_target, IRenderTarget* in_depth_stencil_target)
{
  static const char* METHOD_NAME = "render::mid_level::low_level_driver::BasicFrame::SetRenderTargets";

  if (in_render_target)
  {
    RenderTarget* casted_render_target = dynamic_cast<RenderTarget*> (in_render_target);
    
    if (!casted_render_target)
      throw xtl::make_argument_exception (METHOD_NAME, "render_target", typeid (in_render_target).name (),
        "Render target type incompatible with render::mid_level::low_level_driver::RenderTarget");
        
    render_target = casted_render_target;
  }
  else render_target = 0;
  
  if (in_depth_stencil_target)
  {
    RenderTarget* casted_depth_stencil_target = dynamic_cast<RenderTarget*> (in_depth_stencil_target);
    
    if (!casted_depth_stencil_target)
      throw xtl::make_argument_exception (METHOD_NAME, "depth_stencil_target", typeid (in_depth_stencil_target).name (),
        "Depth-stencil target type incompatible with render::mid_level::low_level_driver::RenderTarget");
        
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
  clear_color.red   = color.x;
  clear_color.green = color.y;
  clear_color.blue  = color.z;
  clear_color.alpha = color.w;
}

void BasicFrame::SetClearDepthStencil (float depth_value, unsigned char stencil_index)
{
  clear_depth_value   = depth_value;
  clear_stencil_index = stencil_index;
}  

void BasicFrame::GetClearColor (math::vec4f& color)
{
  color.x = clear_color.red;
  color.y = clear_color.green;
  color.z = clear_color.blue;
  color.w = clear_color.alpha;
}

void BasicFrame::GetClearDepthStencil (float& depth_value, unsigned char& stencil_index)
{
  depth_value   = clear_depth_value;
  stencil_index = clear_stencil_index;
}

/*
    Визуализация
*/

void BasicFrame::Draw (render::low_level::IDevice* device)
{
  using namespace render::low_level;

  if (!render_target && !depth_stencil_target)
    return;

    //установка вьюпорта

  device->RSSetViewport (viewport);

    //очистка целевых буферов отрисовки  

  size_t clear_flags = 0;

  if (need_clear_render_target && render_target)
    clear_flags |= ClearFlag_RenderTarget;

  if (need_clear_depth_stencil_target && depth_stencil_target)
    clear_flags |= ClearFlag_Depth | ClearFlag_Stencil;

  device->ClearViews (clear_flags, clear_color, clear_depth_value, clear_stencil_index);

    //собственно отрисовка

  DrawCore (device);
}
