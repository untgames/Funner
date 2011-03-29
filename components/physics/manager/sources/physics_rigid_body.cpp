#include "shared.h"

using namespace physics;

/*
   Твердое тело
*/

RigidBodyImpl::RigidBodyImpl (RigidBodyPtr in_body, const physics::Shape& in_shape, const physics::Material& in_material)
  : body (in_body), shape (in_shape), material (in_material)
{
  body->SetMaterial (MaterialImplProvider::LowLevelMaterial (material));

  transform_update_connection = body->RegisterTransformUpdateCallback (xtl::bind (&RigidBodyImpl::OnTransformUpdate, this));

  OnTransformUpdate ();
}

/*
   Обработка изменения позиции
*/

void RigidBodyImpl::OnTransformUpdate ()
{
  const physics::low_level::Transform& body_transform = body->WorldTransform ();

  current_transform.position    = body_transform.position;
  current_transform.orientation = body_transform.orientation;
}

/*
   Геометрическое тело
*/

physics::Shape& RigidBodyImpl::Shape ()
{
  return shape;
}

/*
   Материал
*/

physics::Material& RigidBodyImpl::Material ()
{
  return material;
}

void RigidBodyImpl::SetMaterial (const physics::Material& material)
{
  body->SetMaterial (MaterialImplProvider::LowLevelMaterial (material));
}

/*
   Получение низкоуровневого тела
*/

physics::low_level::IRigidBody* RigidBodyImpl::LowLevelBody ()
{
  return body.get ();
}

/*
   Конструктор / деструктор / копирование
*/

RigidBody::RigidBody (const RigidBody& source)
  : impl (source.impl)
{
  addref (impl);
}

RigidBody::RigidBody (RigidBodyImpl* source_impl)
  : impl (source_impl)
  {}

RigidBody::~RigidBody ()
{
  release (impl);
}

RigidBody& RigidBody::operator = (const RigidBody& source)
{
  RigidBody (source).Swap (*this);

  return *this;
}
    
/*
   Масса
*/

float RigidBody::Mass () const
{
  return impl->LowLevelBody ()->Mass ();
}

void RigidBody::SetMass (float mass)
{
  impl->LowLevelBody ()->SetMass (mass);
}

/*
   Управление тензором
*/

const math::vec3f& RigidBody::MassSpaceInertiaTensor () const
{
  return impl->LowLevelBody ()->MassSpaceInertiaTensor ();
}

void RigidBody::SetMassSpaceInertiaTensor (const math::vec3f& tensor)
{
  impl->LowLevelBody ()->SetMassSpaceInertiaTensor (tensor);
}

/*
   Мировое положение
*/

const Transform& RigidBody::WorldTransform () const
{
  return impl->CurrentTransform ();
}

void RigidBody::SetWorldTransform (const Transform& transform)
{
  physics::low_level::Transform body_transform;

  body_transform.position    = transform.position;
  body_transform.orientation = transform.orientation;

  impl->LowLevelBody ()->SetWorldTransform (body_transform);
}

/*
   Управление сном
*/

float RigidBody::SleepLinearVelocity () const
{
  return impl->LowLevelBody ()->SleepLinearVelocity ();
}

float RigidBody::SleepAngularVelocity () const
{
  return impl->LowLevelBody ()->SleepAngularVelocity ();
}

void RigidBody::SetSleepLinearVelocity (float value)
{
  impl->LowLevelBody ()->SetSleepLinearVelocity (value);
}

void RigidBody::SetSleepAngularVelocity (float value)
{
  impl->LowLevelBody ()->SetSleepAngularVelocity (value);
}

/*
   Управление пороговым значением скорости перехода в режим непрерывного движения
*/

float RigidBody::CcdMotionThreshold () const
{
  return impl->LowLevelBody ()->CcdMotionThreshold ();
}

void RigidBody::SetCcdMotionThreshold (float value)
{
  impl->LowLevelBody ()->SetCcdMotionThreshold (value);
}

/*
   Геометрическое тело
*/

const physics::Shape& RigidBody::Shape () const
{
  return const_cast<RigidBody&> (*this).Shape ();
}

physics::Shape& RigidBody::Shape ()
{
  return impl->Shape ();
}

/*
   Материал
*/

const physics::Material& RigidBody::Material () const
{
  return const_cast<RigidBody&> (*this).Material ();
}

physics::Material& RigidBody::Material ()
{
  return impl->Material ();
}

void RigidBody::SetMaterial (const physics::Material& material)
{
  impl->SetMaterial (material);
}

/*
   Флаги поведения
*/

size_t RigidBody::Flags () const
{
  size_t return_value = 0, body_flags = impl->LowLevelBody ()->Flags ();

  if (body_flags & physics::low_level::RigidBodyFlag_FrozenPositionX)
    return_value |= physics::RigidBodyFlag_FrozenPositionX;
  if (body_flags & physics::low_level::RigidBodyFlag_FrozenPositionY)
    return_value |= physics::RigidBodyFlag_FrozenPositionY;
  if (body_flags & physics::low_level::RigidBodyFlag_FrozenPositionZ)
    return_value |= physics::RigidBodyFlag_FrozenPositionZ;
  if (body_flags & physics::low_level::RigidBodyFlag_FrozenRotationX)
    return_value |= physics::RigidBodyFlag_FrozenRotationX;
  if (body_flags & physics::low_level::RigidBodyFlag_FrozenRotationY)
    return_value |= physics::RigidBodyFlag_FrozenRotationY;
  if (body_flags & physics::low_level::RigidBodyFlag_FrozenRotationZ)
    return_value |= physics::RigidBodyFlag_FrozenRotationZ;
  if (body_flags & physics::low_level::RigidBodyFlag_Kinematic)
    return_value |= physics::RigidBodyFlag_Kinematic;

  return return_value;
}

