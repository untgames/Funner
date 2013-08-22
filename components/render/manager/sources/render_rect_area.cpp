#include "shared.h"

using namespace render::manager;

/*
    Конструкторы
*/

RectAreaImpl::RectAreaImpl ()
{
}

RectAreaImpl::RectAreaImpl (const render::manager::Rect& in_rect)
  : rect (in_rect)
{
}
    
/*
    Размеры области
*/

void RectAreaImpl::SetRect (const render::manager::Rect& in_rect)
{
  rect = in_rect;
}
