#include "shared.h"

using namespace render::manager;

Texture::Texture (TextureImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Texture::Texture (const Texture& texture)
  : impl (texture.impl)
{
  addref (impl);
}

Texture::~Texture ()
{
  release (impl);
}

Texture& Texture::operator = (const Texture& texture)
{
  Texture (texture).Swap (*this);
  return *this;
}

TextureDimension Texture::Dimension () const
{
  return impl->Dimension ();
}

PixelFormat Texture::Format () const
{
  return impl->Format ();
}

unsigned int Texture::Width () const
{
  return impl->Width ();
}

unsigned int Texture::Height () const
{
  return impl->Height ();
}

unsigned int Texture::Depth () const
{
  return impl->Depth ();
}

RenderTarget Texture::RenderTarget (unsigned int layer, unsigned int mip_level) const
{
  return Wrappers::Wrap<render::manager::RenderTarget> (impl->RenderTarget (layer, mip_level));
}

void Texture::Update (const media::Image& image)
{
  impl->Update (image);
}

void Texture::Capture (unsigned int layer, unsigned int mip_level, media::Image& image)
{
  impl->Capture (layer, mip_level, image);
}

void Texture::Capture (unsigned int mip_level, media::Image& image)
{
  impl->Capture (mip_level, image);
}

void Texture::Swap (Texture& texture)
{
  stl::swap (impl, texture.impl);
}

namespace render
{

namespace manager
{

void swap (Texture& texture1, Texture& texture2)
{
  texture1.Swap (texture2);
}

}

}
