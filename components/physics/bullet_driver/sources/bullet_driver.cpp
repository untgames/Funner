#include "shared.h"

using namespace physics::low_level;
using namespace physics::low_level::bullet;
using namespace common;

namespace
{

/*
    Константы
*/

const char*  DRIVER_NAME    = "Bullet";                   //имя драйвера
const char*  COMPONENT_NAME = "physics.low_level.bullet"; //имя компонента

}

/*
    Описание реализации драйвера физической системы на основе библиотеки Bullet
*/

struct Driver::Impl
{
};

/*
    Конструктор
*/

Driver::Driver ()
  : impl (new Impl)
  {}

/*
    Описание драйвера
*/

const char* Driver::GetDescription ()
{
  return "physics::low_level::bullet::Driver";
}

/*
   Создание сцены
*/

Scene* Driver::CreateScene ()
{
  return new Scene ();
}

/*
   Создание геометрических тел
*/

Shape* Driver::CreateBoxShape (const math::vec3f& half_dimensions)
{
  btVector3 half_extents (half_dimensions.x, half_dimensions.y, half_dimensions.z);

  return new Shape (new btBoxShape (half_extents));
}

Shape* Driver::CreateSphereShape (float radius)
{
  return new Shape (new btSphereShape (radius));
}

Shape* Driver::CreateCapsuleShape (float radius, float height)
{
  return new Shape (new btCapsuleShape (radius, height));
}

Shape* Driver::CreatePlaneShape (const math::vec3f& normal, float d)
{
  btVector3 plane_normal (normal.x, normal.y, normal.z);

  return new Shape (new btStaticPlaneShape (plane_normal, d));
}

Shape* Driver::CreateConvexShape (size_t vertices_count, math::vec3f* vertices)
{
  static const char* METHOD_NAME = "physics::low_level::bullet::Driver::CreateConvexShape";

  if (!vertices_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "vertices_count");

  if (!vertices)
    throw xtl::make_null_argument_exception (METHOD_NAME, "vertices");

  xtl::uninitialized_storage<btScalar> bullet_vertices (vertices_count * 3);

  math::vec3f* current_vertex        = vertices;
  btScalar*    current_bullet_vertex = bullet_vertices.data ();

  for (size_t i = 0; i < vertices_count; i++, current_vertex++, current_bullet_vertex += 3)
  {
    current_bullet_vertex [0] = current_vertex->x;
    current_bullet_vertex [1] = current_vertex->y;
    current_bullet_vertex [2] = current_vertex->z;
  }

  return new Shape (new btConvexHullShape (bullet_vertices.data (), vertices_count, 3 * sizeof (float)));
}

Shape* Driver::CreateTriangleMeshShape (size_t vertices_count, math::vec3f* vertices, size_t triangles_count, size_t* triangles)
{
  static const char* METHOD_NAME = "physics::low_level::bullet::Driver::CreateTriangleMeshShape";

  if (!vertices_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "vertices_count");

  if (!vertices)
    throw xtl::make_null_argument_exception (METHOD_NAME, "vertices");

  if (!triangles_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "triangles_count");

  if (!triangles)
    throw xtl::make_null_argument_exception (METHOD_NAME, "triangles");

  btTriangleMesh* triangle_mesh = new btTriangleMesh (true, false);

  triangle_mesh->preallocateIndices  (triangles_count * 3);
  triangle_mesh->preallocateVertices (vertices_count);

  math::vec3f* current_vertex = vertices;

  for (size_t i = 0; i < vertices_count; i++, current_vertex++)
    triangle_mesh->findOrAddVertex (btVector3 (current_vertex->x, current_vertex->y, current_vertex->z), false);

  size_t* current_index = triangles;

  for (size_t i = 0, indices_count = triangles_count * 3; i < indices_count; i++, current_index++)
    triangle_mesh->addIndex (*current_index);

  triangle_mesh->getIndexedMeshArray () [0].m_numTriangles += triangles_count;

  return new TriangleMeshShape (new btBvhTriangleMeshShape (triangle_mesh, true), triangle_mesh);
}

Shape* Driver::CreateCompoundShape (size_t shapes_count, IShape** shapes, Transform* local_transforms)
{
  static const char* METHOD_NAME = "physics::low_level::bullet::Driver::CreateCompoundShape";

  if (!shapes_count)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shapes_count");

  if (!shapes)
    throw xtl::make_null_argument_exception (METHOD_NAME, "shapes");

  if (!local_transforms)
    throw xtl::make_null_argument_exception (METHOD_NAME, "local_transforms");

  for (size_t i = 0; i < shapes_count; i++)
    cast_object<Shape, IShape> (shapes [i], METHOD_NAME, "shapes");

  btCompoundShape* compound_shape = new btCompoundShape ();

  btVector3    bullet_vector;
  btQuaternion bullet_quaternion;

  for (size_t i = 0; i < shapes_count; i++)
  {
    bullet_vector_from_vector         (local_transforms [i].position, bullet_vector);
    bullet_quaternion_from_quaternion (local_transforms [i].orientation, bullet_quaternion);

    btTransform shape_transform (bullet_quaternion, bullet_vector);

    compound_shape->addChildShape (shape_transform, ((Shape**)shapes) [i]->BulletCollisionShape ());
  }

  return new Shape (compound_shape);
}

/*
    Компонент драйвера
*/

namespace
{

class BulletDriverComponent
{
  public:
    BulletDriverComponent ()
    {
      DriverManager::RegisterDriver (DRIVER_NAME, xtl::com_ptr<Driver> (new Driver, false).get ());
    }
};

}

extern "C"
{

common::ComponentRegistrator<BulletDriverComponent> BulletDriver (COMPONENT_NAME);

}
