#include "shared.h"

using namespace render;

Frame::Frame (FrameImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Frame::Frame (const Frame& frame)
  : impl (frame.impl)
{
  addref (impl);
}

Frame::~Frame ()
{
  release (impl);
}

Frame& Frame::operator = (const Frame& frame)
{
  Frame (frame).Swap (*this);
  return *this;
}

void Frame::SetRenderTargets (const render::RenderTarget& render_target, const render::RenderTarget& depth_stencil_target)
{
  impl->SetRenderTargets (Wrappers::Unwrap<RenderTargetImpl> (render_target), Wrappers::Unwrap<RenderTargetImpl> (depth_stencil_target));
}

RenderTarget Frame::RenderTarget ()
{
  return Wrappers::Wrap<render::RenderTarget> (impl->RenderTarget ());
}

RenderTarget Frame::DepthStencilTarget ()
{
  return Wrappers::Wrap<render::RenderTarget> (impl->DepthStencilTarget ());
}

const RenderTarget Frame::RenderTarget () const
{
  return Wrappers::Wrap<render::RenderTarget> (impl->RenderTarget ());
}

const RenderTarget Frame::DepthStencilTarget () const
{
  return Wrappers::Wrap<render::RenderTarget> (impl->DepthStencilTarget ());
}

void Frame::SetViewport (const Rect& rect)
{
  impl->SetViewport (rect);
}

const Rect& Frame::Viewport () const
{
  return impl->Viewport ();
}

void Frame::SetScissor (const Rect& rect)
{
  impl->SetScissor (rect);
}

const Rect& Frame::Scissor () const
{
  return impl->Scissor ();
}

void Frame::SetScissorState (bool state)
{
  impl->SetScissorState (state);
}

bool Frame::ScissorState () const
{
  return impl->ScissorState ();
}

void Frame::SetClearFlags (size_t clear_flags)
{
  impl->SetClearFlags (clear_flags);
}

size_t Frame::ClearFlags () const
{
  return impl->ClearFlags ();
}

void Frame::SetClearColor (const math::vec4f& color)
{
  impl->SetClearColor (color);
}

const math::vec4f& Frame::ClearColor () const
{
  return impl->ClearColor ();
}

void Frame::SetClearDepthValue (float depth_value)
{
  impl->SetClearDepthValue (depth_value);
}

void Frame::SetClearStencilIndex (unsigned char stencil_index)
{
  impl->SetClearStencilIndex (stencil_index);
}

float Frame::ClearDepthValue ()
{
  return impl->ClearDepthValue ();
}

unsigned char Frame::ClearStencilIndex ()
{
  return impl->ClearStencilIndex ();
}

void Frame::SetViewMatrix (const math::mat4f& tm)
{
  impl->SetViewMatrix (tm);
}

void Frame::SetProjectionMatrix (const math::mat4f& tm)
{
  impl->SetProjectionMatrix (tm);
}

const math::mat4f& Frame::ViewMatrix () const
{
  return impl->ViewMatrix ();
}

const math::mat4f& Frame::ProjectionMatrix () const
{
  return impl->ProjectionMatrix ();
}

void Frame::SetTechnique (const char* name)
{
  impl->SetTechnique (name);
}

const char* Frame::Technique () const
{
  return impl->Technique ();
}

void Frame::SetProperties (const common::PropertyMap& properties)
{
  impl->SetProperties (properties);
}

const common::PropertyMap& Frame::Properties () const
{
  return impl->Properties ();
}

size_t Frame::EntitiesCount () const
{
  return impl->EntitiesCount ();
}

void Frame::AddEntity (const Entity& entity)
{
  impl->AddEntity (Wrappers::Unwrap<EntityImpl> (entity));
}

void Frame::RemoveAllEntities ()
{
  impl->RemoveAllEntities ();
}

void Frame::AddFrame (FrameOrder order, const Frame& frame)
{
  impl->AddFrame (order, Wrappers::Unwrap<FrameImpl> (frame));
}

void Frame::RemoveAllFrames ()
{
  impl->RemoveAllFrames ();
}

Frame Frame::AddPreRenderFrame ()
{
  return Wrappers::Wrap<Frame> (impl->AddPreRenderFrame ());
}

Frame Frame::AddChildFrame ()
{
  return Wrappers::Wrap<Frame> (impl->AddChildFrame ());
}

Frame Frame::AddPostRenderFrame ()
{
  return Wrappers::Wrap<Frame> (impl->AddPostRenderFrame ());
}

void Frame::RemoveAll ()
{
  impl->RemoveAll ();
}

void Frame::Draw ()
{
  impl->Draw ();
}

void Frame::Swap (Frame& frame)
{
  stl::swap (impl, frame.impl);
}

namespace render
{

void swap (Frame& frame1, Frame& frame2)
{
  frame1.Swap (frame2);
}

}
