#include "shared.h"

int main ()
{
  printf ("Results of fall_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    printf ("Manager description = '%s'\n", manager.Description ());

    Scene* scene (new Scene (manager.CreateScene ()));

    const math::vec3f& default_gravity = scene->Gravity ();

    printf ("Default scene gravity = %.2f; %.2f; %.2f \n", default_gravity.x, default_gravity.y, default_gravity.z);
    printf ("Default scene simulation step = %.3f\n", scene->SimulationStep ());

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    RigidBody static_body        (scene->CreateRigidBody (sphere_shape, 0)),
              light_falling_body (scene->CreateRigidBody (sphere_shape, 1)),
              heavy_falling_body (scene->CreateRigidBody (sphere_shape, 5));

    Transform body_transform;

    body_transform.position.x = -10;

    light_falling_body.SetWorldTransform (body_transform);

    body_transform.position.x = 10;

    heavy_falling_body.SetWorldTransform (body_transform);

    printf ("Initial static body state:\n");
    printf ("  mass = %.2f\n", static_body.Mass ());
    printf ("  linear sleeping threshold = %.2f\n", static_body.SleepLinearVelocity ());
    printf ("  angular sleeping threshold = %.2f\n", static_body.SleepAngularVelocity ());
    printf ("  ccd motion threshold = %.2f\n", static_body.CcdMotionThreshold ());
    printf ("  collision group = '%s'\n", static_body.CollisionGroup ());
    dump_body_position (static_body);
    printf ("Initial light falling body state:\n");
    printf ("  mass = %.2f\n", light_falling_body.Mass ());
    printf ("  linear sleeping threshold = %.2f\n", light_falling_body.SleepLinearVelocity ());
    printf ("  angular sleeping threshold = %.2f\n", light_falling_body.SleepAngularVelocity ());
    printf ("  ccd motion threshold = %.2f\n", light_falling_body.CcdMotionThreshold ());
    dump_body_position (light_falling_body);
    printf ("Initial heavy falling body state:\n");
    printf ("  mass = %.2f\n", heavy_falling_body.Mass ());
    dump_body_position (heavy_falling_body);

    scene->PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("static body state:\n");
    printf ("  mass = %.2f\n", static_body.Mass ());
    dump_body_position (static_body);
    printf ("light falling body state:\n");
    printf ("  mass = %.2f\n", light_falling_body.Mass ());
    dump_body_position (light_falling_body);
    printf ("heavy falling body state:\n");
    printf ("  mass = %.2f\n", heavy_falling_body.Mass ());
    dump_body_position (heavy_falling_body);

    math::vec3f plane_normal (0, 1, 0);
    Shape       plane_shape  (manager.CreatePlaneShape (plane_normal, 0));

    {
      RigidBody ground_plane (scene->CreateRigidBody (plane_shape, 0));

      body_transform.position.x = 0;
      body_transform.position.y = -15;

      ground_plane.SetWorldTransform (body_transform);

      printf ("ground plane state:\n");
      printf ("  mass = %.2f\n", ground_plane.Mass ());
      dump_body_position (ground_plane);

      scene->PerformSimulation (1.f);

      printf ("Simulating one second\n");

      printf ("ground plane state:\n");
      dump_body_position (ground_plane);
      printf ("light falling body state:\n");
      dump_body_position (light_falling_body);
      printf ("heavy falling body state:\n");
      dump_body_position (heavy_falling_body);
    }

    scene->PerformSimulation (1.f);

    printf ("Simulating one second after deleting ground\n");

    printf ("light falling body state:\n");
    dump_body_position (light_falling_body);
    printf ("heavy falling body state:\n");
    dump_body_position (heavy_falling_body);

    scene = 0;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
