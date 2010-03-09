#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    Описание реализации объекта рендеринга
*/

struct EntityImpl::Impl
{
};

/*
    Конструктор / деструктор
*/

EntityImpl::EntityImpl ()
  : impl (new Impl)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::EntityImpl");
}

EntityImpl::~Entity ()
{
}

/*
    Свойства рендеринга
*/

void EntityImpl::SetProperties (const common::PropertyMap& properties)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetProperties");
}

const common::PropertyMap& EntityImpl::Properties ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::Properties");
}

/*
    Матрица преобразований
*/

void EntityImpl::SetTransformation (const math::mat4f&)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetTransformation");
}

const math::vec4f& EntityImpl::Transformation ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::Transformation");
}

/*
    Работа с костями (для скиннинга)
      преобразования умножаются на матрицу EntityImpl::Transformation в случае если она не единична    
*/

void EntityImpl::SetJointsCount (size_t count)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetJointsCount");
}

size_t EntityImpl::JointsCount ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::JointsCount");
}

void EntityImpl::SetJointTransformation (size_t joint_index, const math::mat4f&)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetJointTransformation");
}

const math::mat4f& EntityImpl::JointTransformation (size_t joint_index)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::JointTransformation");
}

/*
    Работа с уровнями детализации
*/

size_t EntityImpl::LodsCount ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::LodsCount");
}

/*
    Работа с примитивом
*/

const PrimitivePtr& EntityImpl::Primitive (size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::Primitive");
}

void EntityImpl::SetPrimitive (const PrimitivePtr&, size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::SetPrimitive");
}

bool EntityImpl::HasPrimitive (size_t level_of_detail)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::HasPrimitive");
}

void EntityImpl::ResetAllPrimitives ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::EntityImpl::ResetAllPrimitives");
}

/*
    Получение обёртки
*/

Entity EntityImpl::Wrap ()
{
  return Wrappers::Wrap<Entity> (this);
}

/*
    Обёртки
*/

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

const common::PropertyMap& Properties () const
{
  return impl->Properties ();
}

void Entity::SetTransformation (const math::mat4f& tm)
{
  impl->SetTransformation (tm);
}

const math::vec4f& Entity::Transformation () const
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

const mid_level::Primitive& Entity::Primitive (size_t level_of_detail) const
{
  return impl->Primitive (level_of_detail);
}

void Entity::SetPrimitive (const mid_level::Primitive& primitive, size_t level_of_detail)
{
  impl->SetPrimitive (Wrappers::Unwrap (primitive), level_of_detail);
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

namespace mid_level
{

void swap (Entity& entity1, Entity& entity2)
{
  entity1.Swap (entity2);
}

}

}
