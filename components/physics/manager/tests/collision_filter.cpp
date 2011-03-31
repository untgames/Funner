#include "shared.h"

bool false_filter (const RigidBody& body1, const RigidBody& body2)
{
  printf ("false filter called for bodies from group %s and %s\n", body1.CollisionGroup (), body2.CollisionGroup ());

  return false;
}

bool true_filter (const RigidBody& body1, const RigidBody& body2)
{
  printf ("true filter called for bodies from group %s and %s\n", body1.CollisionGroup (), body2.CollisionGroup ());

  return true;
}

void collision_event_handler (CollisionEventType event, RigidBody& first_body, RigidBody& second_body, const math::vec3f& point)
{
  printf ("new collision event detected: \n  type is ");

  switch (event)
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

  printf ("  position is %.2f %.2f %.2f\n", point.x, point.y, point.z);
}

int main ()
{
  printf ("Results of collision_filter_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    Scene scene (manager.CreateScene ());

    xtl::connection collision_begin_connection   = scene.RegisterCollisionCallback ("*", "*", CollisionEventType_Begin, &collision_event_handler),
                    collision_process_connection = scene.RegisterCollisionCallback ("*", "*", CollisionEventType_Process, &collision_event_handler),
                    collision_end_connection     = scene.RegisterCollisionCallback ("*", "*", CollisionEventType_End, &collision_event_handler);

    scene.AddCollisionFilter ("0", "0", true, &false_filter);

    size_t collision_filter_1 = scene.AddCollisionFilter ("0", "1", true,  &false_filter),
           collision_filter_2 = scene.AddCollisionFilter ("0", "2", false, &false_filter),
           collision_filter_3 = scene.AddCollisionFilter ("0", "3", true,  &true_filter),
           collision_filter_4 = scene.AddCollisionFilter ("0", "4", false, &true_filter);

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    RigidBody falling_bodies [] = { RigidBody (scene.CreateRigidBody (sphere_shape, 1)),
                                    RigidBody (scene.CreateRigidBody (sphere_shape, 1)),
                                    RigidBody (scene.CreateRigidBody (sphere_shape, 1)),
                                    RigidBody (scene.CreateRigidBody (sphere_shape, 1)),
                                    RigidBody (scene.CreateRigidBody (sphere_shape, 1)),
                                    RigidBody (scene.CreateRigidBody (sphere_shape, 1)) };

    size_t bodies_count = sizeof (falling_bodies) / sizeof (falling_bodies [0]);

    Transform body_transform;

    body_transform.position.y = 5;

    for (size_t i = 0; i < bodies_count; i++)
    {
      body_transform.position.x = -30.f + 3.f * i;

      falling_bodies [i].SetWorldTransform (body_transform);

      char buffer [2];

      xtl::xsnprintf (buffer, sizeof (buffer), "%u", i);

      falling_bodies [i].SetCollisionGroup (buffer);
    }

    Shape     plane_shape  (manager.CreateBoxShape (math::vec3f (50, 1, 50)));
    RigidBody ground_plane (scene.CreateRigidBody (plane_shape, 0));

    ground_plane.SetCollisionGroup ("0");

    body_transform.position.x = 0;
    body_transform.position.y = 0;

    ground_plane.SetWorldTransform (body_transform);

    scene.PerformSimulation (1.f);

    collision_begin_connection.disconnect ();
    collision_process_connection.disconnect ();

    printf ("Simulating one seconds\n");

    printf ("ground plane state:\n");
    dump_body_position (ground_plane);

    for (size_t i = 0; i < bodies_count; i++)
    {
      printf ("falling body %u state:\n", i);
      dump_body_position (falling_bodies [i]);
    }

    scene.RemoveCollisionFilter (collision_filter_1);
    scene.RemoveCollisionFilter (collision_filter_2);
    scene.RemoveCollisionFilter (collision_filter_3);
    scene.RemoveCollisionFilter (collision_filter_4);

    scene.AddCollisionFilter ("0", "3", false);
    scene.AddCollisionFilter ("0", "4", false);

    body_transform.position.y = 5;

    for (size_t i = 0; i < bodies_count; i++)
    {
      body_transform.position.x = -30.f + 3.f * i;

      falling_bodies [i].SetLinearVelocity (0);
      falling_bodies [i].SetWorldTransform (body_transform);
    }

    scene.PerformSimulation (1.f);

    printf ("Simulating one seconds after removing filters\n");

    printf ("ground plane state:\n");
    dump_body_position (ground_plane);

    for (size_t i = 0; i < bodies_count; i++)
    {
      printf ("falling body %u state:\n", i);
      dump_body_position (falling_bodies [i]);
    }
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
