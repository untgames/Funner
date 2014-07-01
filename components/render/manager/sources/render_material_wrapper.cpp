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

Material Material::Clone () const
{
  try
  {
    MaterialPtr copy (new MaterialImpl (*impl), false);

    return Material (copy.get ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Material::Clone");
    throw;
  }
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

void Material::SetTexmap (const char* semantic, const char* image_name, const char* sampler)
{
  if (!image_name)
    throw xtl::make_null_argument_exception ("render::manager::Material::SetTexmap", "image_name");

  if (!sampler)
    throw xtl::make_null_argument_exception ("render::manager::Material::SetTexmap", "sampler");

  impl->SetTexmap (semantic, image_name, sampler);
}

void Material::SetTexmapImage (const char* semantic, const char* image_name)
{
  if (!image_name)
    throw xtl::make_null_argument_exception ("render::manager::Material::SetTexmapImage", "image_name");

  impl->SetTexmap (semantic, image_name, 0);
}

void Material::SetTexmapSampler (const char* semantic, const char* sampler)
{
  if (!sampler)
    throw xtl::make_null_argument_exception ("render::manager::Material::SetTexmap", "sampler");

  impl->SetTexmap (semantic, 0, sampler);
}

void Material::RemoveTexmap (const char* semantic)
{
  if (!semantic)
    return;

  impl->SetTexmap (semantic, 0, 0);
}

const char* Material::TexmapImage (const char* semantic) const
{
  return impl->TexmapImage (semantic);
}

const char* Material::TexmapSampler (const char* semantic) const
{
  return impl->TexmapSampler (semantic);
}

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
