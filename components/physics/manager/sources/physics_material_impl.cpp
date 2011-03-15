#include "shared.h"

using namespace physics;

typedef xtl::com_ptr<physics::low_level::IDriver>   DriverPtr;
typedef xtl::com_ptr<physics::low_level::IMaterial> MaterialPtr;

/*
   Реализация материала
*/

struct MaterialImpl::Impl
{
  DriverPtr   driver;
  MaterialPtr material;

  Impl (physics::low_level::IDriver* in_driver)
  {
    if (!driver)
      throw xtl::make_null_argument_exception ("physics::MaterialImpl::MaterialImpl", "driver");

    driver = in_driver;

    material = MaterialPtr (driver->CreateMaterial (), false);
  }

  Impl (const Impl& source)
    : driver (source.driver)
  {
    material = MaterialPtr (driver->CreateMaterial (), false);

    material->SetLinearDamping       (source.material->LinearDamping ());
    material->SetAngularDamping      (source.material->AngularDamping ());
    material->SetFriction            (source.material->Friction ());
    material->SetAnisotropicFriction (source.material->AnisotropicFriction ());
    material->SetRestitution         (source.material->Restitution ());
  }
};

/*
   Конструктор / деструктор / копирование
*/

MaterialImpl::MaterialImpl (physics::low_level::IDriver* driver)
  : impl (new Impl (driver))
  {}

MaterialImpl::MaterialImpl (const MaterialImpl& source)
  : impl (new Impl (*source.impl))
  {}

MaterialImpl::~MaterialImpl ()
{
  delete (impl);
}

MaterialImpl& MaterialImpl::operator = (const MaterialImpl& source)
{
  MaterialImpl (source).Swap (*this);

  return *this;
}
    
/*
   Управление линейной/угловой аммортизацией
*/

float MaterialImpl::LinearDamping () const
{
  return impl->material->LinearDamping ();
}

float MaterialImpl::AngularDamping () const
{
  return impl->material->AngularDamping ();
}

void MaterialImpl::SetLinearDamping (float value)
{
  impl->material->SetLinearDamping (value);
}

void MaterialImpl::SetAngularDamping (float value)
{
  impl->material->SetAngularDamping (value);
}

/*
   Управление трением
*/

float MaterialImpl::Friction () const
{
  return impl->material->Friction ();
}

const math::vec3f& MaterialImpl::AnisotropicFriction () const
{
  return impl->material->AnisotropicFriction ();
}

void MaterialImpl::SetFriction (float value)
{
  impl->material->SetFriction (value);
}

void MaterialImpl::SetAnisotropicFriction (const math::vec3f& value)
{
  impl->material->SetAnisotropicFriction (value);
}

/*
   Управление упругостью
*/

float MaterialImpl::Restitution () const
{
  return impl->material->Restitution ();
}

void MaterialImpl::SetRestitution (float value)
{
  impl->material->SetRestitution (value);
}

/*
   Обмен
*/

void MaterialImpl::Swap (MaterialImpl& material)
{
  stl::swap (impl, material.impl);
}
