#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

Frame::Frame (FrameImpl* in_impl)
  : impl (in_impl)
{
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

void Frame::SetRenderTargets (const mid_level::RenderTarget& render_target, const mid_level::RenderTarget& depth_stencil_target)
{
}

mid_level::RenderTarget Frame::RenderTarget ();
mid_level::RenderTarget Frame::DepthStencilTarget ();
const mid_level::RenderTarget Frame::RenderTarget () const;
const mid_level::RenderTarget Frame::DepthStencilTarget () const;

void Frame::SetViewport (const Rect&);
const Rect& Frame::Viewport () const;

void Frame::SetScissor (const Rect&);
const Rect& Frame::Scissor () const;
void Frame::SetScissorState (bool state);
bool Frame::ScissorState () const;

void Frame::SetClearFlags (size_t clear_flags);
size_t Frame::ClearFlags    () const;

void Frame::SetClearColor (const math::vec4f& color);
const math::vec4f& Frame::ClearColor    () const;

void Frame::SetClearDepthValue   (float depth_value);
void Frame::SetClearStencilIndex (unsigned char stencil_index);
float Frame::ClearDepthValue   ();
unsigned char Frame::ClearStencilIndex ();

void Frame::SetViewMatrix (const math::mat4f&);
void Frame::SetProjectionMatrix (const math::mat4f&);
const math::mat4f& Frame::ViewMatrix () const;
const math::mat4f& Frame::ProjectionMatrix () const;

void Frame::SetTechnique (const char* name);
const char* Frame::Tehcnique () const;

void Frame::SetProperties (const common::PropertyMap&);
const common::PropertyMap& Frame::Properties () const;

size_t Frame::EntitiesCount () const;
void Frame::AddEntity (const Entity&);
void Frame::RemoveAllEntities ();

void Frame::AddFrame (FrameOrder order, const Frame& frame);
void Frame::RemoveAllFrames ();

Frame Frame::AddPreRenderFrame  ();
Frame Frame::AddChildFrame      ();
Frame Frame::AddPostRenderFrame ();

void Frame::RemoveAll ();

void Frame::Draw ();

void Frame::Swap (Frame&);

namespace render
{

namespace mid_level
{

void swap (Frame& frame1, Frame& frame2)
{
  frame1.Swap (frame2);
}

}

}
