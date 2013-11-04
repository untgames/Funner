#include "shared.h"

using namespace render::manager;

RenderTargetMap::RenderTargetMap ()
  : impl (new RenderTargetMapImpl)
{
  addref (impl);
}

RenderTargetMap::RenderTargetMap (const RenderTargetMap& map)
  : impl (map.impl)
{
  addref (impl);
}

RenderTargetMap::~RenderTargetMap ()
{
  release (impl);
}

RenderTargetMap& RenderTargetMap::operator = (const RenderTargetMap& map)
{
  RenderTargetMap (map).Swap (*this);
  return *this;
}

void RenderTargetMap::SetRenderTarget (const char* name, const render::manager::RenderTarget& target)
{
  impl->SetRenderTarget (name, Wrappers::Unwrap<RenderTargetImpl> (target));
}

void RenderTargetMap::SetRenderTarget (const char* name, const render::manager::RenderTarget& target, const render::manager::Viewport& viewport)
{
  impl->SetRenderTarget (name, Wrappers::Unwrap<RenderTargetImpl> (target), Wrappers::Unwrap<ViewportImpl> (viewport));
}

void RenderTargetMap::SetRenderTarget (const char* name, const render::manager::RenderTarget& target, const render::manager::Viewport& viewport, const RectArea& scissor)
{
  impl->SetRenderTarget (name, Wrappers::Unwrap<RenderTargetImpl> (target), Wrappers::Unwrap<ViewportImpl> (viewport), Wrappers::Unwrap<RectAreaImpl> (scissor));
}

void RenderTargetMap::RemoveRenderTarget (const char* name)
{
  impl->RemoveRenderTarget (name);
}

void RenderTargetMap::RemoveAllRenderTargets ()
{
  impl->RemoveAllRenderTargets ();
}

bool RenderTargetMap::HasRenderTarget (const char* name) const
{
  return impl->FindRenderTarget (name) != RenderTargetPtr ();
}

RenderTarget RenderTargetMap::RenderTarget (const char* name) const
{
  return Wrappers::Wrap<render::manager::RenderTarget> (impl->RenderTarget (name));
}

Viewport RenderTargetMap::Viewport (const char* name) const
{
  return Wrappers::Wrap<render::manager::Viewport> (impl->Viewport (name));
}

RectArea RenderTargetMap::Scissor (const char* name) const
{
  return Wrappers::Wrap<RectArea> (impl->Scissor (name));
}

namespace render
{

namespace manager
{

void swap (RenderTargetMap& map1, RenderTargetMap& map2)
{
  map1.Swap (map2);
}

}

}
