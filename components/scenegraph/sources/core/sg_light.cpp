#include <sg/light.h>
#include <xtl/visitor.h>

using namespace sg;
using namespace math;

/*
    Описание реализации Light
*/

struct Light::Impl
{
  LightType type;          //тип источника света
  vec3f     color;         //цвет света
  float     spot_angle;    //угол (для Spot)
  float     spot_exponent; //экспонента затухания по углу (для Spot)
  vec3f     attenuation;   //коэффициенты затухания (x - constant, y - linear, z - quadratic)
  float     range;         //расстояние действия света
};

/*
    Конструктор / деструктор
*/

Light::Light ()
  : impl (new Impl)
{
  impl->type          = LightType_Point;
  impl->color         = 0;
  impl->spot_angle    = 0;
  impl->spot_exponent = 0;
  impl->attenuation   = 0;
  impl->range         = DEFAULT_LIGHT_RANGE;
}

Light::~Light ()
{
  delete impl;
}

/*
    Создание источника света
*/

Light* Light::Create ()
{
  return new Light;
}

/*
    Тип источника света
*/

void Light::SetType (LightType type)
{
  impl->type = type;
}

LightType Light::Type () const
{
  return impl->type;
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
    Параметры источника света
*/

void Light::SetSpotAngle (float angle)
{
  impl->spot_angle = angle;
}

float Light::SpotAngle () const
{
  return impl->spot_angle;
}

void Light::SetSpotExponent (float exponent)
{
  impl->spot_exponent = exponent;
}

float Light::SpotExponent () const
{
  return impl->spot_exponent;
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
}

float Light::Range () const
{
  return impl->range;
}

/*
    Динамическая диспетчеризация    
*/

void Light::AcceptCore (Visitor& visitor)
{
  if (!TryAccept (*this, visitor))
    Node::AcceptCore (visitor);
}
