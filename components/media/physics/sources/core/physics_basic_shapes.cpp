#include "shared.h"

using namespace media::physics::shapes;

/*
   Описание тела в виде параллелепипеда
*/

Box::Box ()
  : half_dimensions (1.f)
  {}

Box::Box (const math::vec3f& in_half_dimensions)
  : half_dimensions (in_half_dimensions)
  {}

/*
   Описание тела в виде сферы
*/

Sphere::Sphere ()
  : radius (1.f)
  {}

Sphere::Sphere (float in_radius)
  : radius (in_radius)
  {}

/*
   Описание тела в виде капсулы
*/

Capsule::Capsule ()
  : radius (1.f), height (1.f)
  {}

Capsule::Capsule (float in_radius, float in_height)
  : radius (in_radius), height (in_height)
  {}

/*
   Описание тела в виде плоскости
*/

Plane::Plane ()
  : normal (0.f, 1.f, 0.f), d (0.f)
  {}

Plane::Plane (const math::vec3f& in_normal, float in_d)
  : normal (in_normal), d (in_d)
  {}
