#include "shared.h"

using namespace render::manager;

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

const char* Material::Name () const
{
  return impl->Name ();
}

void Material::SetName (const char* name)
{
  impl->SetName (name);
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

namespace manager
{

void swap (Material& material1, Material& material2)
{
  material1.Swap (material2);
}

}

}
