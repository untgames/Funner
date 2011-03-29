#include "shared.h"

using namespace physics;

/*
   Материал
*/

typedef xtl::com_ptr<physics::low_level::IDriver>   DriverPtr;
typedef xtl::com_ptr<physics::low_level::IMaterial> MaterialPtr;

struct Material::Impl : public xtl::reference_counter
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

Material::Material (const Material& source)
  : impl (source.impl)
{
  addref (impl);
}

Material::Material (Impl* source_impl)
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
  return Material (new Impl (*impl));
}
    
/*
   Управление линейной/угловой аммортизацией
*/

float Material::LinearDamping () const
{
  return impl->material->LinearDamping ();
}

float Material::AngularDamping () const
{
  return impl->material->AngularDamping ();
}

void Material::SetLinearDamping (float value)
{
  impl->material->SetLinearDamping (value);
}

void Material::SetAngularDamping (float value)
{
  impl->material->SetAngularDamping (value);
}

/*
   Управление трением
*/

float Material::Friction () const
{
  return impl->material->Friction ();
}

const math::vec3f& Material::AnisotropicFriction () const
{
  return impl->material->AnisotropicFriction ();
}

void Material::SetFriction (float value)
{
  impl->material->SetFriction (value);
}

void Material::SetAnisotropicFriction (const math::vec3f& value)
{
  impl->material->SetAnisotropicFriction (value);
}

/*
   Управление упругостью
*/

float Material::Restitution () const
{
  return impl->material->Restitution ();
}

void Material::SetRestitution (float value)
{
  impl->material->SetRestitution (value);
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

/*
   Создание
*/

Material MaterialImplProvider::CreateMaterial (physics::low_level::IDriver* driver)
{
  return Material (new Material::Impl (driver));
}

Material MaterialImplProvider::CreateMaterial (physics::low_level::IDriver* driver, const media::physics::Material& media_material)
{
  Material return_value (new Material::Impl (driver));

  return_value.SetLinearDamping       (media_material.LinearDamping ());
  return_value.SetAngularDamping      (media_material.AngularDamping ());
  return_value.SetFriction            (media_material.Friction ());
  return_value.SetAnisotropicFriction (media_material.AnisotropicFriction ());
  return_value.SetRestitution         (media_material.Restitution ());

  return return_value;
}

/*
   Получение низкоуровневого материала
*/

physics::low_level::IMaterial* MaterialImplProvider::LowLevelMaterial (const Material& material)
{
  return material.impl->material.get ();
}
