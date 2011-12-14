#include "shared.h"

using namespace render;

/*
    Конструкторы
*/

RectAreaImpl::RectAreaImpl ()
{
}

RectAreaImpl::RectAreaImpl (const render::Rect& in_rect)
  : rect (in_rect)
{
}
    
/*
    Размеры области
*/

void RectAreaImpl::SetRect (const render::Rect& in_rect)
{
  rect = in_rect;
}
