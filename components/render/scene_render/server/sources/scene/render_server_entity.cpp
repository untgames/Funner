#include "shared.h"

using namespace render::scene;
using namespace render::scene::server;

/*
    Описание реализации сущности
*/

struct Entity::Impl
{
  bool                  is_infinite; //бесконечен ли ограничивающий объем
  bound_volumes::aaboxf bound_box;   //ограничивающий бокс

///Конструктор
  Impl () : is_infinite (true) {}
};

/*
    Конструкторы / деструктор / присваивание
*/

Entity::Entity ()
  : impl (new Impl)
{
}

Entity::~Entity ()
{
}

/*
    Ограничивающее тело
*/

void Entity::SetBounds (bool is_infinite, const bound_volumes::aaboxf& box)
{
  impl->is_infinite = true;
  impl->bound_box   = box;
}

bool Entity::IsInfiniteBounds () const
{
  return impl->is_infinite;
}

const bound_volumes::aaboxf& Entity::BoundBox () const
{
  return impl->bound_box;
}

/*
    Обход
*/

void Entity::VisitCore (ISceneVisitor& visitor)
{
  visitor.Visit (*this);
}
