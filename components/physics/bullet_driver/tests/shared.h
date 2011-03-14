#ifndef PHYSICS_BULLET_DRIVER_TESTS_SHARED_HEADER
#define PHYSICS_BULLET_DRIVER_TESTS_SHARED_HEADER

#include <cstdio>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <physics/low_level/driver.h>
#include <physics/low_level/joints.h>
#include <physics/low_level/material.h>
#include <physics/low_level/rigid_body.h>
#include <physics/low_level/scene.h>
#include <physics/low_level/shape.h>

#include <render/debug_render.h>

using namespace physics::low_level;

const char* DRIVER_NAME = "Bullet";

typedef xtl::com_ptr<IJoint>     JointPtr;
typedef xtl::com_ptr<IMaterial>  MaterialPtr;
typedef xtl::com_ptr<IRigidBody> RigidBodyPtr;
typedef xtl::com_ptr<IScene>     ScenePtr;
typedef xtl::com_ptr<IShape>     ShapePtr;

void convert_float (float& value)
{
  value = ceil ((int)(value * 100)) / 100.f;
}

void dump_body_position (IRigidBody* body)
{
  const Transform& body_transform = body->WorldTransform ();

  printf ("Body world transform:\n");

  math::vec3f world_position = body_transform.position;

  for (size_t i = 0; i < 3; i++)
    convert_float (world_position [i]);

  printf ("  position = %.2f; %.2f; %.2f\n", world_position.x, world_position.y, world_position.z);

  math::quatf world_orientation = body_transform.orientation;

  for (size_t i = 0; i < 4; i++)
    convert_float (world_orientation [i]);

  printf ("  orientation = %.2f; %.2f; %.2f; %.2f\n", world_orientation.x, world_orientation.y, world_orientation.z, world_orientation.w);
}

#endif
