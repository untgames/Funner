#include <sg/light.h>
#include <xtl/visitor.h>
#include <bv/axis_aligned_box.h>

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

const float INFINITY = 1e9;  //если радиус/расстояние источника света превышает эту величину, устанавливаются бесконечные bv

/*
    Описание реализации DirectLight
*/

struct DirectLight::Impl
{
  float radius;         //радиус действия света
};

/*
    Конструктор / деструктор
*/

DirectLight::DirectLight ()
  : impl (new Impl)
{
  impl->radius = DEFAULT_LIGHT_RADIUS;
}

DirectLight::~DirectLight ()
{
  delete impl;
}

/*
    Создание источника света
*/

DirectLight* DirectLight::Create ()
{
  return new DirectLight;
}

/*
    Радиус действия источника света
*/

void DirectLight::SetRadius (float radius)
{
  impl->radius = radius;
  UpdateBoundsNotify ();
}

float DirectLight::Radius () const
{
  return impl->radius;
}

/*
   Рассчёт ограничивающего объёма
*/

void DirectLight::UpdateBoundsCore ()
{
  if (Range () >= INFINITY || impl->radius >= INFINITY)
    SetInfiniteBounds ();
  else
    SetBoundBox (axis_aligned_box <float> (-impl->radius, -impl->radius, 0, impl->radius, impl->radius, Range ()));
}

/*
    Метод, вызываемый при посещении объекта
*/

void DirectLight::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Light::AcceptCore (visitor);
}
