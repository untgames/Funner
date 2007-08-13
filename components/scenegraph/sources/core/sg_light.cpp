#include <sg/light.h>
#include <xtl/visitor.h>

using namespace scene_graph;
using namespace math;

/*
    Описание реализации Light
*/

struct Light::Impl
{
  vec3f color;         //цвет света
  vec3f attenuation;   //коэффициенты затухания (x - constant, y - linear, z - quadratic)
  float range;         //расстояние действия света
};

/*
    Конструктор / деструктор
*/

Light::Light ()
  : impl (new Impl)
{
  impl->color       = 0;
  impl->attenuation = 0;
  impl->range       = DEFAULT_LIGHT_RANGE;
}

Light::~Light ()
{
  delete impl;
}

/*
    Цвет источника света
*/

void Light::SetLightColor (const math::vec3f& color)
{
  impl->color = color;
}

const math::vec3f& Light::LightColor () const
{
  return impl->color;
}

/*
    Затухание (constant, linear, quadratic)
*/

void Light::SetAttenuation (const math::vec3f& multiplier)
{
  impl->attenuation = multiplier;
}

const math::vec3f& Light::Attenuation () const
{
  return impl->attenuation;
}

/*
    Расстояние действия источника света
*/

void Light::SetRange (float range)
{
  impl->range = range;
  ComputeBV ();
}

float Light::Range () const
{
  return impl->range;
}

/*
    Метод, вызываемый при посещении объекта
*/

void Light::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Entity::AcceptCore (visitor);
}
