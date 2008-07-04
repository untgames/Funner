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
    need_update_normalized_viewport (true),
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
  
  need_update_normalized_viewport = true;
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
  
  need_update_normalized_viewport = true;
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
    Обновление нормированной области вывода
*/

void BasicFrame::UpdateNormalizedViewport ()
{
  IRenderTarget* screen = render_target ? render_target.get () : depth_stencil_target.get ();
  
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

void BasicFrame::Draw (render::low_level::IDevice* device, const render::low_level::Rect& device_viewport)
{
  using namespace render::low_level;

  if (!render_target && !depth_stencil_target)
    return;
    
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
