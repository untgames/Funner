#include "shared.h"

using namespace render::manager;

Viewport::Viewport ()
  : impl (new ViewportImpl)
{
}

Viewport::Viewport (const render::manager::Rect& rect, float min_depth, float max_depth)
  : impl (new ViewportImpl (rect, min_depth, max_depth))
{
}

Viewport::Viewport (ViewportImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Viewport::~Viewport ()
{
  release (impl);
}

Viewport& Viewport::operator = (const Viewport& viewport)
{
  Viewport (viewport).Swap (*this);
  
  return *this;
}

RectArea Viewport::Area () const
{
  return Wrappers::Wrap<RectArea> (&impl->Area ());
}

void Viewport::SetRect (const render::manager::Rect& rect)
{
  impl->Area ().SetRect (rect);
}

void Viewport::SetRect (int x, int y, size_t width, size_t height)
{
  SetRect (render::manager::Rect (x, y, width, height));
}

const render::manager::Rect& Viewport::Rect () const
{
  return impl->Area ().Rect ();
}

int Viewport::X () const
{
  return impl->Area ().Rect ().x;
}

int Viewport::Y () const
{
  return impl->Area ().Rect ().y;
}

size_t Viewport::Width () const
{
  return impl->Area ().Rect ().width;
}

size_t Viewport::Height () const
{
  return impl->Area ().Rect ().height;
}

float Viewport::MinDepth () const
{
  return impl->MinDepth ();
}

float Viewport::MaxDepth () const
{
  return impl->MaxDepth ();
}

void Viewport::SetMinDepth (float value)
{
  impl->SetMinDepth (value);
}

void Viewport::SetMaxDepth (float value)
{
  impl->SetMaxDepth (value);
}

void Viewport::Swap (Viewport& viewport)
{
  stl::swap (impl, viewport.impl);
}

namespace render
{

namespace manager
{

void swap (Viewport& viewport1, Viewport& viewport2)
{
  viewport1.Swap (viewport2);
}

}

}