void RigidBody::SetFlags (size_t flags)
{
  ChangeFlags (flags, true);
}

void RigidBody::ChangeFlags (size_t flags, bool state)
{
  size_t flags_to_change = 0;

  if (flags & physics::RigidBodyFlag_FrozenPositionX)
    flags_to_change |= physics::low_level::RigidBodyFlag_FrozenPositionX;
  if (flags & physics::RigidBodyFlag_FrozenPositionY)
    flags_to_change |= physics::low_level::RigidBodyFlag_FrozenPositionY;
  if (flags & physics::RigidBodyFlag_FrozenPositionZ)
    flags_to_change |= physics::low_level::RigidBodyFlag_FrozenPositionZ;
  if (flags & physics::RigidBodyFlag_FrozenRotationX)
    flags_to_change |= physics::low_level::RigidBodyFlag_FrozenRotationX;
  if (flags & physics::RigidBodyFlag_FrozenRotationY)
    flags_to_change |= physics::low_level::RigidBodyFlag_FrozenRotationY;
  if (flags & physics::RigidBodyFlag_FrozenRotationZ)
    flags_to_change |= physics::low_level::RigidBodyFlag_FrozenRotationZ;
  if (flags & physics::RigidBodyFlag_Kinematic)
    flags_to_change |= physics::low_level::RigidBodyFlag_Kinematic;

  impl->LowLevelBody ()->SetFlags (state ? impl->LowLevelBody ()->Flags () | flags_to_change : impl->LowLevelBody ()->Flags () & ~flags_to_change);
}

/*
   Согласование объекта с группой
*/

const char* RigidBody::CollisionGroup () const
{
  throw xtl::make_not_implemented_exception ("physics::RigidBody::CollisionGroup");
}

void RigidBody::SetCollisionGroup (const char*)
{
  throw xtl::make_not_implemented_exception ("physics::RigidBody::SetCollisionGroup");
}

/*
   Приложение силы/импульса/вращения
*/

void RigidBody::AddForce (const math::vec3f& force, const math::vec3f& relative_position)
{
  impl->LowLevelBody ()->AddForce (force, relative_position);
}

void RigidBody::AddImpulse (const math::vec3f& impulse, const math::vec3f& relative_position)
{
  impl->LowLevelBody ()->AddImpulse (impulse, relative_position);
}

void RigidBody::AddTorque (const math::vec3f& torque)
{
  impl->LowLevelBody ()->AddTorque (torque);
}

/*
   Управление линейной/угловой скоростями
*/

const math::vec3f& RigidBody::LinearVelocity () const
{
  return impl->LowLevelBody ()->LinearVelocity ();
}

const math::vec3f& RigidBody::AngularVelocity () const
{
  return impl->LowLevelBody ()->AngularVelocity ();
}

void RigidBody::SetLinearVelocity (const math::vec3f& velocity)
{
  impl->LowLevelBody ()->SetLinearVelocity (velocity);
}

void RigidBody::SetAngularVelocity (const math::vec3f& velocity)
{
  impl->LowLevelBody ()->SetAngularVelocity (velocity);
}

/*
   Оповещение об изменении положения тела
*/

xtl::connection RigidBody::RegisterTransformUpdateCallback (const TransformUpdateCallback& callback_handler)
{
  throw xtl::make_not_implemented_exception ("physics::RigidBody::RegisterTransformUpdateCallback");
}

/*
   Обработка столкновений объектов
*/

xtl::connection RigidBody::RegisterCollisionCallback (const char* group_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  throw xtl::make_not_implemented_exception ("physics::RigidBody::RegisterCollisionCallback");
}


/*
   Обмен
*/

void RigidBody::Swap (RigidBody& source)
{
  stl::swap (impl, source.impl);
}

namespace physics
{

void swap (RigidBody& material1, RigidBody& material2)
{
  material1.Swap (material2);
}

}

/*
   Создание
*/

RigidBody RigidBodyImplProvider::CreateRigidBody (RigidBodyPtr body, const Shape& shape, const Material& material)
{
  return RigidBody (new RigidBodyImpl (body, shape, material));
}

RigidBody RigidBodyImplProvider::CreateRigidBody (RigidBodyImpl* impl)
{
  return RigidBody (impl);
}

/*
   Получение реализации
*/

RigidBodyImpl* RigidBodyImplProvider::Impl (const RigidBody& body)
{
  return body.impl;
}

/*
   Получение низкоуровневого тела
*/

physics::low_level::IRigidBody* RigidBodyImplProvider::LowLevelBody (const RigidBody& body)
{
  return body.impl->LowLevelBody ();
}
