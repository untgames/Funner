#include "shared.h"

void on_body_update (const char* body_name, IRigidBody* body)
{
  printf ("%s body updated\n", body_name);
  dump_body_position (body);
}

int main ()
{
  printf ("Results of body_update_test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    ScenePtr scene (bullet_driver->CreateScene (), false);

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr falling_body (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 static_body  (scene->CreateRigidBody (sphere_shape.get (), 0), false);

    falling_body->RegisterTransformUpdateCallback (xtl::bind (&on_body_update, "falling_body", _1));
    static_body->RegisterTransformUpdateCallback (xtl::bind (&on_body_update, "static_body", _1));

    Transform body_transform;

    body_transform.position.x = -10;

    static_body->SetWorldTransform (body_transform);

    printf ("Initial falling body state:\n");
    dump_body_position (falling_body.get ());
    printf ("Initial static body state:\n");
    dump_body_position (static_body.get ());

    scene->PerformSimulation (1.f);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
