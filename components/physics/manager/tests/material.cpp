#include "shared.h"

int main ()
{
  printf ("Results of material_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    Scene scene (manager.CreateScene ());

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    RigidBody body1 (scene.CreateRigidBody (sphere_shape, 1)),
              body2 (scene.CreateRigidBody (sphere_shape, 1));

    body1.SetMassSpaceInertiaTensor (0);
    body2.SetMassSpaceInertiaTensor (0);

    Transform body_transform;

    body_transform.position.x = -10;

    body1.SetWorldTransform (body_transform);

    body_transform.position.x = 10;

    body2.SetWorldTransform (body_transform);

    body1.Material ().SetLinearDamping (0.5f);
    body2.Material ().SetRestitution (0.5f);

    math::vec3f plane_normal (0, 1, 0);
    Shape       plane_shape  (manager.CreatePlaneShape (plane_normal, 0));
    RigidBody   ground_plane (scene.CreateRigidBody (plane_shape, 0));

    ground_plane.Material ().SetRestitution (0.5f);

    body_transform.position.x = 0;
    body_transform.position.y = -10;

    ground_plane.SetWorldTransform (body_transform);

    scene.PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("body1 state:\n");
    dump_body_position (body1);
    printf ("body2 state:\n");
    dump_body_position (body2);

    scene.PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("body1 state:\n");
    dump_body_position (body1);
    printf ("body2 state:\n");
    dump_body_position (body2);

    scene.PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("body1 state:\n");
    dump_body_position (body1);
    printf ("body2 state:\n");
    dump_body_position (body2);

    math::vec3f force (0, 0, 5);

    body1.Material ().SetLinearDamping (0.f);
    body1.Material ().SetFriction (0.5f);
    body2.Material ().SetRestitution (0.f);
    body2.Material ().SetFriction (1.f);

    body1.AddForce (force);
    body2.AddForce (force);

    scene.PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("body1 state:\n");
    dump_body_position (body1);
    printf ("body2 state:\n");
    dump_body_position (body2);

    body1.Material ().SetFriction (0.f);
    body1.Material ().SetAngularDamping (0.5f);
    body2.Material ().SetFriction (0.f);
    body2.Material ().SetAngularDamping (0.1f);

    body1.SetMassSpaceInertiaTensor (1);
    body2.SetMassSpaceInertiaTensor (1);

    body1.AddTorque (force);
    body2.AddTorque (force);

    scene.PerformSimulation (1.f);

    printf ("Simulating one second\n");

    printf ("body1 state:\n");
    dump_body_position (body1);
    printf ("body2 state:\n");
    dump_body_position (body2);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
