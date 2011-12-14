#include "shared.h"

using namespace render;

RenderTarget::RenderTarget (RenderTargetImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

RenderTarget::RenderTarget (const RenderTarget& render_target)
  : impl (render_target.impl)
{
  addref (impl);
}

RenderTarget::~RenderTarget ()
{
  release (impl);
}

RenderTarget& RenderTarget::operator = (const RenderTarget& render_target)
{
  RenderTarget (render_target).Swap (*this);
  return *this;
}

size_t RenderTarget::Width () const
{
  return impl->Width ();
}

size_t RenderTarget::Height () const
{
  return impl->Height ();
}

void RenderTarget::Capture (media::Image& image) const
{
  impl->Capture (image);
}

void RenderTarget::Swap (RenderTarget& render_target)
{
  stl::swap (impl, render_target.impl);
}

namespace render
{

void swap (RenderTarget& render_target1, RenderTarget& render_target2)
{
  render_target1.Swap (render_target2);
}

}
