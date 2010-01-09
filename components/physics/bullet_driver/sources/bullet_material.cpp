#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

namespace
{

const float       DEFAULT_ANGULAR_DAMPING      = 0.f;
const math::vec3f DEFAULT_ANISOTROPIC_FRICTION (1.f);
const float       DEFAULT_FRICTION             = 0.5f;
const float       DEFAULT_LINEAR_DAMPING       = 0.f;
const float       DEFAULT_RESTITUTION          = 0.f;

typedef xtl::signal<void ()> UpdateSignal;

}

/*
    Описание реализации материала тела
*/

struct Material::Impl
{
  math::vec3f  anisotropic_friction;  //трение по другим осям
  float        linear_damping;        //линейная амортизация
  float        angular_damping;       //угловая амортизация
  float        friction;              //трение
  float        restitution;           //упругость
  UpdateSignal update_signal;         //сигнал обновления свойств объекта

  Impl ()
    : anisotropic_friction (DEFAULT_ANISOTROPIC_FRICTION)
    , linear_damping       (DEFAULT_LINEAR_DAMPING)
    , angular_damping      (DEFAULT_ANGULAR_DAMPING)
    , friction             (DEFAULT_FRICTION)
    , restitution          (DEFAULT_RESTITUTION)
    {}

  void OnUpdate ()
  {
    update_signal ();
  }
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

float Material::LinearDamping ()
{
  return impl->linear_damping;
}

float Material::AngularDamping ()
{
  return impl->angular_damping;
}

void Material::SetLinearDamping (float value)
{
  impl->linear_damping = value;

  impl->OnUpdate ();
}

void Material::SetAngularDamping (float value)
{
  impl->angular_damping = value;

  impl->OnUpdate ();
}

/*
   Управление трением
*/

float Material::Friction ()
{
  return impl->friction;
}

const math::vec3f& Material::AnisotropicFriction ()
{
  return impl->anisotropic_friction;
}

void Material::SetFriction (float value)
{
  impl->friction = value;

  impl->OnUpdate ();
}

void Material::SetAnisotropicFriction (const math::vec3f& value)
{
  impl->anisotropic_friction = value;

  impl->OnUpdate ();
}

/*
   Управление упругостью
*/

float Material::Restitution ()
{
  return impl->restitution;
}

void Material::SetRestitution (float value)
{
  impl->restitution = value;

  impl->OnUpdate ();
}

/*
   Подписка на обновление свойств
*/

xtl::connection Material::RegisterUpdateHandler (const UpdateHandler& handler)
{
  return impl->update_signal.connect (handler);
}
