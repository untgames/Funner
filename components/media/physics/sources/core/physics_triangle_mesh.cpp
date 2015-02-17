#include "shared.h"

using namespace media::physics::shapes;

/*
   Реализация сетки
*/

typedef xtl::uninitialized_storage<math::vec3f>     VerticesArray;
typedef xtl::uninitialized_storage<TriangleIndices> TrianglesArray;

struct TriangleMesh::Impl : public xtl::reference_counter
{
  stl::string    name;      //имя сетки
  bool           is_convex; //является ли выпуклым многогранником
  VerticesArray  vertices;  //вершины
  TrianglesArray triangles; //индексы треугольников

  Impl ()
    : is_convex (true)
    {}
};

/*
   Конструкторы / деструктор / присваивание
*/

TriangleMesh::TriangleMesh ()
  : impl (new Impl)
{
}

TriangleMesh::TriangleMesh (const TriangleMesh& source)
  : impl (source.impl)
{
  addref (impl);
}

TriangleMesh::TriangleMesh (Impl* in_impl)
  : impl (in_impl)
{
}

TriangleMesh::~TriangleMesh ()
{
  release (impl);
}

TriangleMesh& TriangleMesh::operator = (const TriangleMesh& source)
{
  TriangleMesh (source).Swap (*this);

  return *this;
}
    
/*
   Создание копии
*/

TriangleMesh TriangleMesh::Clone () const
{
  return TriangleMesh (new Impl (*impl));
}

/*
   Идентификатор
*/

size_t TriangleMesh::Id () const
{
  return (size_t)impl;
}

/*
   Имя сетки
*/

const char* TriangleMesh::Name () const
{
  return impl->name.c_str ();
}

void TriangleMesh::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::physics::TriangleMesh::Rename", "name");

  impl->name = name;
}

/*
   Является ли выпуклым многогранником
*/

bool TriangleMesh::IsConvex () const
{
  return impl->is_convex;
}

void TriangleMesh::SetConvex (bool convex)
{
  impl->is_convex = convex;
}

/*
   Работа с вершинами
*/

unsigned int TriangleMesh::VerticesCount () const
{
  return (unsigned int)impl->vertices.size ();
}

void TriangleMesh::SetVerticesCount (unsigned int count)
{
  impl->vertices.resize (count);
}

const math::vec3f* TriangleMesh::Vertices () const
{
  return const_cast<TriangleMesh&> (*this).Vertices ();
}

math::vec3f* TriangleMesh::Vertices ()
{
  return impl->vertices.data ();
}

/*
   Работа с индексами
*/

unsigned int TriangleMesh::TrianglesCount () const
{
  return (unsigned int)impl->triangles.size ();
}

void TriangleMesh::SetTrianglesCount (unsigned int count)
{
  impl->triangles.resize (count);
}

const TriangleIndices* TriangleMesh::Triangles () const
{
  return const_cast<TriangleMesh&> (*this).Triangles ();
}

TriangleIndices* TriangleMesh::Triangles ()
{
  return impl->triangles.data ();
}

/*
   Обмен
*/

void TriangleMesh::Swap (TriangleMesh& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace physics
{

/*
   Обмен
*/

void swap (TriangleMesh& mesh1, TriangleMesh& mesh2)
{
  mesh1.Swap (mesh2);
}

}

}
