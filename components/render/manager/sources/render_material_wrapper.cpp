#include "shared.h"

using namespace render;

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

/*
    Управление кэшированием
*/

void Material::UpdateCache ()
{
  impl->UpdateCache ();
}

void Material::ResetCache ()
{
  impl->ResetCache ();
}

void Material::Swap (Material& material)
{
  stl::swap (impl, material.impl);
}

namespace render
{

void swap (Material& material1, Material& material2)
{
  material1.Swap (material2);
}

}
