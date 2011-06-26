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

void Frame::SetViewMatrix (const math::mat4f& matrix)
{
  impl->SetViewMatrix (matrix);
}

void Frame::SetProjectionMatrix (const math::mat4f& matrix)
{
  impl->SetProjectionMatrix (matrix);
}

const math::mat4f& Frame::ViewMatrix () const
{
  return impl->ViewMatrix ();
}

const math::mat4f& Frame::ProjectionMatrix () const
{
  return impl->ProjectionMatrix ();
}

void Frame::SetRenderTarget (const char* name, const render::RenderTarget& target)
{
  impl->SetRenderTarget (name, Wrappers::Unwrap<RenderTargetImpl> (target));
}

void Frame::SetRenderTarget (const char* name, const render::RenderTarget& target, const RectArea& viewport)
{
  impl->SetRenderTarget (name, Wrappers::Unwrap<RenderTargetImpl> (target), Wrappers::Unwrap<RectAreaImpl> (viewport));
}

void Frame::SetRenderTarget (const char* name, const render::RenderTarget& target, const RectArea& viewport, const RectArea& scissor)
{
  impl->SetRenderTarget (name, Wrappers::Unwrap<RenderTargetImpl> (target), Wrappers::Unwrap<RectAreaImpl> (viewport), 
    Wrappers::Unwrap<RectAreaImpl> (scissor));
}

void Frame::RemoveRenderTarget (const char* name)
{
  impl->RemoveRenderTarget (name);
}

void Frame::RemoveAllRenderTargets ()
{
  impl->RemoveAllRenderTargets ();
}

bool Frame::HasRenderTarget (const char* name) const
{
  return impl->FindRenderTarget (name) != RenderTargetPtr ();
}

RenderTarget Frame::RenderTarget (const char* name) const
{
  return Wrappers::Wrap<render::RenderTarget> (impl->RenderTarget (name));
}

RectArea Frame::Viewport (const char* name) const
{
  return Wrappers::Wrap<RectArea> (impl->Viewport (name));
}

RectArea Frame::Scissor (const char* name) const
{
  return Wrappers::Wrap<RectArea> (impl->Scissor (name));
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

float Frame::ClearDepthValue () const
{
  return impl->ClearDepthValue ();
}

unsigned char Frame::ClearStencilIndex () const
{
  return impl->ClearStencilIndex ();
}

void Frame::SetLocalTexture (const char* name, const render::Texture& texture)
{
  impl->SetLocalTexture (name, Wrappers::Unwrap<TextureImpl> (texture));
}

void Frame::RemoveLocalTexture (const char* name)
{
  impl->RemoveLocalTexture (name);
}

void Frame::RemoveAllLocalTextures ()
{
  impl->RemoveAllLocalTextures ();
}

bool Frame::HasLocalTexture (const char* name) const
{
  return impl->FindLocalTexture (name) != TexturePtr ();
}

Texture Frame::LocalTexture (const char* name) const
{
  return Wrappers::Wrap<Texture> (impl->LocalTexture (name));
}

void Frame::SetEffect (const char* name)
{
  impl->SetEffect (name);
}

const char* Frame::Effect () const
{
  return impl->Effect ();
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

void Frame::AddEntity (const Entity& entity, const common::PropertyMap& properties)
{
  impl->AddEntity (Wrappers::Unwrap<EntityImpl> (entity), properties);
}

void Frame::RemoveAllEntities ()
{
  impl->RemoveAllEntities ();
}

size_t Frame::FramesCount () const
{
  return impl->FramesCount ();
}

void Frame::AddFrame (const Frame& frame)
{
  impl->AddFrame (Wrappers::Unwrap<FrameImpl> (frame));
}

void Frame::RemoveAllFrames ()
{
  impl->RemoveAllFrames ();
}

void Frame::RemoveAllFramesAndEntities ()
{
  impl->RemoveAllFrames ();
  impl->RemoveAllEntities ();
}

void Frame::Draw ()
{
  impl->Draw ();
}

/*
    Управление кэшированием
*/

void Frame::UpdateCache ()
{
  impl->UpdateCache ();
}

void Frame::ResetCache ()
{
  impl->ResetCache ();
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
