#include "shared.h"

const char* LIBRARY_PATH = "data/test.xphys";

int main ()
{
  printf ("Results of manager_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    manager.LoadLibrary (LIBRARY_PATH);

    printf ("Creating objects from library\n");

    Material default_material = manager.CreateMaterial ("Default");
    Shape    plane_shape      = manager.CreateShape ("PlaneShape");

    Scene scene (manager.CreateScene ());

    RigidBody body1 (scene.CreateRigidBody ("BigShip")),
              body2 (scene.CreateRigidBody ("CompoundBody")),
              body3 (scene.CreateRigidBody ("TriangleMeshBody")),
              body4 (scene.CreateRigidBody ("BigShip2")),
              body5 (scene.CreateRigidBody ("KinematicBody"));

    printf ("Unloading library\n");

    manager.UnloadLibrary (LIBRARY_PATH);

    default_material = manager.CreateMaterial ("Default");
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
