#include "shared.h"

using namespace scene_graph;
using namespace scene_graph::controllers;

namespace
{

const float ACCELERATION_EPS = 0.1f;
const float EPS              = 0.01f;
const float SPEED_EQUAL_EPS  = 0.1f;

}

///Рассчет ускорения с использованием ускорения, замедления, максимальной скорости
struct LinearAccelerationEvaluator::Impl: public xtl::reference_counter
{
  float       acceleration;                  //ускорение разгона
  float       deceleration;                  //ускорение торможения
  float       max_speed;                     //максимальная скорость
  float       braking_distance;              //тормозной путь
  bool        needs_recalc_braking_distance; //необходимо ли обновить тормозной путь
  math::vec3f return_acceleration;           //поле возвращаемое при рассчете ускорения

  Impl ()
    : acceleration (1.f)
    , deceleration (1.f)
    , max_speed (0.f)
    , needs_recalc_braking_distance (true)
    {}

  const math::vec3f& CalculateAcceleration (const math::vec3f& current_speed, const math::vec3f& distance, float dt)
  {
    if (needs_recalc_braking_distance)
    {
      braking_distance = max_speed * max_speed / deceleration / 2.f;

      needs_recalc_braking_distance = false;
    }

    float distance_length      = math::length (distance),
          desired_speed_module = 0,
          current_speed_module = math::length (current_speed);

    if (distance_length > braking_distance)
      desired_speed_module = max_speed;
    else
      desired_speed_module = max_speed * (distance_length / braking_distance);

    math::vec3f normalized_current_speed = math::normalize (current_speed);

    if (!distance_length)  //торможение
    {
      if (!current_speed_module)
        return_acceleration = 0.f;
      else
        return_acceleration = -normalized_current_speed * (current_speed_module > deceleration * dt ? deceleration : current_speed_module / dt);
    }
    else
    {
      math::vec3f normalized_distance = math::normalize (distance);

      return_acceleration = math::equal (normalized_distance, normalized_current_speed, EPS) ? normalized_distance : math::normalize (normalized_distance - normalized_current_speed);

      bool accelerating = desired_speed_module >= (current_speed_module + SPEED_EQUAL_EPS);

      if (accelerating)
        return_acceleration *= acceleration;
      else
        return_acceleration *= deceleration;

      float new_speed = math::length (current_speed + acceleration * dt);

      if (accelerating && new_speed > max_speed)
        return_acceleration = (normalized_distance * max_speed - current_speed) / dt;
    }

    return return_acceleration;
  }
};

///Конструктор / деструктор / копирование
LinearAccelerationEvaluator::LinearAccelerationEvaluator ()
  : impl (new Impl ())
{
}

LinearAccelerationEvaluator::LinearAccelerationEvaluator (const LinearAccelerationEvaluator& fn)
  : impl (fn.impl)
{
  addref (impl);
}

LinearAccelerationEvaluator::~LinearAccelerationEvaluator ()
{
  release (impl);
}

/*
   Рассчет ускорения
*/

const math::vec3f& LinearAccelerationEvaluator::operator () (const math::vec3f& current_speed, const math::vec3f& distance, float dt) const
{
  return impl->CalculateAcceleration (current_speed, distance, dt);
}

/*
   Установка/получение ускорения разгона
*/

void LinearAccelerationEvaluator::SetAcceleration (float acceleration)
{
  if (acceleration < ACCELERATION_EPS)
    throw xtl::make_argument_exception ("scene_graph::controllers::LinearAccelerationEvaluator::SetAcceleration", "acceleration", acceleration, "Too low acceleration");

  impl->acceleration = acceleration;
}

float LinearAccelerationEvaluator::Acceleration () const
{
  return impl->acceleration;
}

/*
   Установка/получение ускорения замедления
*/

void LinearAccelerationEvaluator::SetDeceleration (float deceleration)
{
  if (deceleration < ACCELERATION_EPS)
    throw xtl::make_argument_exception ("scene_graph::controllers::LinearAccelerationEvaluator::SetDeceleration", "deceleration", deceleration, "Too low deceleration");

  impl->deceleration = deceleration;
  impl->needs_recalc_braking_distance = true;
}

float LinearAccelerationEvaluator::Deceleration () const
{
  return impl->deceleration;
}

/*
   Установка/получение максимальной скорости
*/

void LinearAccelerationEvaluator::SetMaxSpeed (float max_speed)
{
  impl->max_speed = max_speed;
  impl->needs_recalc_braking_distance = true;
}

float LinearAccelerationEvaluator::MaxSpeed () const
{
  return impl->max_speed;
}
