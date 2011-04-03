#include "shared.h"

using namespace physics;

/*
   Описание точки привязки соединения в теле
*/

JointBind::JointBind (const RigidBody& in_body, const math::vec3f& in_anchor, const math::vec3f& in_axis)
  : body (in_body), anchor (in_anchor), axis (in_axis)
  {}

JointBind::JointBind (const RigidBody& in_body, const math::vec3f& in_axis)
  : body (in_body), axis (in_axis)
  {}

/*
   Соединение тел
*/

/*
   Соединение тел
*/

struct Joint::Impl : public xtl::reference_counter
{
  JointPtr       joint;
  RigidBodyArray bodies;

  Impl (JointPtr in_joint, const RigidBodyArray& in_bodies)
    : joint (in_joint), bodies (in_bodies)
    {}
};

/*
   Конструктор / деструктор / копирование
*/

Joint::Joint (Impl* source_impl)
  : impl (source_impl)
  {}

Joint::Joint (const Joint& source)
  : impl (source.impl)
{
  addref (impl);
}

Joint::~Joint ()
{
  release (impl);
}

Joint& Joint::operator = (const Joint& source)
{
  Joint (source).Swap (*this);

  return *this;
}

/*
   Идентификатор
*/

size_t Joint::Id () const
{
  return (size_t)impl;
}

/*
   Получение соединенных тел
*/

size_t Joint::BodiesCount () const
{
  return impl->bodies.size ();
}

const RigidBody& Joint::Body (size_t index) const
{
  return const_cast<Joint&> (*this).Body (index);
}

RigidBody& Joint::Body (size_t index)
{
  if (index >= impl->bodies.size ())
    throw xtl::make_range_exception ("physics::Joint::Body", "index", index, 0u, impl->bodies.size ());

  return impl->bodies [index];
}

/*
   Обмен
*/

void Joint::Swap (Joint& joint)
{
  stl::swap (impl, joint.impl);
}

namespace physics
{

/*
   Обмен
*/

void swap (Joint& joint1, Joint& joint2)
{
  joint1.Swap (joint2);
}

}

/*
   Создание
*/

Joint JointImplProvider::CreateJoint (JointPtr joint, const RigidBodyArray& bodies)
{
  return Joint (new Joint::Impl (joint, bodies));
}
