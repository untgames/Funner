#include "shared.h"

int main ()
{
  printf ("Results of material2_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    Scene scene (manager.CreateScene ());

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    RigidBody body1 (scene.CreateRigidBody (sphere_shape, 1)),
              body2 (scene.CreateRigidBody (sphere_shape, 1));

    printf ("body 1 material friction = %.2f, body 2 material friction = %.2f\n", body1.Material ().Friction (), body2.Material ().Friction ());

    body2.Material ().SetFriction (0.3f);

    printf ("body 1 material friction = %.2f, body 2 material friction = %.2f\n", body1.Material ().Friction (), body2.Material ().Friction ());

    Material material (manager.CreateMaterial ());

    body1.SetMaterial (material);
    body2.SetMaterial (material);

    printf ("body 1 material friction = %.2f, body 2 material friction = %.2f\n", body1.Material ().Friction (), body2.Material ().Friction ());

    material.SetFriction (0.25f);

    printf ("body 1 material friction = %.2f, body 2 material friction = %.2f\n", body1.Material ().Friction (), body2.Material ().Friction ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
