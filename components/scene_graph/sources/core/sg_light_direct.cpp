#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

const float LIGHT_INFINITY = 1.0e+9f;  //если радиус/расстояние источника света превышает эту величину, устанавливаются бесконечные bv

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

DirectLight::Pointer DirectLight::Create ()
{
  return Pointer (new DirectLight, false);
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
  if (Range () >= LIGHT_INFINITY || impl->radius >= LIGHT_INFINITY)
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
