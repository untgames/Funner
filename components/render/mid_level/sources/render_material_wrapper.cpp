#include "shared.h"

using namespace render::mid_level;

Material::Material (MaterialImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Material::Material (const Material& material)
  : impl (material.impl)
{
  addref (impl);
}

Material::~Material ()
{
  release (impl);
}

Material& Material::operator = (const Material& material)
{
  Material (material).Swap (*this);
  return *this;
}

const char* Material::Id () const
{
  return impl->Id ();
}

void Material::SetId (const char* id)
{
  impl->SetId (id);
}

void Material::Update (const media::rfx::Material& material)
{
  impl->Update (material);
}

void Material::Swap (Material& material)
{
  stl::swap (impl, material.impl);
}

namespace render
{

namespace mid_level
{

void swap (Material& material1, Material& material2)
{
  material1.Swap (material2);
}

}

}
