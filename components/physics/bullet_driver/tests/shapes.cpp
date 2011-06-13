#include "shared.h"

const size_t NUM_VERTS_X = 5;
const size_t NUM_VERTS_Y = 5;

int main ()
{
  printf ("Results of shapes_test:\n");
  
  try
  {
    IDriver* bullet_driver = DriverManager::FindDriver (DRIVER_NAME);

    if (!bullet_driver)
      throw xtl::format_operation_exception ("", "Can't find driver '%s'", DRIVER_NAME);

    ScenePtr scene (bullet_driver->CreateScene (), false);

    math::vec3f mesh_vertices []        = { math::vec3f (0, -1.5, 0), math::vec3f (0.5, 0, 0), math::vec3f (0, 0, 1), math::vec3f (-1, 0, 0) };
    size_t      triangles_indices []    = { 0, 1, 2, 0, 3, 1, 0, 2, 3, 3, 2, 1};
    size_t      mesh_vertices_count     = sizeof (mesh_vertices) / sizeof (*mesh_vertices);
    size_t      triangles_indices_count = sizeof (triangles_indices) / sizeof (*triangles_indices);

    ShapePtr box_shape           (bullet_driver->CreateBoxShape          (1.f), false),
             sphere_shape        (bullet_driver->CreateSphereShape       (1.f), false),
             capsule_shape       (bullet_driver->CreateCapsuleShape      (1.f, 0.5f), false),
             plane_shape         (bullet_driver->CreateBoxShape          (math::vec3f (50.f, 0.01f, 50.f)), false),
             convex_shape        (bullet_driver->CreateConvexShape       (mesh_vertices_count, mesh_vertices), false),
             triangle_mesh_shape (bullet_driver->CreateTriangleMeshShape (mesh_vertices_count, mesh_vertices, triangles_indices_count / 3, triangles_indices), false);

    triangle_mesh_shape->SetMargin (1.5f);

    IShape* compounding_shapes [] = { box_shape.get (), sphere_shape.get () };
    size_t  compounding_shapes_count = sizeof (compounding_shapes) / sizeof (*compounding_shapes);

    Transform compounding_shapes_transforms [2];

    compounding_shapes_transforms[0].position.y = -4;

    ShapePtr compound_shape (bullet_driver->CreateCompoundShape (compounding_shapes_count, compounding_shapes, compounding_shapes_transforms));

    RigidBodyPtr box_body           (scene->CreateRigidBody (box_shape.get (), 1),           false),
                 sphere_body        (scene->CreateRigidBody (sphere_shape.get (), 1),        false),
                 capsule_body       (scene->CreateRigidBody (capsule_shape.get (), 1),       false),
                 plane_body         (scene->CreateRigidBody (plane_shape.get (), 0),         false),
                 convex_body        (scene->CreateRigidBody (convex_shape.get (), 1),        false),
                 triangle_mesh_body (scene->CreateRigidBody (triangle_mesh_shape.get (), 1), false),
                 compound_body      (scene->CreateRigidBody (compound_shape.get (), 1),      false);

    Transform body_transform;

    body_transform.position.x = -10;
    box_body->SetWorldTransform (body_transform);

    body_transform.position.x = -7;
    sphere_body->SetWorldTransform (body_transform);

    body_transform.position.x = -4;
    capsule_body->SetWorldTransform (body_transform);
    capsule_body->AddTorque (1.5f);

    body_transform.position.x = -1;
    convex_body->SetWorldTransform (body_transform);
    convex_body->AddImpulse (0.5f);

    body_transform.position.x = 2;
    triangle_mesh_body->SetWorldTransform (body_transform);

    body_transform.position.x = 5;
    compound_body->SetWorldTransform (body_transform);

    body_transform.position.x = 0;
    body_transform.position.y = -15;
    plane_body->SetWorldTransform (body_transform);

    printf ("plane body state:\n");
    dump_body_position (plane_body.get ());
    printf ("box body state:\n");
    dump_body_position (box_body.get ());
    printf ("sphere body state:\n");
    dump_body_position (sphere_body.get ());
    printf ("capsule body state:\n");
    dump_body_position (capsule_body.get ());
    printf ("convex body state:\n");
    dump_body_position (convex_body.get ());
    printf ("triangle mesh body state:\n");
    dump_body_position (triangle_mesh_body.get ());
    printf ("compound body state:\n");
    dump_body_position (compound_body.get ());

    for (size_t i = 0; i < 10; i++)
      scene->PerformSimulation (0.5f);

    printf ("Simulating five seconds\n");

    printf ("plane body state:\n");
    dump_body_position (plane_body.get ());
    printf ("box body state:\n");
    dump_body_position (box_body.get ());
    printf ("sphere body state:\n");
    dump_body_position (sphere_body.get ());
    printf ("capsule body state:\n");
    dump_body_position (capsule_body.get ());
    printf ("convex body state:\n");
    dump_body_position (convex_body.get ());
    printf ("triangle mesh body state:\n");
    dump_body_position (triangle_mesh_body.get ());
    printf ("compound body state:\n");
    dump_body_position (compound_body.get ());
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
