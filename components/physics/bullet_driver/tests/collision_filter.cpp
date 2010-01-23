#include "shared.h"

const size_t BODIES_COUNT = 5;

bool false_filter (IRigidBody* body1, IRigidBody* body2)
{
  printf ("false filter called for bodies from group %u and %u\n", body1->CollisionGroup (), body2->CollisionGroup ());

  return false;
}

bool true_filter (IRigidBody* body1, IRigidBody* body2)
{
  printf ("true filter called for bodies from group %u and %u\n", body1->CollisionGroup (), body2->CollisionGroup ());

  return true;
}

int main ()
{
  printf ("Results of collision_filter_test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    ScenePtr scene (bullet_driver->CreateScene (), false);

    scene->SetCollisionFilter (0, 0, true,  &false_filter);
    scene->SetCollisionFilter (0, 1, true,  &false_filter);
    scene->SetCollisionFilter (0, 2, false, &false_filter);
    scene->SetCollisionFilter (0, 3, true,  &true_filter);
    scene->SetCollisionFilter (0, 4, false, &true_filter);

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr falling_bodies [BODIES_COUNT] = { RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                                                   RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                                                   RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                                                   RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                                                   RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false) };

    Transform body_transform;

    body_transform.position.y = 5;

    for (size_t i = 0; i < BODIES_COUNT; i++)
    {
      body_transform.position.x = -30.f + 3.f * i;

      falling_bodies [i]->SetWorldTransform (body_transform);
      falling_bodies [i]->SetCollisionGroup (i);
    }

    ShapePtr     plane_shape  (bullet_driver->CreateBoxShape (math::vec3f (50, 1, 50)));
    RigidBodyPtr ground_plane (scene->CreateRigidBody (plane_shape.get (), 0), false);

    body_transform.position.x = 0;
    body_transform.position.y = 0;

    ground_plane->SetWorldTransform (body_transform);

    scene->PerformSimulation (1.f);

    printf ("Simulating one seconds\n");

    printf ("ground plane state:\n");
    dump_body_position (ground_plane.get ());

    for (size_t i = 0; i < BODIES_COUNT; i++)
    {
      printf ("falling body %u state:\n", i);
      dump_body_position (falling_bodies [i].get ());
    }

    scene->SetCollisionFilter (0, 1, true);
    scene->SetCollisionFilter (0, 2, true);
    scene->SetCollisionFilter (0, 3, false);
    scene->SetCollisionFilter (0, 4, false);

    body_transform.position.y = 5;

    for (size_t i = 0; i < BODIES_COUNT; i++)
    {
      body_transform.position.x = -30.f + 3.f * i;

      falling_bodies [i]->SetLinearVelocity (0);
      falling_bodies [i]->SetWorldTransform (body_transform);
    }

    scene->PerformSimulation (1.f);

    printf ("Simulating one seconds after removing filters\n");

    printf ("ground plane state:\n");
    dump_body_position (ground_plane.get ());

    for (size_t i = 0; i < BODIES_COUNT; i++)
    {
      printf ("falling body %u state:\n", i);
      dump_body_position (falling_bodies [i].get ());
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
