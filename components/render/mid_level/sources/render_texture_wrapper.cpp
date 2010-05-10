#include "shared.h"

using namespace render::mid_level;

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

const char* Texture::Id () const
{
  return impl->Id ();
}

void Texture::SetId (const char* name)
{
  impl->SetId (name);
}

TextureDimension Texture::Dimension () const
{
  return impl->Dimension ();
}

PixelFormat Texture::Format () const
{
  return impl->Format ();
}

size_t Texture::Width () const
{
  return impl->Width ();
}

size_t Texture::Height () const
{
  return impl->Height ();
}

size_t Texture::Depth () const
{
  return impl->Depth ();
}

RenderTarget Texture::RenderTarget (size_t layer, size_t mip_level) const
{
  return Wrappers::Wrap<render::mid_level::RenderTarget> (impl->RenderTarget (layer, mip_level));
}

void Texture::Update (const media::Image& image)
{
  impl->Update (image);
}

void Texture::Swap (Texture& texture)
{
  stl::swap (impl, texture.impl);
}

namespace render
{

namespace mid_level
{

void swap (Texture& texture1, Texture& texture2)
{
  texture1.Swap (texture2);
}

}

}
