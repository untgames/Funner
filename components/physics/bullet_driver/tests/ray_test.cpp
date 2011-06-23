#include "shared.h"

void ray_test_callback (IRigidBody* body, const math::vec3f& position, const math::vec3f& normal)
{
  const math::vec3f& body_position = body->WorldTransform ().position;

  printf ("ray collided with body at %.2f %.2f %.2f, point %.2f %.2f %.2f, normal %.2f %.2f %.2f\n", body_position.x, body_position.y, body_position.z,
          position.x, position.y, position.z, normal.x, normal.y, normal.z);
}

int main ()
{
  printf ("Results of ray_test test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    ScenePtr scene (bullet_driver->CreateScene (), false);

    ShapePtr sphere_shape (bullet_driver->CreateSphereShape (1.f), false);

    RigidBodyPtr body1 (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 body2 (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 body3 (scene->CreateRigidBody (sphere_shape.get (), 1), false),
                 body4 (scene->CreateRigidBody (sphere_shape.get (), 1), false);

    body1->SetCollisionGroup (1);
    body2->SetCollisionGroup (2);
    body3->SetCollisionGroup (3);

    Transform body_transform;

    body_transform.position.x = 0;
    body_transform.position.y = 2;

    body2->SetWorldTransform (body_transform);

    body_transform.position.y = 4;

    body3->SetWorldTransform (body_transform);

    body_transform.position.x = 2;
    body_transform.position.y = 0;

    body4->SetWorldTransform (body_transform);

    printf ("Test very short ray\n");
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, -1.5f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test short ray\n");
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 0.f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test long ray\n");
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test side ray\n");
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (1.f, 10.f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test side ray 2\n");
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (2.f, 0.f, 0.f), RayTestMode_All, ray_test_callback);

    printf ("Test long ray with empty filter\n");
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), 0, 0, RayTestMode_All, ray_test_callback);

    printf ("Test long ray with one group filter\n");
    size_t filter_group = 2;
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), 1, &filter_group, RayTestMode_All, ray_test_callback);

    printf ("Test long ray with two groups filter\n");
    size_t filter_groups [] = { 2, 3 };
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), sizeof (filter_groups) / sizeof (*filter_groups), filter_groups, RayTestMode_All, ray_test_callback);

    printf ("Test closest long ray\n");
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), RayTestMode_Nearest, ray_test_callback);

    printf ("Test farthest long ray\n");
    scene->RayTest (math::vec3f (0.f, -2.f, 0.f), math::vec3f (0.f, 10.f, 0.f), RayTestMode_Farthest, ray_test_callback);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
