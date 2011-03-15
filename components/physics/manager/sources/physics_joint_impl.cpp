#include "shared.h"

using namespace physics;

/*
   Соединение тел
*/

struct JointImpl::Impl : public xtl::reference_counter
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

JointImpl::JointImpl (JointPtr joint, const RigidBodyArray& bodies)
  : impl (new Impl (joint, bodies))
  {}

JointImpl::JointImpl (const JointImpl& source)
  : impl (source.impl)
{
  addref (impl);
}

JointImpl::~JointImpl ()
{
  release (impl);
}

JointImpl& JointImpl::operator = (const JointImpl& source)
{
  JointImpl (source).Swap (*this);

  return *this;
}

/*
   Получение соединенных тел
*/

size_t JointImpl::BodiesCount () const
{
  return impl->bodies.size ();
}

const RigidBody& JointImpl::Body (size_t index) const
{
  return const_cast<JointImpl&> (*this).Body (index);
}

RigidBody& JointImpl::Body (size_t index)
{
  if (index >= impl->bodies.size ())
    throw xtl::make_range_exception ("physics::JointImpl::Body", "index", index, 0u, impl->bodies.size ());

  return impl->bodies [index];
}

/*
   Обмен
*/

void JointImpl::Swap (JointImpl& joint)
{
  stl::swap (impl, joint.impl);
}

