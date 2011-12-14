#include "shared.h"

using namespace render;

RectArea::RectArea ()
  : impl (new RectAreaImpl)
{
}

RectArea::RectArea (const render::Rect& in_rect)
  : impl (new RectAreaImpl (in_rect))
{
}

RectArea::RectArea (RectAreaImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

RectArea::~RectArea ()
{
  release (impl);
}

RectArea& RectArea::operator = (const RectArea& area)
{
  RectArea (area).Swap (*this);
  
  return *this;
}

void RectArea::SetRect (const render::Rect& rect)
{
  impl->SetRect (rect);
}

void  RectArea::SetRect (int x, int y, size_t width, size_t height)
{
  impl->SetRect (render::Rect (x, y, width, height));
}

const render::Rect& RectArea::Rect () const
{
  return impl->Rect ();
}

int RectArea::X () const
{
  return impl->Rect ().x;
}

int RectArea::Y () const
{
  return impl->Rect ().y;
}

size_t RectArea::Width () const
{
  return impl->Rect ().width;
}

size_t RectArea::Height () const
{
  return impl->Rect ().height;
}

void RectArea::Swap (RectArea& area)
{
  stl::swap (impl, area.impl);
}

namespace render
{

void swap (RectArea& area1, RectArea& area2)
{
  area1.Swap (area2);
}

}
