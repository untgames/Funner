#include "shared.h"

using namespace render::manager;

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

void Entity::SetWorldMatrix (const math::mat4f& tm)
{
  impl->SetWorldMatrix (tm);
}

const math::mat4f& Entity::WorldMatrix () const
{
  return impl->WorldMatrix ();
}

const math::mat4f& Entity::InverseWorldMatrix () const
{
  return impl->InverseWorldMatrix ();
}

void Entity::SetProperties (const common::PropertyMap& properties)
{
  return impl->SetProperties (properties);  
}

const common::PropertyMap& Entity::Properties () const
{
  return impl->Properties ();
}

void Entity::SetShaderOptions (const common::PropertyMap& defines)
{
  impl->SetShaderOptions (defines);
}

const common::PropertyMap& Entity::ShaderOptions () const
{
  return impl->ShaderOptions ();
}

void Entity::SetUserData (void* data)
{
  impl->SetUserData (data);
}

void* Entity::UserData () const
{
  return impl->UserData ();
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
  return Wrappers::Wrap<render::manager::Primitive> (impl->Primitive (level_of_detail));
}

const char* Entity::PrimitiveName (size_t level_of_detail) const
{
  return impl->PrimitiveName (level_of_detail);
}

void Entity::SetPrimitive (const render::manager::Primitive& primitive, size_t level_of_detail)
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

void Entity::SetLodPoint (const math::vec3f& p)
{
  impl->SetLodPoint (p);
}

const math::vec3f& Entity::LodPoint () const
{
  return impl->LodPoint ();
}

void Entity::SetScissor (const BoxArea& scissor)
{
  impl->SetScissor (scissor);
}

const BoxArea& Entity::Scissor () const
{
  return impl->Scissor ();
}

void Entity::SetScissorState (bool state)
{
  impl->SetScissorState (state);
}

bool Entity::ScissorState () const
{
  return impl->ScissorState ();
}

void Entity::Swap (Entity& entity)
{
  stl::swap (impl, entity.impl);
}

namespace render
{

namespace manager
{

void swap (Entity& entity1, Entity& entity2)
{
  entity1.Swap (entity2);
}

}

}

void Entity::UpdateCache ()
{
  impl->UpdateCache ();
}

void Entity::ResetCache ()
{
  impl->ResetCache ();
}
