#include "shared.h"

bool default_filter (IRigidBody* body1, IRigidBody* body2)
{
  printf ("default filter called for bodies from group %u and %u\n", body1->CollisionGroup (), body2->CollisionGroup ());

  return true;
}

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

void collision_event_handler (const CollisionEvent& event)
{
  printf ("new collision event detected: \n  type is ");

  switch (event.type)
  {
    case CollisionEventType_Begin:
      printf ("ColisionEventType_Begin\n");
      break;
    case CollisionEventType_Process:
      printf ("ColisionEventType_Process\n");
      break;
    case CollisionEventType_End:
      printf ("ColisionEventType_End\n");
      break;
    default:
      printf ("Unknown\n");
  }

  printf ("  position is %.2f %.2f %.2f\n", event.point.x, event.point.y, event.point.z);
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

    scene->SetDefaultCollisionFilter (&default_filter);

    xtl::connection collision_begin_connection   = scene->RegisterCollisionCallback (CollisionEventType_Begin, &collision_event_handler),
                    collision_process_connection = scene->RegisterCollisionCallback (CollisionEventType_Process, &collision_event_handler),
                    collision_end_connection     = scene->RegisterCollisionCallback (CollisionEventType_End, &collision_event_handler);

    scene->SetCollisionFilter (0, 0, true,  &false_filter);
    scene->SetCollisionFilter (0, 1, true,  &false_filter);
    scene->SetCollisionFilter (0, 2, false, &false_filter);
    scene->SetCollisionFilter (0, 3, true,  &true_filter);
    scene->SetCollisionFilter (0, 4, false, &true_filter);

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr falling_bodies [] = { RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                                       RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                                       RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                                       RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                                       RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                                       RigidBodyPtr (scene->CreateRigidBody (sphere_shape.get (), 1), false) };

    size_t bodies_count = sizeof (falling_bodies) / sizeof (falling_bodies [0]);

    Transform body_transform;

    body_transform.position.y = 5;

    for (size_t i = 0; i < bodies_count; i++)
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

    collision_begin_connection.disconnect ();
    collision_process_connection.disconnect ();

    printf ("Simulating one seconds\n");

    printf ("ground plane state:\n");
    dump_body_position (ground_plane.get ());

    for (size_t i = 0; i < bodies_count; i++)
    {
      printf ("falling body %u state:\n", i);
      dump_body_position (falling_bodies [i].get ());
    }

    scene->SetCollisionFilter (0, 1, true);
    scene->SetCollisionFilter (0, 2, true);
    scene->SetCollisionFilter (0, 3, false);
    scene->SetCollisionFilter (0, 4, false);

    body_transform.position.y = 5;

    for (size_t i = 0; i < bodies_count; i++)
    {
      body_transform.position.x = -30.f + 3.f * i;

      falling_bodies [i]->SetLinearVelocity (0);
      falling_bodies [i]->SetWorldTransform (body_transform);
    }

    scene->PerformSimulation (1.f);

    printf ("Simulating one seconds after removing filters\n");

    printf ("ground plane state:\n");
    dump_body_position (ground_plane.get ());

    for (size_t i = 0; i < bodies_count; i++)
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
