#include "shared.h"

int main ()
{
  printf ("Results of fall_test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    printf ("Driver description = '%s'\n", bullet_driver->GetDescription ());

    ScenePtr scene (bullet_driver->CreateScene (), false);

    const math::vec3f& default_gravity = scene->Gravity ();

    printf ("Default scene gravity = %.2f; %.2f; %.2f \n", default_gravity.x, default_gravity.y, default_gravity.z);
    printf ("Default scene simulation step = %.3f\n", scene->SimulationStep ());

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr static_body        (scene->CreateRigidBody (sphere_shape.get (), 0), false),
                 light_falling_body (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 heavy_falling_body (scene->CreateRigidBody (sphere_shape.get (), 5), false);

    Transform body_transform;

    body_transform.position.x = -10;

    light_falling_body->SetWorldTransform (body_transform);

    body_transform.position.x = 10;

    heavy_falling_body->SetWorldTransform (body_transform);

    printf ("Initial static body state:\n");
    printf ("  mass = %.2f\n", static_body->Mass ());
    printf ("  linear sleeping threshold = %.2f\n", static_body->SleepLinearVelocity ());
    printf ("  angular sleeping threshold = %.2f\n", static_body->SleepAngularVelocity ());
    printf ("  ccd motion threshold = %.2f\n", static_body->CcdMotionThreshold ());
    printf ("  collision group = %u\n", static_body->CollisionGroup ());
    dump_body_position (static_body.get ());
    printf ("Initial light falling body state:\n");
    printf ("  mass = %.2f\n", light_falling_body->Mass ());
    printf ("  linear sleeping threshold = %.2f\n", light_falling_body->SleepLinearVelocity ());
    printf ("  angular sleeping threshold = %.2f\n", light_falling_body->SleepAngularVelocity ());
    printf ("  ccd motion threshold = %.2f\n", light_falling_body->CcdMotionThreshold ());
    dump_body_position (light_falling_body.get ());
    printf ("Initial heavy falling body state:\n");
    printf ("  mass = %.2f\n", heavy_falling_body->Mass ());
    dump_body_position (heavy_falling_body.get ());

    scene->PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("static body state:\n");
    printf ("  mass = %.2f\n", static_body->Mass ());
    dump_body_position (static_body.get ());
    printf ("light falling body state:\n");
    printf ("  mass = %.2f\n", light_falling_body->Mass ());
    dump_body_position (light_falling_body.get ());
    printf ("heavy falling body state:\n");
    printf ("  mass = %.2f\n", heavy_falling_body->Mass ());
    dump_body_position (heavy_falling_body.get ());

    math::vec3f  plane_normal (0, 1, 0);
    ShapePtr     plane_shape  (bullet_driver->CreatePlaneShape (plane_normal, 0));
    RigidBodyPtr ground_plane (scene->CreateRigidBody (plane_shape.get (), 0), false);

    body_transform.position.x = 0;
    body_transform.position.y = -15;

    ground_plane->SetWorldTransform (body_transform);

    printf ("ground plane state:\n");
    printf ("  mass = %.2f\n", ground_plane->Mass ());
    dump_body_position (ground_plane.get ());

    scene->PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("ground plane state:\n");
    dump_body_position (ground_plane.get ());
    printf ("light falling body state:\n");
    dump_body_position (light_falling_body.get ());
    printf ("heavy falling body state:\n");
    dump_body_position (heavy_falling_body.get ());

    ground_plane = 0;

    scene->PerformSimulation (1.f);

    printf ("Simulating one second after deleting ground\n");

    printf ("light falling body state:\n");
    dump_body_position (light_falling_body.get ());
    printf ("heavy falling body state:\n");
    dump_body_position (heavy_falling_body.get ());

    scene = 0;
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
