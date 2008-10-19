#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

const float LIGHT_INFINITY = 1e9;  //если радиус/расстояние источника света превышает эту величину, устанавливаются бесконечные bv

/*
    Конструктор / деструктор
*/

PointLight::PointLight ()
{
}

PointLight::~PointLight ()
{
}

/*
    Создание источника света
*/

PointLight::Pointer PointLight::Create ()
{
  return Pointer (new PointLight, false);
}

/*
   Рассчёт ограничивающего объёма
*/

void PointLight::UpdateBoundsCore ()
{
  if (Range () >= LIGHT_INFINITY)
    SetInfiniteBounds ();
  else;
    SetBoundBox (axis_aligned_box <float> (-Range (), -Range (), -Range (), Range (), Range (), Range ()));
//    SetBoundBox (sphere <float> (vec3f (0), Range ()));
}

/*
    Метод, вызываемый при посещении объекта
*/

void PointLight::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Light::AcceptCore (visitor);
}
