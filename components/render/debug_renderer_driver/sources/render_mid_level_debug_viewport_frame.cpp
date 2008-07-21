#include "shared.h"

using namespace render::mid_level;
using namespace render::mid_level::debug;

/*
    Конструктор
*/

ViewportFrame::ViewportFrame ()
{
  memset (&viewport, 0, sizeof viewport);
}

/*
    Параметры области вывода
*/

void ViewportFrame::SetViewport (const Viewport& in_viewport)
{
  viewport = in_viewport;
}

void ViewportFrame::GetViewport (Viewport& out_viewport)
{
  out_viewport = viewport;
}

/*
    Визуализация
*/

void ViewportFrame::DrawCore ()
{ 
    //установка области вывода
    
  log.Printf ("Set viewport (left=%d, top=%d, width=%u, height=%u)", viewport.x, viewport.y, viewport.width, viewport.height);
}
