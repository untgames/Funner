#include "shared.h"

using namespace render::manager;

/*
    Конструкторы
*/

ViewportImpl::ViewportImpl ()
  : area (new RectAreaImpl, false)
  , min_depth (0.0f)
  , max_depth (1.0f)
{
}

ViewportImpl::ViewportImpl (const render::manager::Rect& rect, float in_min_depth, float in_max_depth)
  : area (new RectAreaImpl (rect), false)
  , min_depth (in_min_depth)
  , max_depth (in_max_depth)
{
}

/*
    Размеры области
*/

RectAreaImpl& ViewportImpl::Area ()
{
  return *area;
}

/*
    Диапазон глубины для области вывода
*/

float ViewportImpl::MinDepth ()
{
  return min_depth;
}

float ViewportImpl::MaxDepth ()
{
  return max_depth;
}

void ViewportImpl::SetMinDepth (float value)
{
  min_depth = value;
}

void ViewportImpl::SetMaxDepth (float value)
{
  max_depth = value;
}
