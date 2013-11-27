#include "shared.h"

using namespace render::manager;

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

void Frame::SetRenderTargets (const RenderTargetMap& map)
{
  impl->SetRenderTargets (*Wrappers::Unwrap<RenderTargetMapImpl> (map));
}

RenderTargetMap Frame::RenderTargets () const
{
  return Wrappers::Wrap<RenderTargetMap> (&impl->RenderTargets ());
}

void Frame::SetRenderTarget (const char* name, const render::manager::RenderTarget& target)
{
  try
  {
    impl->RenderTargets ().SetRenderTarget (name, Wrappers::Unwrap<RenderTargetImpl> (target));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Frame::SetRenderTarget(const char*,const RenderTarget&)");
    throw;
  }
}

void Frame::SetRenderTarget (const char* name, const render::manager::RenderTarget& target, const render::manager::Viewport& viewport)
{
  try
  {
    impl->RenderTargets ().SetRenderTarget (name, Wrappers::Unwrap<RenderTargetImpl> (target), Wrappers::Unwrap<ViewportImpl> (viewport));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Frame::SetRenderTarget(const char*,const RenderTarget&,const Viewport&)");
    throw;
  }
}

void Frame::SetRenderTarget (const char* name, const render::manager::RenderTarget& target, const render::manager::Viewport& viewport, const RectArea& scissor)
{
  try
  {
    impl->RenderTargets ().SetRenderTarget (name, Wrappers::Unwrap<RenderTargetImpl> (target), Wrappers::Unwrap<ViewportImpl> (viewport), Wrappers::Unwrap<RectAreaImpl> (scissor));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Frame::SetRenderTarget(const char*,const RenderTarget&,const Viewport&,const RectArea&)");
    throw;
  }
}

void Frame::RemoveRenderTarget (const char* name)
{
  impl->RenderTargets ().RemoveRenderTarget (name);
}

void Frame::RemoveAllRenderTargets ()
{
  impl->RenderTargets ().RemoveAllRenderTargets ();
}

bool Frame::HasRenderTarget (const char* name) const
{
  return impl->RenderTargets ().FindRenderTarget (name) != RenderTargetPtr ();
}

RenderTarget Frame::RenderTarget (const char* name) const
{
  try
  {
    return Wrappers::Wrap<render::manager::RenderTarget> (impl->RenderTargets ().RenderTarget (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Frame::RenderTarget");
    throw;
  }
}

Viewport Frame::Viewport (const char* name) const
{
  try
  {
    return Wrappers::Wrap<render::manager::Viewport> (impl->RenderTargets ().Viewport (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Frame::Viewport");
    throw;
  }
}

RectArea Frame::Scissor (const char* name) const
{
  try
  {
    return Wrappers::Wrap<RectArea> (impl->RenderTargets ().Scissor (name));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Frame::Scissor");
    throw;
  }
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

void Frame::SetLocalTexture (const char* name, const render::manager::Texture& texture)
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

void Frame::SetShaderOptions (const common::PropertyMap& defines)
{
  impl->SetShaderOptions (defines);
}

const common::PropertyMap& Frame::ShaderOptions () const
{
  return impl->ShaderOptions ();
}

size_t Frame::EntitiesCount () const
{
  return impl->EntitiesCount ();
}

void Frame::AddEntity (const Entity& entity, void* user_data)
{
  impl->AddEntity (Wrappers::Unwrap<EntityImpl> (entity), user_data);
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

void Frame::SetAutoCleanup (bool state)
{
  impl->SetAutoCleanup (state);
}

bool Frame::IsAutoCleanupEnabled () const
{
  return impl->IsAutoCleanupEnabled ();
}

void Frame::SetEntityDrawHandler (const EntityDrawFunction& handler)
{
  impl->SetEntityDrawHandler (handler);
}

const Frame::EntityDrawFunction& Frame::EntityDrawHandler () const
{
  return impl->EntityDrawHandler ();
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

namespace manager
{

void swap (Frame& frame1, Frame& frame2)
{
  frame1.Swap (frame2);
}

}

}
