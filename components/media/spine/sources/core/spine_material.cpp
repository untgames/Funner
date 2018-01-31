#include "shared.h"

using namespace media::spine;

/*
   Constructors / destructor / assignment
*/

Material::Material (const Material& source)
  : impl (source.impl)
{
  if (impl)
    impl->AddRef ();
}

Material::Material (MaterialImpl* in_impl)
  : impl (in_impl)
{
  if (!impl)
    throw xtl::make_null_argument_exception ("media::spine::Material::Material (IMaterial*)", "impl");
}

Material::~Material ()
{
  impl->Release ();
}

Material& Material::operator = (const Material& source)
{
  Material (source).Swap (*this);

  return *this;
}

/*
   Name
*/

const char* Material::Name () const
{
  return impl->Name ();
}

/*
   Params
*/

BlendMode Material::BlendMode () const
{
  return impl->BlendMode ();
}

TexcoordWrap Material::TexcoordWrapU () const
{
  return impl->TexcoordWrapU ();
}

TexcoordWrap Material::TexcoordWrapV () const
{
  return impl->TexcoordWrapV ();
}

const char* Material::TexturePath () const
{
  return impl->TexturePath ();
}

/*
   Swap
*/

void Material::Swap (Material& material)
{
  stl::swap (impl, material.impl);
}

namespace media
{

namespace spine
{

/*
   Swap
*/

void swap (Material& material1, Material& material2)
{
  material1.Swap (material2);
}

}

}
