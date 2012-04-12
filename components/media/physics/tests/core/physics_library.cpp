#include "shared.h"

using namespace media::physics;
using namespace media::physics::shapes;

void dump (const PhysicsLibrary& library)
{
  printf ("library '%s':\n", library.Name ());
  printf ("  collision filters count %u\n", library.CollisionFilters ().Size ());
  printf ("  rigid bodies count %u\n", library.RigidBodies ().Size ());
  printf ("  materials count %u\n", library.Materials ().Size ());
  printf ("  shapes count %u\n", library.Shapes ().Size ());
  printf ("  triangle meshes count %u\n", library.TriangleMeshes ().Size ());

  if (library.CollisionFilters ().Size ())
    printf ("  collision filters:\n");
  for (PhysicsLibrary::CollisionFilterCollection::ConstIterator i = library.CollisionFilters ().CreateIterator (); i; ++i)
    printf ("    collision filters '%s' with '%s' collides %d\n", i->Group1Wildcard (), i->Group2Wildcard (), i->IsCollides ());

  if (library.RigidBodies ().Size ())
    printf ("  bodies:\n");
  for (PhysicsLibrary::RigidBodyCollection::ConstIterator i = library.RigidBodies ().CreateIterator (); i; ++i)
    printf ("    body '%s'\n", i->Name ());

  if (library.Materials ().Size ())
    printf ("  materials:\n");
  for (PhysicsLibrary::MaterialCollection::ConstIterator i = library.Materials ().CreateIterator (); i; ++i)
    printf ("    material '%s'\n", i->Name ());

  if (library.Shapes ().Size ())
    printf ("  shapes:\n");
  for (PhysicsLibrary::ShapeCollection::ConstIterator i = library.Shapes ().CreateIterator (); i; ++i)
    printf ("    shape '%s'\n", i->Name ());

  if (library.TriangleMeshes ().Size ())
    printf ("  meshes:\n");
  for (PhysicsLibrary::TriangleMeshCollection::ConstIterator i = library.TriangleMeshes ().CreateIterator (); i; ++i)
    printf ("    mesh '%s'\n", i->Name ());
}

int main ()
{
  printf ("Results of physics_library_test:\n");
  
  try
  {
    PhysicsLibrary library;
    
    library.Rename ("library1");

    const char *body_name []                 = { "body1", "body2", "body3", "body4" },
               *material_name []             = { "material1", "material2", "material3", "material4" },
               *shape_name []                = { "shape1", "shape2", "shape3", "shape4" },
               *mesh_name []                 = { "mesh1", "mesh2", "mesh3", "mesh4" },
               *collision_filters_groups1 [] = { "group1_1", "group1_2", "group1_3", "group1_4" },
               *collision_filters_groups2 [] = { "group2_1", "group2_2", "group2_3", "group2_4" };
    bool       collision_filters []          = { true, true, false, true };
    
    PhysicsLibrary::CollisionFilterCollection filter_collection   = library.CollisionFilters ();
    PhysicsLibrary::MaterialCollection        material_collection = library.Materials ();
    PhysicsLibrary::RigidBodyCollection       bodies_collection   = library.RigidBodies ();
    PhysicsLibrary::ShapeCollection           shape_collection    = library.Shapes ();
    PhysicsLibrary::TriangleMeshCollection    mesh_collection     = library.TriangleMeshes ();

    for (size_t i = 0; i < 4; i++)
    {
      RigidBody       body;
      Material        material;
      Shape           shape;
      TriangleMesh    mesh;
      CollisionFilter filter;

      body.Rename (body_name [i]);
      material.Rename (material_name [i]);
      shape.Rename (shape_name [i]);
      mesh.Rename (mesh_name [i]);
      filter.SetGroup1Wildcard (collision_filters_groups1 [i]);
      filter.SetGroup2Wildcard (collision_filters_groups2 [i]);
      filter.SetCollides (collision_filters [i]);

      bodies_collection.Attach (body_name [i], body);
      material_collection.Attach (material_name [i], material);
      shape_collection.Attach (shape_name [i], shape);
      mesh_collection.Attach (mesh_name [i], mesh);
      filter_collection.Add (filter);
    }  
    
    dump (library);
    
    printf ("detach object\n");
    
    bodies_collection.Detach ("body1");
    material_collection.Detach ("material2");
    shape_collection.Detach ("shape3");
    mesh_collection.Detach ("mesh4");
    filter_collection.Remove (2);
    
    dump (library);
    
    printf ("Has 'shape2' - %c\n", shape_collection.Find ("shape2") ? 'y' : 'n');
    printf ("Has 'shape3' - %c\n", shape_collection.Find ("shape3") ? 'y' : 'n');

    printf ("clear collections\n");
    
    bodies_collection.DetachAll ();
    material_collection.DetachAll ();
    shape_collection.DetachAll ();
    mesh_collection.DetachAll ();
    filter_collection.Clear ();
    
    dump (library);
  }
  catch (std::exception& exception)
  {
    printf ("exception: %s\n", exception.what ());
  }

  return 0;
}
