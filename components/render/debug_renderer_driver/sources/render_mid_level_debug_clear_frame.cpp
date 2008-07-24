#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

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
  clear_color = color;
}

void ClearFrame::GetColor (math::vec4f& out_color)
{
  out_color = clear_color;
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

void ClearFrame::DrawCore ()
{
    //если требуется очищать только область вывода

  if (clear_flags & ClearFlag_ViewportOnly)
    BasicFrame::BindViewport ();

    //очистка целевых буферов отрисовки

  if ((clear_flags & ClearFlag_RenderTarget) && GetRenderTarget ())
    log.Printf ("Clear render target [%.2f %.2f %.2f %.2f]", clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    
  if (GetDepthStencilTarget ())
  {
    switch (clear_flags & ClearFlag_DepthStencil)
    {
      case ClearFlag_DepthStencil:
        log.Printf ("Clear depth-stencil target [%.3f %u]", clear_depth_value, clear_stencil_index);
        break;
      case ClearFlag_Depth:
        log.Printf ("Clear depth-stencil target [%.3f no-clear]", clear_depth_value);
        break;
      case ClearFlag_Stencil:
        log.Printf ("Clear depth-stencil target [no-clear %u]", clear_stencil_index);
        break;        
    }
  }
}
