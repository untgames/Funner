#include "shared.h"

using namespace physics;

/*
   Твердое тело
*/

struct RigidBody::Impl : public xtl::reference_counter
{
  RigidBodyPtr         body;
  Transform            current_transform;
  xtl::auto_connection transform_update_connection;

  Impl (RigidBodyPtr in_body)
    : body (in_body)
  {
    transform_update_connection = body->RegisterTransformUpdateCallback (xtl::bind (&RigidBody::Impl::OnTransformUpdate, this));

    OnTransformUpdate ();
  }

  void OnTransformUpdate ()
  {
    const physics::low_level::Transform& body_transform = body->WorldTransform ();

    current_transform.position    = body_transform.position;
    current_transform.orientation = body_transform.orientation;
  }
};

/*
   Конструктор / деструктор / копирование
*/

RigidBody::RigidBody (const RigidBody& source)
  : impl (source.impl)
{
  addref (impl);
}

RigidBody::RigidBody (Impl* source_impl)
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
  return impl->body->Mass ();
}

void RigidBody::SetMass (float mass)
{
  impl->body->SetMass (mass);
}

/*
   Управление центром масс
*/

const math::vec3f& RigidBody::CenterOfMassPosition () const
{
  throw xtl::make_not_implemented_exception ("physics::RigidBody::CenterOfMassPosition");
}

void RigidBody::SetCenterOfMassPosition (const math::vec3f& center)
{
  throw xtl::make_not_implemented_exception ("physics::RigidBody::SetCenterOfMassPosition");
}

const math::quatf& RigidBody::CenterOfMassOrientation () const
{
  throw xtl::make_not_implemented_exception ("physics::RigidBody::CenterOfMassOrientation");
}

void RigidBody::SetCenterOfMassOrientation (const math::quatf& orientation)
{
  throw xtl::make_not_implemented_exception ("physics::RigidBody::SetCenterOfMassOrientation");
}

/*
   Управление тензором
*/

const math::vec3f& RigidBody::MassSpaceInertiaTensor () const
{
  return impl->body->MassSpaceInertiaTensor ();
}

void RigidBody::SetMassSpaceInertiaTensor (const math::vec3f& tensor)
{
  impl->body->SetMassSpaceInertiaTensor (tensor);
}

/*
   Мировое положение
*/

const Transform& RigidBody::WorldTransform () const
{
  return impl->current_transform;
}

void RigidBody::SetWorldTransform (const Transform& transform)
{
  physics::low_level::Transform body_transform;

  body_transform.position    = transform.position;
  body_transform.orientation = transform.orientation;

  impl->body->SetWorldTransform (body_transform);
}

/*
   Управление сном
*/

float RigidBody::SleepLinearVelocity () const
{
  return impl->body->SleepLinearVelocity ();
}

float RigidBody::SleepAngularVelocity () const
{
  return impl->body->SleepAngularVelocity ();
}

void RigidBody::SetSleepLinearVelocity (float value)
{
  impl->body->SetSleepLinearVelocity (value);
}

void RigidBody::SetSleepAngularVelocity (float value)
{
  impl->body->SetSleepAngularVelocity (value);
}

/*
   Управление пороговым значением скорости перехода в режим непрерывного движения
*/

float RigidBody::CcdMotionThreshold () const
{
  return impl->body->CcdMotionThreshold ();
}

void RigidBody::SetCcdMotionThreshold (float value)
{
  impl->body->SetCcdMotionThreshold (value);
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
  throw xtl::make_not_implemented_exception ("physics::RigidBody::Shape");
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
  throw xtl::make_not_implemented_exception ("physics::RigidBody::Material");
}

void RigidBody::SetMaterial (const physics::Material& material)
{
  throw xtl::make_not_implemented_exception ("physics::RigidBody::SetMaterial");
}

/*
   Флаги поведения
*/

size_t RigidBody::Flags () const
{
  size_t return_value = 0, body_flags = impl->body->Flags ();

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

  impl->body->SetFlags (state ? impl->body->Flags () | flags_to_change : impl->body->Flags () & ~flags_to_change);
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
  impl->body->AddForce (force, relative_position);
}

void RigidBody::AddImpulse (const math::vec3f& impulse, const math::vec3f& relative_position)
{
  impl->body->AddImpulse (impulse, relative_position);
}

void RigidBody::AddTorque (const math::vec3f& torque)
{
  impl->body->AddTorque (torque);
}

/*
   Управление линейной/угловой скоростями
*/

const math::vec3f& RigidBody::LinearVelocity () const
{
  return impl->body->LinearVelocity ();
}

const math::vec3f& RigidBody::AngularVelocity () const
{
  return impl->body->AngularVelocity ();
}

void RigidBody::SetLinearVelocity (const math::vec3f& velocity)
{
  impl->body->SetLinearVelocity (velocity);
}

void RigidBody::SetAngularVelocity (const math::vec3f& velocity)
{
  impl->body->SetAngularVelocity (velocity);
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

RigidBody RigidBodyImplProvider::CreateRigidBody (RigidBodyPtr body)
{
  return RigidBody (new RigidBody::Impl (body));
}

/*
   Получение низкоуровневого тела
*/

physics::low_level::IRigidBody* RigidBodyImplProvider::LowLevelBody (const RigidBody& body)
{
  return body.impl->body.get ();
}
