#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

/*
    Описание реализации соединения тел
*/

struct Joint::Impl
{
};

/*
    Конструктор
*/

Joint::Joint ()
  : impl (new Impl)
  {}

/*
   Толщина полей
*/

/*
   Получение объектов
*/

size_t Joint::ObjectsCount ()
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Joint::ObjectsCount");
}

RigidBody* Joint::GetObject (size_t index)
{
  throw xtl::make_not_implemented_exception ("physics::low_level::bullet::Joint::GetObject");
}
