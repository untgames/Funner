#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;

/*
    Описание реализации геометрического тела
*/

/*
    Конструктор
*/

Shape::Shape (btCollisionShape* shape)
  : collision_shape (shape)
  {}

Shape::~Shape ()
{
  delete collision_shape;
}

/*
   Толщина полей
*/

float Shape::Margin ()
{
  return collision_shape->getMargin ();
}

void Shape::GetMargin (float value)
{
  collision_shape->setMargin (value);
}

/*
   Получение bullet тела
*/

btCollisionShape* Shape::BulletCollisionShape ()
{
  return collision_shape;
}
