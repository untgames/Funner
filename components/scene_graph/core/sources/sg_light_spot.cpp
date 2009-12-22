#include "shared.h"

using namespace scene_graph;
using namespace math;
using namespace bound_volumes;

const float LIGHT_INFINITY = 1e9;  //если радиус/расстояние источника света превышает эту величину, устанавливаются бесконечные bv

/*
    Описание реализации SpotLight
*/

struct SpotLight::Impl
{
  anglef angle;         //угол света
  float  exponent;      //экспонента рассеивания по углу
};

/*
    Конструктор / деструктор
*/

SpotLight::SpotLight ()
  : impl (new Impl)
{
  impl->angle    = radian (0.0f);
  impl->exponent = 0;
}

SpotLight::~SpotLight ()
{
  delete impl;
}

/*
    Создание источника света
*/

SpotLight::Pointer SpotLight::Create ()
{
  return Pointer (new SpotLight, false);
}

/*
    Параметры действия источника света
*/

void SpotLight::SetAngle (const math::anglef& angle)
{
  impl->angle = angle;
  UpdateBoundsNotify ();
}

const math::anglef& SpotLight::Angle () const
{
  return impl->angle;
}

void SpotLight::SetExponent (float exponent)
{
  impl->exponent = exponent;
}

float SpotLight::Exponent () const
{
  return impl->exponent;
}

/*
   Рассчёт ограничивающего объёма
*/

void SpotLight::UpdateBoundsCore ()
{
  if (Range () >= LIGHT_INFINITY)
    SetInfiniteBounds ();
  else
  {
    float half_width = tan (impl->angle / 2) * Range (); 
  
    SetBoundBox (axis_aligned_box <float> (-half_width, -half_width, 0, half_width, half_width, Range ()));
  }
}

/*
    Метод, вызываемый при посещении объекта
*/

void SpotLight::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Light::AcceptCore (visitor);
}
