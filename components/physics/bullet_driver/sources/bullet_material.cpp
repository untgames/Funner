#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

/*
    Описание реализации материала тела
*/

struct Material::Impl
{
};

/*
    Конструктор
*/

Material::Material ()
  : impl (new Impl)
  {}

/*
   Управление линейной/угловой аммортизацией
*/

const math::vec3f& Material::LinearDamping ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::LinearDamping");
}

const math::vec3f& Material::AngularDamping ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::AngularDamping");
}

void Material::SetLinearDamping (const math::vec3f& value)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::SetLinearDamping");
}

void Material::SetAngularDamping (const math::vec3f& value)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::SetAngularDamping");
}

/*
   Управление трением
*/

float Material::Friction ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::Friction");
}

float Material::AnisotropicFriction ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::AnisotropicFriction");
}

void Material::SetFriction (float value)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::SetFriction");
}

void Material::SetAnisotropicFriction (float value)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::SetAnisotropicFriction");
}

/*
   Управление упругостью
*/

float Material::Restitution ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::Restitution");
}

void Material::SetRestitution (float value)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Material::SetRestitution");
}
