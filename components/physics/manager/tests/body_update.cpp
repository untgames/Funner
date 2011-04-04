#include "shared.h"

void on_body_update (const char* body_name, const RigidBody& body)
{
  printf ("%s body updated\n", body_name);
  dump_body_position (body);
}

int main ()
{
  printf ("Results of body_update_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    Scene scene (manager.CreateScene ());

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    RigidBody falling_body (scene.CreateRigidBody (sphere_shape, 1)),
              static_body  (scene.CreateRigidBody (sphere_shape, 0));

    falling_body.RegisterTransformUpdateCallback (xtl::bind (&on_body_update, "falling_body", _1));
    static_body.RegisterTransformUpdateCallback (xtl::bind (&on_body_update, "static_body", _1));

    Transform body_transform;

    body_transform.position.x = -10;

    static_body.SetWorldTransform (body_transform);

    printf ("Initial falling body state:\n");
    dump_body_position (falling_body);
    printf ("Initial static body state:\n");
    dump_body_position (static_body);

    scene.PerformSimulation (0.79f);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
