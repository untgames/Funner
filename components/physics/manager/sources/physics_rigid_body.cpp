#include "shared.h"

using namespace physics;

/*
   Твердое тело
*/

RigidBodyImpl::RigidBodyImpl (RigidBodyPtr in_body, const physics::Shape& in_shape, const physics::Material& in_material, const Scene& in_scene)
  : scene (in_scene), body (in_body), shape (in_shape), material (in_material)
{
  body->SetMaterial (MaterialImplProvider::LowLevelMaterial (material));

  transform_update_connection = body->RegisterTransformUpdateCallback (xtl::bind (&RigidBodyImpl::OnTransformUpdate, this));

  OnTransformUpdate ();

  SetCollisionGroup ("");
}

/*
   Согласование объекта с группой
*/

const char* RigidBodyImpl::CollisionGroup ()
{
  return collision_group.c_str ();
}

void RigidBodyImpl::SetCollisionGroup (const char* new_group)
{
  if (!new_group)
    throw xtl::make_null_argument_exception ("physics::RigidBodyImpl::SetCollisionGroup", "new_group");

  body->SetCollisionGroup (SceneImplProvider::CollisionGroupForName (scene, new_group));

  collision_group = new_group;
}

/*
   Получение текущей позиции
*/

const Transform& RigidBodyImpl::CurrentTransform ()
{
  return current_transform;
}

void RigidBodyImpl::SetWorldTransform (const Transform& transform)
{
  physics::low_level::Transform body_transform;

  body_transform.position    = transform.position;
  body_transform.orientation = transform.orientation;

  current_transform = transform;

  body->SetWorldTransform (body_transform);
}

/*
   Обработка изменения позиции
*/

void RigidBodyImpl::OnTransformUpdate ()
{
  const physics::low_level::Transform& body_transform = body->WorldTransform ();

  current_transform.position    = body_transform.position;
  current_transform.orientation = body_transform.orientation;

  transform_update_signal (RigidBodyImplProvider::CreateRigidBody (this));
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

void RigidBodyImpl::SetMaterial (const physics::Material& in_material)
{
  body->SetMaterial (MaterialImplProvider::LowLevelMaterial (in_material));

  material = in_material;
}

/*
   Получение низкоуровневого тела
*/

physics::low_level::IRigidBody* RigidBodyImpl::LowLevelBody ()
{
  return body.get ();
}

/*
   Подписка на обновление положения тела
*/

xtl::connection RigidBodyImpl::RegisterTransformUpdateCallback (const RigidBody::TransformUpdateCallback& callback_handler)
{
  return transform_update_signal.connect (callback_handler);
}

/*
   Подписка на столкновения тела
*/

xtl::connection RigidBodyImpl::RegisterCollisionCallback (const char* group_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  static const char* METHOD_NAME = "physics::RigidBodyImpl::RegisterCollisionCallback";

  if (!group_mask)
    throw xtl::make_null_argument_exception (METHOD_NAME, "group_mask");

  if (event_type >= CollisionEventType_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "event_type", event_type);

  if (!xtl::xstrcmp ("*", group_mask))
    return collision_signal [event_type].connect (xtl::bind (callback_handler, _1, _2, _3, _4));
  else if (!strstr (group_mask, "*") && !strstr (group_mask, "?"))
    return collision_signal [event_type].connect (xtl::bind (&RigidBodyImpl::ProcessCollisionWithHash, this, _1, _2, _3, _4, common::strhash (group_mask), callback_handler));
  else
    return collision_signal [event_type].connect (xtl::bind (&RigidBodyImpl::ProcessCollisionWithMask, this, _1, _2, _3, _4, stl::string (group_mask), callback_handler));
}

/*
   Оповещение о столкновения тела
*/

void RigidBodyImpl::OnCollision (CollisionEventType event_type, RigidBody& second_body, const math::vec3f& collision_point)
{
  if (event_type >= CollisionEventType_Num)
    throw xtl::make_argument_exception ("physics::RigidBodyImpl::OnCollision", "event_type", event_type);

  RigidBody this_body (RigidBodyImplProvider::CreateRigidBody (this));

  collision_signal [event_type] (event_type, this_body, second_body, collision_point);
}

/*
   Обработка события о столкновения тела
*/

void RigidBodyImpl::ProcessCollisionWithMask (CollisionEventType event_type, RigidBody& this_body, RigidBody& second_body,
                                              const math::vec3f& collision_point, const stl::string& wanted_group_mask,
                                              const CollisionCallback& callback_handler)
{
  if (!common::wcmatch (second_body.CollisionGroup (), wanted_group_mask.c_str ()))
    return;

  callback_handler (event_type, this_body, second_body, collision_point);
}

void RigidBodyImpl::ProcessCollisionWithHash (CollisionEventType event_type, RigidBody& this_body, RigidBody& second_body,
                                              const math::vec3f& collision_point, size_t wanted_group_hash,
                                              const CollisionCallback& callback_handler)
{
  if (common::strhash (second_body.CollisionGroup ()) != wanted_group_hash)
    return;

  callback_handler (event_type, this_body, second_body, collision_point);
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
   Идентификатор
*/

size_t RigidBody::Id () const
{
  return (size_t)impl;
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
  impl->SetWorldTransform (transform);
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
  return impl->CollisionGroup ();
}

void RigidBody::SetCollisionGroup (const char* new_group)
{
  impl->SetCollisionGroup (new_group);
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
  return impl->RegisterTransformUpdateCallback (callback_handler);
}

/*
   Обработка столкновений объектов
*/

xtl::connection RigidBody::RegisterCollisionCallback (const char* group_mask, CollisionEventType event_type, const CollisionCallback& callback_handler)
{
  return impl->RegisterCollisionCallback (group_mask, event_type, callback_handler);
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

RigidBody RigidBodyImplProvider::CreateRigidBody (RigidBodyPtr body, const Shape& shape, const Material& material, const Scene& scene)
{
  return RigidBody (new RigidBodyImpl (body, shape, material, scene));
}

RigidBody RigidBodyImplProvider::CreateRigidBody (RigidBodyImpl* impl)
{
  addref (impl);

  try
  {
    return RigidBody (impl);
  }
  catch (...)
  {
    release (impl);
    throw;
  }
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
