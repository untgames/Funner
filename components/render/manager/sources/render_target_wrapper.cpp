#include "shared.h"

using namespace render::manager;

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

unsigned int RenderTarget::Width () const
{
  return impl->Width ();
}

unsigned int RenderTarget::Height () const
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

namespace manager
{

void swap (RenderTarget& render_target1, RenderTarget& render_target2)
{
  render_target1.Swap (render_target2);
}

}

}
