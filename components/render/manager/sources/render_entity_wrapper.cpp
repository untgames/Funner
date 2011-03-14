#include "shared.h"

using namespace render;

Entity::Entity (EntityImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

Entity::Entity (const Entity& entity)
  : impl (entity.impl)
{
  addref (impl);
}

Entity::~Entity ()
{
  release (impl);
}

Entity& Entity::operator = (const Entity& entity)
{
  Entity (entity).Swap (*this);
  return *this;
}

void Entity::SetProperties (const common::PropertyMap& properties)
{
  return impl->SetProperties (properties);  
}

const common::PropertyMap& Entity::Properties () const
{
  return impl->Properties ();
}

void Entity::SetTransformation (const math::mat4f& tm)
{
  impl->SetTransformation (tm);
}

const math::mat4f& Entity::Transformation () const
{
  return impl->Transformation ();
}

void Entity::SetJointsCount (size_t count)
{
  impl->SetJointsCount (count);
}

size_t Entity::JointsCount () const
{
  return impl->JointsCount ();
}

void Entity::SetJointTransformation (size_t joint_index, const math::mat4f& tm)
{
  impl->SetJointTransformation (joint_index, tm);
}

const math::mat4f& Entity::JointTransformation (size_t joint_index) const
{
  return impl->JointTransformation (joint_index);
}

size_t Entity::LodsCount () const
{
  return impl->LodsCount ();
}

Primitive Entity::Primitive (size_t level_of_detail) const
{
  return Wrappers::Wrap<render::Primitive> (impl->Primitive (level_of_detail));
}

const char* Entity::PrimitiveName (size_t level_of_detail) const
{
  return impl->PrimitiveName (level_of_detail);
}

void Entity::SetPrimitive (const render::Primitive& primitive, size_t level_of_detail)
{
  impl->SetPrimitive (Wrappers::Unwrap<PrimitiveImpl> (primitive), level_of_detail);
}

void Entity::SetPrimitive (const char* name, size_t level_of_detail)
{
  impl->SetPrimitive (name, level_of_detail);
}

void Entity::ResetPrimitive (size_t level_of_detail)
{
  impl->ResetPrimitive (level_of_detail);
}

bool Entity::HasPrimitive (size_t level_of_detail) const
{
  return impl->HasPrimitive (level_of_detail);
}

void Entity::ResetAllPrimitives ()
{
  impl->ResetAllPrimitives ();
}

void Entity::Swap (Entity& entity)
{
  stl::swap (impl, entity.impl);
}

namespace render
{

void swap (Entity& entity1, Entity& entity2)
{
  entity1.Swap (entity2);
}

}
