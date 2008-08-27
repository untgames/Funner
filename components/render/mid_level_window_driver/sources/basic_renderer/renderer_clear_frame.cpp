#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::window_driver;

/*
    Конструктор
*/

ClearFrame::ClearFrame ()
  : clear_flags (ClearFlag_All),
    clear_depth_value (1.0f),
    clear_stencil_index (0)
{
}

/*
    Флаги очистки кадра
*/

void ClearFrame::SetFlags (size_t in_clear_flags)
{
  clear_flags = in_clear_flags;
}

/*
    Параметры очистки буфера цвета
*/

void ClearFrame::SetColor (const math::vec4f& color)
{
  clear_color.red   = color.x;
  clear_color.green = color.y;
  clear_color.blue  = color.z;
  clear_color.alpha = color.w;  
}

void ClearFrame::GetColor (math::vec4f& out_color)
{
  out_color.x = clear_color.red;
  out_color.y = clear_color.green;
  out_color.z = clear_color.blue;
  out_color.w = clear_color.alpha;  
}

/*
    Параметры очистки буфера попиксельного отсечения
*/

void ClearFrame::SetDepthValue (float depth_value)
{
  clear_depth_value = depth_value;
}

void ClearFrame::SetStencilIndex (unsigned char stencil_index)
{
  clear_stencil_index = stencil_index;
}

/*
    Реализация визуализации
*/

void ClearFrame::DrawCore (render::low_level::IDevice* device)
{
  size_t device_clear_flags = 0;

    //если необходимо очищать только область вывода - устанавливаем её
    
  if (clear_flags & render::mid_level::ClearFlag_ViewportOnly)
  {
    BasicFrame::BindViewport (device);

    device_clear_flags |= render::low_level::ClearFlag_ViewportOnly;
  }

    //очистка целевых буферов отрисовки          
  
  if ((clear_flags & render::mid_level::ClearFlag_RenderTarget) && BasicFrame::GetRenderTarget ())
    device_clear_flags |= render::low_level::ClearFlag_RenderTarget;

  if (BasicFrame::GetDepthStencilTarget ())
  {
    if (clear_flags & render::mid_level::ClearFlag_Depth)   device_clear_flags |= render::low_level::ClearFlag_Depth;
    if (clear_flags & render::mid_level::ClearFlag_Stencil) device_clear_flags |= render::low_level::ClearFlag_Stencil;
  }

  device->ClearViews (device_clear_flags, clear_color, clear_depth_value, clear_stencil_index);  
}
