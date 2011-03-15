#include "shared.h"

using namespace physics;

/*
   Материал
*/

/*
   Конструктор / деструктор / копирование
*/

MaterialInternal::MaterialInternal (MaterialImpl* impl)
  : Material (impl)
  {}

Material::Material (const Material& source)
  : impl (source.impl)
{
  addref (impl);
}

Material::Material (MaterialImpl* source_impl)
  : impl (source_impl)
  {}

Material::~Material ()
{
  release (impl);
}

Material& Material::operator = (const Material& source)
{
  Material (source).Swap (*this);

  return *this;
}
    
/*
   Клонирование
*/

Material Material::Clone () const
{
  stl::auto_ptr<MaterialImpl> material_impl (new MaterialImpl (*impl));

  MaterialInternal return_value (material_impl.get ());

  material_impl.release ();

  return return_value;
}
    
/*
   Управление линейной/угловой аммортизацией
*/

float Material::LinearDamping () const
{
  return impl->LinearDamping ();
}

float Material::AngularDamping () const
{
  return impl->AngularDamping ();
}

void Material::SetLinearDamping (float value)
{
  impl->SetLinearDamping (value);
}

void Material::SetAngularDamping (float value)
{
  impl->SetAngularDamping (value);
}

/*
   Управление трением
*/

float Material::Friction () const
{
  return impl->Friction ();
}

const math::vec3f& Material::AnisotropicFriction () const
{
  return impl->AnisotropicFriction ();
}

void Material::SetFriction (float value)
{
  impl->SetFriction (value);
}

void Material::SetAnisotropicFriction (const math::vec3f& value)
{
  impl->SetAnisotropicFriction (value);
}

/*
   Управление упругостью
*/

float Material::Restitution () const
{
  return impl->Restitution ();
}

void Material::SetRestitution (float value)
{
  impl->SetRestitution (value);
}

/*
   Обмен
*/

void Material::Swap (Material& source)
{
  stl::swap (impl, source.impl);
}

namespace physics
{

void swap (Material& material1, Material& material2)
{
  material1.Swap (material2);
}

}
