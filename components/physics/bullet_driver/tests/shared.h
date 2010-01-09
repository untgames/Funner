#ifndef PHYSICS_BULLET_DRIVER_TESTS_SHARED_HEADER
#define PHYSICS_BULLET_DRIVER_TESTS_SHARED_HEADER

#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/connection.h>
#include <xtl/function.h>
#include <xtl/intrusive_ptr.h>
#include <xtl/reference_counter.h>

#include <physics/low_level/debug_renderable.h>
#include <physics/low_level/driver.h>
#include <physics/low_level/joints.h>
#include <physics/low_level/material.h>
#include <physics/low_level/rigid_body.h>
#include <physics/low_level/scene.h>
#include <physics/low_level/shape.h>

using namespace physics::low_level;

const char* DRIVER_NAME = "Bullet";

typedef xtl::com_ptr<IRigidBody> RigidBodyPtr;
typedef xtl::com_ptr<IScene>     ScenePtr;
typedef xtl::com_ptr<IShape>     ShapePtr;

void dump_body_position (IRigidBody* body)
{
  const Transform& body_transform = body->WorldTransform ();

  printf ("Body world transform:\n");

  const math::vec3f& world_position = body_transform.position;

  printf ("  position = %.2f; %.2f; %.2f\n", world_position.x, world_position.y, world_position.z);

  const math::quatf& world_orientation = body_transform.orientation;

  printf ("  orientation = %.2f; %.2f; %.2f; %.2f\n", world_orientation.x, world_orientation.y, world_orientation.z, world_orientation.w);
}

#endif
