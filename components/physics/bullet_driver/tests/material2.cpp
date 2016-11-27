#include "shared.h"

int main ()
{
  printf ("Results of material_test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    ScenePtr scene (bullet_driver->CreateScene (), false);

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr body1 (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 body2 (scene->CreateRigidBody (sphere_shape.get (), 1), false);

    printf ("body 1 material friction = %.2f, body 2 material friction = %.2f\n", body1->Material ()->Friction (), body2->Material ()->Friction ());

    body2->Material ()->SetFriction (0.3f);

    printf ("body 1 material friction = %.2f, body 2 material friction = %.2f\n", body1->Material ()->Friction (), body2->Material ()->Friction ());

    MaterialPtr material (bullet_driver->CreateMaterial (), false);

    body1->SetMaterial (material.get ());
    body2->SetMaterial (material.get ());

    printf ("body 1 material friction = %.2f, body 2 material friction = %.2f\n", body1->Material ()->Friction (), body2->Material ()->Friction ());

    material->SetFriction (0.25f);

    printf ("body 1 material friction = %.2f, body 2 material friction = %.2f\n", body1->Material ()->Friction (), body2->Material ()->Friction ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
