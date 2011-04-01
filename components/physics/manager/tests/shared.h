#ifndef PHYSICS_MANAGER_TESTS_SHARED_HEADER
#define PHYSICS_MANAGER_TESTS_SHARED_HEADER

#include <cstdio>

#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>

#include <media/physics/shape.h>
#include <media/physics/triangle_mesh.h>

#include <render/debug_render.h>

#include <physics/manager.h>

using namespace physics;

const char* DRIVER_NAME = "Bullet";

void convert_float (float& value, int rounding)
{
  value = ceil ((float)(int)(value * rounding)) / (float)rounding;

  if (fabs (value) < 0.01f)
    value = 0.f;
}

void dump_body_position (const RigidBody& body)
{
  const Transform& body_transform = body.WorldTransform ();

  printf ("Body world transform:\n");

  math::vec3f world_position = body_transform.position;

  for (size_t i = 0; i < 3; i++)
    convert_float (world_position [i], 1);

  printf ("  position = %.0f; %.0f; %.0f\n", world_position.x, world_position.y, world_position.z);

  math::quatf world_orientation = body_transform.orientation;

  for (size_t i = 0; i < 4; i++)
    convert_float (world_orientation [i], 10);

  printf ("  orientation = %.1f; %.1f; %.1f; %.1f\n", world_orientation.x, world_orientation.y, world_orientation.z, world_orientation.w);
}

#endif
