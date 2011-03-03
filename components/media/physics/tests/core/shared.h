#ifndef MEDIALIB_PHYSICS_TESTS_HEADER
#define MEDIALIB_PHYSICS_TESTS_HEADER

#include <xtl/exception.h>
#include <xtl/iterator.h>

#include <media/physics/basic_shapes.h>
#include <media/physics/compound.h>
#include <media/physics/material.h>
#include <media/physics/physics_library.h>
#include <media/physics/rigid_body.h>
#include <media/physics/shape.h>
#include <media/physics/triangle_mesh.h>

void dump (const math::vec3f& v)
{
  printf ("[%.2f %.2f %.2f]", v.x, v.y, v.z);
}

void dump (const math::quatf& q)
{
  printf ("[%.2f %.2f %.2f %.2f]", q.x, q.y, q.z, q.w);
}

#endif
