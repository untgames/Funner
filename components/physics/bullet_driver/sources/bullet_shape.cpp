#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

/*
    Описание реализации геометрического тела
*/

struct Shape::Impl
{
  btCollisionShape* collision_shape;

  Impl (btCollisionShape* shape)
    : collision_shape (shape)
  {
    if (!shape)
      throw xtl::make_null_argument_exception ("physics::low_level::bullet::Shape::Shape", "shape");
  }

  ~Impl ()
  {
    delete collision_shape;
  }
};

/*
    Конструктор
*/

Shape::Shape (btCollisionShape* shape)
  : impl (new Impl (shape))
  {}

/*
   Толщина полей
*/

float Shape::Margin ()
{
  return impl->collision_shape->getMargin ();
}

void Shape::GetMargin (float value)
{
  impl->collision_shape->setMargin (value);
}

/*
   Получение bullet тела
*/

btCollisionShape* Shape::BulletCollisionShape ()
{
  return impl->collision_shape;
}
