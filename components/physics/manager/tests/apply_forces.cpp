#include "shared.h"

int main ()
{
  printf ("Results of apply_forces_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    Scene scene (manager.CreateScene ());

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    RigidBody falling_body (scene.CreateRigidBody (sphere_shape, 1));

    math::vec3f plane_normal (0, 1, 0);
    Shape       plane_shape  (manager.CreatePlaneShape (plane_normal, 0));
    RigidBody   ground_plane (scene.CreateRigidBody (plane_shape, 0));

    Transform body_transform;

    body_transform.position.x = 0;
    body_transform.position.y = -2;

    ground_plane.SetWorldTransform (body_transform);

    printf ("ground plane state:\n");
    dump_body_position (ground_plane);
    printf ("falling body state:\n");
    dump_body_position (falling_body);

    scene.PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("ground plane state:\n");
    dump_body_position (ground_plane);
    printf ("falling body state:\n");
    dump_body_position (falling_body);

    math::vec3f force (0, 5, 0);

    falling_body.AddForce (force);

    scene.PerformSimulation (1.f);
    printf ("Simulating one second after apply light force\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body);

    force.y = 15;

    falling_body.AddForce (force);

    scene.PerformSimulation (1.f);
    printf ("Simulating one second after apply big force\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body);

    scene.PerformSimulation (2.f);
    printf ("Simulating two seconds\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body);

    force.y = 5;

    falling_body.AddImpulse (force);

    scene.PerformSimulation (1.f);
    printf ("Simulating one second after apply light impulse\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body);

    scene.PerformSimulation (1.f);
    printf ("Simulating one second\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body);

    falling_body.AddTorque (force);

    scene.PerformSimulation (1.f);
    printf ("Simulating one second after apply light torque\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body);

    falling_body.AddTorqueImpulse (force);

    scene.PerformSimulation (1.f);
    printf ("Simulating one second after apply light torque impulse\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
