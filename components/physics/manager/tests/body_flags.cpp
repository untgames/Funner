#include "shared.h"

int main ()
{
  printf ("Results of body_flags_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    Scene scene (manager.CreateScene ());

    scene.SetGravity (math::vec3f (-10, -10, -10));

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    RigidBody position_x_frozen_body (scene.CreateRigidBody (sphere_shape, 1)),
              position_y_frozen_body (scene.CreateRigidBody (sphere_shape, 1)),
              position_z_frozen_body (scene.CreateRigidBody (sphere_shape, 1)),
              position_frozen_body   (scene.CreateRigidBody (sphere_shape, 1)),
              rotation_x_frozen_body (scene.CreateRigidBody (sphere_shape, 1)),
              rotation_y_frozen_body (scene.CreateRigidBody (sphere_shape, 1)),
              rotation_z_frozen_body (scene.CreateRigidBody (sphere_shape, 1)),
              rotation_frozen_body   (scene.CreateRigidBody (sphere_shape, 1)),
              kinematic_body         (scene.CreateRigidBody (sphere_shape, 1));

    position_x_frozen_body.SetFlags (RigidBodyFlag_FrozenPositionX);
    position_y_frozen_body.SetFlags (RigidBodyFlag_FrozenPositionY);
    position_z_frozen_body.SetFlags (RigidBodyFlag_FrozenPositionZ);
    position_frozen_body.SetFlags (RigidBodyFlag_FrozenPosition);
    rotation_x_frozen_body.SetFlags (RigidBodyFlag_FrozenRotationX);
    rotation_y_frozen_body.SetFlags (RigidBodyFlag_FrozenRotationY);
    rotation_z_frozen_body.SetFlags (RigidBodyFlag_FrozenRotationZ);
    rotation_frozen_body.SetFlags (RigidBodyFlag_FrozenRotation);
    kinematic_body.SetFlags (RigidBodyFlag_Kinematic);

    scene.PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("position_x_frozen_body state:\n");
    dump_body_position (position_x_frozen_body);
    printf ("position_y_frozen_body state:\n");
    dump_body_position (position_y_frozen_body);
    printf ("position_z_frozen_body state:\n");
    dump_body_position (position_z_frozen_body);
    printf ("position_frozen_body state:\n");
    dump_body_position (position_frozen_body);
    printf ("rotation_x_frozen_body state:\n");
    dump_body_position (rotation_x_frozen_body);
    printf ("rotation_y_frozen_body state:\n");
    dump_body_position (rotation_y_frozen_body);
    printf ("rotation_z_frozen_body state:\n");
    dump_body_position (rotation_z_frozen_body);
    printf ("rotation_frozen_body state:\n");
    dump_body_position (rotation_frozen_body);
    printf ("kinematic_body state:\n");
    dump_body_position (kinematic_body);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
