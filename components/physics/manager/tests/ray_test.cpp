#include "shared.h"

void ray_test_callback (RigidBody& body, const math::vec3f& position, const math::vec3f& normal)
{
  const math::vec3f& body_position = body.WorldTransform ().position;

  printf ("ray collided with body at %.2f %.2f %.2f, point %.2f %.2f %.2f, normal %.2f %.2f %.2f\n", body_position.x, body_position.y, body_position.z,
          position.x, position.y, position.z, normal.x, normal.y, normal.z);
}

int main ()
{
  printf ("Results of ray_test test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    Scene scene (manager.CreateScene ());

    Shape sphere_shape (manager.CreateSphereShape (1.f));

    RigidBody body1 (scene.CreateRigidBody (sphere_shape, 1)),
              body2 (scene.CreateRigidBody (sphere_shape, 1)),
              body3 (scene.CreateRigidBody (sphere_shape, 1)),
              body4 (scene.CreateRigidBody (sphere_shape, 1));

    body1.SetCollisionGroup ("group1");
    body2.SetCollisionGroup ("group2");
    body3.SetCollisionGroup ("third_group");

    Transform body_transform;

    body_transform.position.x = 0;
    body_transform.position.y = 2;

    body2.SetWorldTransform (body_transform);

    body_transform.position.y = 4;

    body3.SetWorldTransform (body_transform);

    body_transform.position.x = 2;
    body_transform.position.y = 0;

    body4.SetWorldTransform (body_transform);

    printf ("Test very short ray\n");
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, -1.5f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test short ray\n");
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 0.f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test long ray\n");
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test side ray\n");
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (1.f, 10.f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test side ray 2\n");
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (2.f, 0.f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test long ray with empty filter\n");
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), 0, 0, RayTestMode_All, ray_test_callback);

    printf ("Test long ray with one group filter\n");
    const char* filter_group = "group1";
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), 1, &filter_group, RayTestMode_All, ray_test_callback);

    printf ("Test long ray with two groups filter\n");
    const char* filter_groups [] = { "group2", "third_group" };
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), sizeof (filter_groups) / sizeof (*filter_groups), filter_groups, RayTestMode_All, ray_test_callback);

    printf ("Test long ray with wildcard groups filter\n");
    const char* filter_groups2 [] = { "gr*", "third_group_*" };
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), sizeof (filter_groups2) / sizeof (*filter_groups2), filter_groups2, RayTestMode_All, ray_test_callback);

    printf ("Test closest long ray\n");
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), RayTestMode_Closest, ray_test_callback);

    printf ("Test farthest long ray\n");
    scene.RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), RayTestMode_Farthest, ray_test_callback);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
