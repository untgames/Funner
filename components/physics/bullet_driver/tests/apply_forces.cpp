#include "shared.h"

int main ()
{
  printf ("Results of apply_forces_test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    ScenePtr scene (bullet_driver->CreateScene (), false);

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr falling_body (scene->CreateRigidBody (sphere_shape.get (), 1), false);

    math::vec3f  plane_normal (0, 1, 0);
    ShapePtr     plane_shape  (bullet_driver->CreatePlaneShape (plane_normal, 0));
    RigidBodyPtr ground_plane (scene->CreateRigidBody (plane_shape.get (), 0), false);

    Transform body_transform;

    body_transform.position.x = 0;
    body_transform.position.y = -2;

    ground_plane->SetWorldTransform (body_transform);

    printf ("ground plane state:\n");
    dump_body_position (ground_plane.get ());
    printf ("falling body state:\n");
    dump_body_position (falling_body.get ());

    scene->PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("ground plane state:\n");
    dump_body_position (ground_plane.get ());
    printf ("falling body state:\n");
    dump_body_position (falling_body.get ());

    math::vec3f force (0, 5, 0);

    falling_body->AddForce (force);

    scene->PerformSimulation (1.f);
    printf ("Simulating one second after apply light force\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body.get ());

    force.y = 15;

    falling_body->AddForce (force);

    scene->PerformSimulation (1.f);
    printf ("Simulating one second after apply big force\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body.get ());

    scene->PerformSimulation (2.f);
    printf ("Simulating two seconds\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body.get ());

    force.y = 5;

    falling_body->AddImpulse (force);

    scene->PerformSimulation (1.f);
    printf ("Simulating one second after apply light impulse\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body.get ());

    scene->PerformSimulation (1.f);
    printf ("Simulating one second\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body.get ());

    falling_body->AddTorque (force);

    scene->PerformSimulation (1.f);
    printf ("Simulating one second after apply light torque\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body.get ());

    falling_body->AddTorqueImpulse (force);

    scene->PerformSimulation (1.f);
    printf ("Simulating one second after apply light torque impulse\n");
    printf ("falling body state:\n");
    dump_body_position (falling_body.get ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
