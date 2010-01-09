#include "shared.h"

int main ()
{
  printf ("Results of body_flags_test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    ScenePtr scene (bullet_driver->CreateScene (), false);

    scene->SetGravity (math::vec3f (-10, -10, -10));

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr position_x_frozen_body (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 position_y_frozen_body (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 position_z_frozen_body (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 position_frozen_body   (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 rotation_x_frozen_body (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 rotation_y_frozen_body (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 rotation_z_frozen_body (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 rotation_frozen_body   (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 kinematic_body         (scene->CreateRigidBody (sphere_shape.get (), 1), false);

    position_x_frozen_body->SetMassSpaceInertiaTensor (1);
    position_y_frozen_body->SetMassSpaceInertiaTensor (1);
    position_z_frozen_body->SetMassSpaceInertiaTensor (1);
    position_frozen_body->SetMassSpaceInertiaTensor (1);
    rotation_x_frozen_body->SetMassSpaceInertiaTensor (1);
    rotation_y_frozen_body->SetMassSpaceInertiaTensor (1);
    rotation_z_frozen_body->SetMassSpaceInertiaTensor (1);
    rotation_frozen_body->SetMassSpaceInertiaTensor (1);
    kinematic_body->SetMassSpaceInertiaTensor (1);

    position_x_frozen_body->SetFlags (RigidBodyFlag_FrozenPositionX);
    position_y_frozen_body->SetFlags (RigidBodyFlag_FrozenPositionY);
    position_z_frozen_body->SetFlags (RigidBodyFlag_FrozenPositionZ);
    position_frozen_body->SetFlags (RigidBodyFlag_FrozenPosition);
    rotation_x_frozen_body->SetFlags (RigidBodyFlag_FrozenRotationX);
    rotation_y_frozen_body->SetFlags (RigidBodyFlag_FrozenRotationY);
    rotation_z_frozen_body->SetFlags (RigidBodyFlag_FrozenRotationZ);
    rotation_frozen_body->SetFlags (RigidBodyFlag_FrozenRotation);
    kinematic_body->SetFlags (RigidBodyFlag_Kinematic);

    scene->PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("position_x_frozen_body state:\n");
    dump_body_position (position_x_frozen_body.get ());
    printf ("position_y_frozen_body state:\n");
    dump_body_position (position_y_frozen_body.get ());
    printf ("position_z_frozen_body state:\n");
    dump_body_position (position_z_frozen_body.get ());
    printf ("position_frozen_body state:\n");
    dump_body_position (position_frozen_body.get ());
    printf ("rotation_x_frozen_body state:\n");
    dump_body_position (rotation_x_frozen_body.get ());
    printf ("rotation_y_frozen_body state:\n");
    dump_body_position (rotation_y_frozen_body.get ());
    printf ("rotation_z_frozen_body state:\n");
    dump_body_position (rotation_z_frozen_body.get ());
    printf ("rotation_frozen_body state:\n");
    dump_body_position (rotation_frozen_body.get ());
    printf ("kinematic_body state:\n");
    dump_body_position (kinematic_body.get ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
