#include "shared.h"

const size_t NUM_VERTS_X = 5;
const size_t NUM_VERTS_Y = 5;

int main ()
{
  printf ("Results of shapes_test:\n");
  
  try
  {
    PhysicsManager manager (DRIVER_NAME);

    Scene scene (manager.CreateScene ());

    math::vec3f mesh_vertices []        = { math::vec3f (0, -2, 0), math::vec3f (0.5, 0, 0), math::vec3f (0, 0, 1), math::vec3f (-1, 0, 0) };
    size_t      triangles_indices []    = { 0, 1, 2, 0, 3, 1, 0, 2, 3, 3, 2, 1};
    size_t      mesh_vertices_count     = sizeof (mesh_vertices) / sizeof (*mesh_vertices);
    size_t      triangles_indices_count = sizeof (triangles_indices) / sizeof (*triangles_indices);

    media::physics::Shape                media_shape;
    media::physics::shapes::TriangleMesh mesh;

    media_shape.SetMargin (0.04f);

    mesh.SetConvex (true);
    mesh.SetVerticesCount (mesh_vertices_count);
    mesh.SetTrianglesCount (triangles_indices_count / 3);

    for (size_t i = 0; i < mesh_vertices_count; i++)
      mesh.Vertices () [i] = mesh_vertices [i];

    media::physics::shapes::TriangleIndices* indices = mesh.Triangles ();

    for (size_t i = 0; i < triangles_indices_count; i++)
      (*indices) [i] = triangles_indices [i];

    media_shape.SetData (mesh);

    Shape box_shape           (manager.CreateBoxShape     (1.f)),
          sphere_shape        (manager.CreateSphereShape  (1.f)),
          capsule_shape       (manager.CreateCapsuleShape (1.f, 0.5f)),
          plane_shape         (manager.CreateBoxShape     (math::vec3f (50.f, 0.01f, 50.f))),
          convex_shape        (manager.CreateShape        (media_shape));

    mesh.SetConvex (false);

    media_shape.SetMargin (0.f);
    media_shape.SetData (mesh);

    Shape triangle_mesh_shape (manager.CreateShape (media_shape));

    Shape  compounding_shapes [] = { box_shape, sphere_shape };
    size_t compounding_shapes_count = sizeof (compounding_shapes) / sizeof (*compounding_shapes);

    Transform compounding_shapes_transforms [2];

    compounding_shapes_transforms[0].position.y = -4;

    ShapeList compound_shape_list;

    for (size_t i = 0; i < compounding_shapes_count; i++)
      compound_shape_list.Add (compounding_shapes [i], compounding_shapes_transforms [i]);

    Shape compound_shape (manager.CreateCompoundShape (compound_shape_list));

    RigidBody box_body           (scene.CreateRigidBody (box_shape, 1)),
              sphere_body        (scene.CreateRigidBody (sphere_shape, 1)),
              capsule_body       (scene.CreateRigidBody (capsule_shape, 1)),
              plane_body         (scene.CreateRigidBody (plane_shape, 0)),
              convex_body        (scene.CreateRigidBody (convex_shape, 1)),
              triangle_mesh_body (scene.CreateRigidBody (triangle_mesh_shape, 1)),
              compound_body      (scene.CreateRigidBody (compound_shape, 1));

    Transform body_transform;

    body_transform.position.x = -10;
    box_body.SetWorldTransform (body_transform);

    body_transform.position.x = -7;
    sphere_body.SetWorldTransform (body_transform);

    body_transform.position.x = -4;
    capsule_body.SetWorldTransform (body_transform);

    body_transform.position.x = -1;
    convex_body.SetWorldTransform (body_transform);

    body_transform.position.x = 2;
    triangle_mesh_body.SetWorldTransform (body_transform);

    body_transform.position.x = 5;
    compound_body.SetWorldTransform (body_transform);

    body_transform.position.x = 0;
    body_transform.position.y = -15;
    plane_body.SetWorldTransform (body_transform);

    printf ("plane body state:\n");
    dump_body_position (plane_body);
    printf ("box body state:\n");
    dump_body_position (box_body);
    printf ("sphere body state:\n");
    dump_body_position (sphere_body);
    printf ("capsule body state:\n");
    dump_body_position (capsule_body);
    printf ("convex body state:\n");
    dump_body_position (convex_body);
    printf ("triangle mesh body state:\n");
    dump_body_position (triangle_mesh_body);
    printf ("compound body state:\n");
    dump_body_position (compound_body);

    for (size_t i = 0; i < 10; i++)
      scene.PerformSimulation (1.f);

    printf ("Simulating ten seconds\n");

    printf ("plane body state:\n");
    dump_body_position (plane_body);
    printf ("box body state:\n");
    dump_body_position (box_body);
    printf ("sphere body state:\n");
    dump_body_position (sphere_body);
    printf ("capsule body state:\n");
    dump_body_position (capsule_body);
    printf ("convex body state:\n");
    dump_body_position (convex_body);
    printf ("triangle mesh body state:\n");
    dump_body_position (triangle_mesh_body);
    printf ("compound body state:\n");
    dump_body_position (compound_body);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
