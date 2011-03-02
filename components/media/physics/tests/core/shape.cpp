#include "shared.h"

using namespace media::physics;
using namespace media::physics::shapes;

const char* get_type_name (ShapeType type)
{
  switch (type)
  {
    case ShapeType_Box:          return "Box";
    case ShapeType_Sphere:       return "Sphere";
    case ShapeType_Capsule:      return "Capsule";
    case ShapeType_Plane:        return "Plane";
    case ShapeType_TriangleMesh: return "TriangleMesh";
    case ShapeType_Compound:     return "Compound";
    default:                     return "Unknown";
  }
}

void dump (const Box& box)
{
  printf ("  half dimensions ");
  dump (box.half_dimensions);
  printf ("\n");
}

void dump (const Sphere& sphere)
{
  printf ("  radius %.2f\n", sphere.radius);
}

void dump (const Capsule& capsule)
{
  printf ("  radius %.2f height %.2f\n", capsule.radius, capsule.height);
}

void dump (const Plane& plane)
{
  printf ("  normal ");
  dump (plane.normal);
  printf (" d = %.2f\n", plane.d);
}

void dump (const TriangleMesh& mesh)
{
  printf ("  mesh name is '%s'\n", mesh.Name ());
  printf ("  is convex = %c\n", mesh.IsConvex () ? 'y' : 'n');
  printf ("  vertices count = %u\n", mesh.VerticesCount ());

  if (mesh.VerticesCount ())
  {
    printf ("  vertices:\n");

    const math::vec3f* vertex = mesh.Vertices ();

    for (size_t i = 0, count = mesh.VerticesCount (); i < count; i++, vertex++)
    {
      printf ("    ");
      dump (*vertex);
      printf ("\n");
    }
  }

  printf ("  triangles count = %u\n", mesh.TrianglesCount ());

  if (mesh.TrianglesCount ())
  {
    printf ("  triangles:\n");

    const TriangleIndices* triangle = mesh.Triangles ();

    for (size_t i = 0, count = mesh.TrianglesCount (); i < count; i++, triangle++)
      printf ("    %u %u %u\n", (*triangle) [0], (*triangle) [1], (*triangle) [2]);
  }
}

void dump (const Compound& compound)
{
  printf ("  shapes count = %u\n", compound.Size ());

  if (!compound.Size ())
    return;

  printf ("  shapes:\n");

  for (size_t i = 0, count = compound.Size (); i < count; i++)
  {
    printf ("    name '%s' position ", compound.Shape (i).Name ());
    dump (compound.Transform (i).position);
    printf (" orientation ");
    dump (compound.Transform (i).orientation);
    printf ("\n");
  }
}

void dump (const Shape& shape)
{
  printf ("Shape '%s':\n", shape.Name ());
  printf ("  Margin = %f\n", shape.Margin ());
  printf ("  Type = '%s'\n", get_type_name (shape.Type ()));

  switch (shape.Type ())
  {
    case ShapeType_Box:
      dump (shape.Data<Box> ());
      break;
    case ShapeType_Sphere:
      dump (shape.Data<Sphere> ());
      break;
    case ShapeType_Capsule:
      dump (shape.Data<Capsule> ());
      break;
    case ShapeType_Plane:
      dump (shape.Data<Plane> ());
      break;
    case ShapeType_TriangleMesh:
      dump (shape.Data<TriangleMesh> ());
      break;
    case ShapeType_Compound:
      dump (shape.Data<Compound> ());
      break;
  }
}

int main ()
{
  printf ("Results of shape_test:\n");

  try
  {
    Shape shape, clonned_shape;

    shape.SetData (Box ());
    shape.Rename ("default box shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    shape.SetData (Box (math::vec3f (2.f, 3.f, 4.f)));
    shape.SetMargin (0.1f);
    shape.Rename ("box shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    shape.SetData (Sphere ());
    shape.Rename ("default sphere shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    shape.SetData (Sphere (2.f));
    shape.Rename ("sphere shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    shape.SetData (Capsule ());
    shape.Rename ("default capsule shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    shape.SetData (Capsule (2.f, 3.f));
    shape.Rename ("capsule shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    shape.SetData (Plane ());
    shape.Rename ("default plane shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    shape.SetData (Plane (math::vec3f (0.1f, 0.2f, 0.3f), 10.f));
    shape.Rename ("plane shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    shape.SetData (TriangleMesh ());
    shape.Rename ("default triangle mesh shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    {
      TriangleMesh mesh;

      mesh.Rename ("mesh");
      mesh.SetConvex (false);
      mesh.SetVerticesCount (4);
      mesh.SetTrianglesCount (2);

      for (size_t i = 0; i < 4; i++)
        mesh.Vertices () [i] = i;

      TriangleIndices* indices = mesh.Triangles ();

      (*indices) [0] = 0;
      (*indices) [1] = 1;
      (*indices) [2] = 2;

      indices++;

      (*indices) [0] = 3;
      (*indices) [1] = 2;
      (*indices) [2] = 1;

      shape.SetData (mesh);
      shape.Rename ("triangle mesh shape");
      dump (shape);
      clonned_shape = shape.Clone ();
      dump (clonned_shape);
    }

    shape.SetData (Compound ());
    shape.Rename ("default compound shape");
    dump (shape);
    clonned_shape = shape.Clone ();
    dump (clonned_shape);

    {
      Shape shape1, shape2, shape3;

      shape1.Rename ("shape1");
      shape2.Rename ("shape2");
      shape3.Rename ("shape3");

      Compound compound;

      compound.Attach (shape1, math::vec3f (1), math::quatf (0.1f, 0.2f, 0.3f, 0.4f));
      compound.Attach (shape2, math::vec3f (2), math::quatf (0.2f, 0.3f, 0.4f, 0.5f));

      Compound::ShapeTransform shape3_transform;

      compound.Attach (shape3, shape3_transform);

      compound.Detach (shape2);

      Compound::ShapeTransform shape2_transform (math::vec3f (3), math::quatf (0.3f, 0.4f, 0.5f, 0.6f));

      compound.Attach (shape2, shape2_transform);

      shape.SetData (compound);
      shape.Rename ("compound shape");
      dump (shape);
      clonned_shape = shape.Clone ();
      dump (clonned_shape);
    }
  }
  catch (xtl::exception& e)
  {
    printf ("exception '%s'\n", e.what ());
  }

  return 0;
}
