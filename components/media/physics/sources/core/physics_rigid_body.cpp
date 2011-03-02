#include "shared.h"

using namespace media::physics;

/*
   Реализация твердого тела
*/

struct RigidBody::Impl : public xtl::reference_counter
{
  stl::string              name;                       //имя тела
  float                    mass;                       //масса
  math::vec3f              center_of_mass_position;    //положение центра масс
  math::quatf              center_of_mass_orientation; //ориентация центра масс
  math::vec3f              mass_space_inertia_tensor;  //тензор
  media::physics::Shape    shape;                      //геометрическое тело
  media::physics::Material material;                   //материал
  size_t                   flags;                      //флаги поведения

  Impl ()
    : mass (0.f)
    , flags (0)
    {}

  Impl (const Impl& source)
    : name                       (source.name)
    , mass                       (source.mass)
    , center_of_mass_position    (source.center_of_mass_position)
    , center_of_mass_orientation (source.center_of_mass_orientation)
    , mass_space_inertia_tensor  (source.mass_space_inertia_tensor)
    , shape                      (source.shape.Clone ())
    , material                   (source.material.Clone ())
    , flags                      (source.flags)
    {}
};

/*
   Конструкторы / деструктор / присваивание
*/

RigidBody::RigidBody ()
  : impl (new Impl)
{
}

RigidBody::RigidBody (const RigidBody& source)
  : impl (source.impl)
{
  addref (impl);
}

RigidBody::RigidBody (Impl* in_impl)
  : impl (in_impl)
{
}

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
   Создание копии
*/

RigidBody RigidBody::Clone () const
{
  return RigidBody (new Impl (*impl));
}

/*
   Идентификатор
*/

size_t RigidBody::Id () const
{
  return (size_t)impl;
}

/*
   Имя тела
*/

const char* RigidBody::Name () const
{
  return impl->name.c_str ();
}

void RigidBody::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::physics::RigidBody::Rename", "name");

  impl->name = name;
}
    
/*
   Масса
*/

float RigidBody::Mass () const
{
  return impl->mass;
}

void RigidBody::SetMass (float mass)
{
  impl->mass = mass;
}

/*
   Управление центром масс
*/

const math::vec3f& RigidBody::CenterOfMassPosition () const
{
  return impl->center_of_mass_position;
}

void RigidBody::SetCenterOfMassPosition (const math::vec3f& center)
{
  impl->center_of_mass_position = center;
}

const math::quatf& RigidBody::CenterOfMassOrientation () const
{
  return impl->center_of_mass_orientation;
}

void RigidBody::SetCenterOfMassOrientation (const math::quatf& orientation)
{
  impl->center_of_mass_orientation = orientation;
}

/*
   Управление тензором
*/

const math::vec3f& RigidBody::MassSpaceInertiaTensor () const
{
  return impl->mass_space_inertia_tensor;
}

void RigidBody::SetMassSpaceInertiaTensor (const math::vec3f& tensor)
{
  impl->mass_space_inertia_tensor = tensor;
}

/*
   Геометрическое тело
*/

const media::physics::Shape& RigidBody::Shape () const
{
  return impl->shape;
}

void RigidBody::SetShape (const media::physics::Shape& shape)
{
  impl->shape = shape;
}

/*
   Материал
*/

const media::physics::Material& RigidBody::Material () const
{
  return impl->material;
}

void RigidBody::SetMaterial (const media::physics::Material& material)
{
  impl->material = material;
}

/*
   Флаги поведения
*/

size_t RigidBody::Flags () const
{
  return impl->flags;
}

void RigidBody::SetFlags (size_t flags)
{
  impl->flags = flags;
}

/*
   Обмен
*/

void RigidBody::Swap (RigidBody& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace physics
{

/*
   Обмен
*/

void swap (RigidBody& body1, RigidBody& body2)
{
  body1.Swap (body2);
}

}

}
