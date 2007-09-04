#ifndef MEDIALIB_GEOMETRY_TESTS_HEADER
#define MEDIALIB_GEOMETRY_TESTS_HEADER

#include <stdio.h>
#include <exception>
#include <media/mesh.h>
#include <xtl/function.h>

using namespace medialib::geometry;
using namespace medialib;
using namespace math;

typedef Vertex<Position3f, Normalf, Color4ub, TexChannel<0>::Coord2f> CustomVertex;

//вывод вершинного атрибута
void dump (const VertexAttribute& attribute)
{
  printf ("  attribute: semantic='%s' type='%s' offset=%u\n", get_semantic_name (attribute.semantic),
          get_type_name (attribute.type), attribute.offset);
}

//вывод формата меша
void dump (const VertexFormat& format, size_t vertex_size=size_t (-1))
{  
  if (vertex_size != (size_t)-1)
  {
    printf ("vertex format (%u attributes, min_vertex_size=%u, vertex_size=%u):\n", format.AttributesCount (),
            format.GetMinimalVertexSize (), vertex_size);
  }
  else  
  {
    printf ("vertex format (%u attributes, min_vertex_size=%u):\n", format.AttributesCount (), format.GetMinimalVertexSize ());
  }
  
  for (size_t i=0; i<format.AttributesCount (); i++)
    dump (format.Attribute (i));
}

//вывод вершины
void dump (const CustomVertex& v)
{
  printf ("  xyz=[%g %g %g] normal=[%g %g %g] rgba=[%u %u %u %u] st=[%g %g]\n", v.position.x, v.position.y, v.position.z,
          v.normal.x, v.normal.y, v.normal.z, v.color.x, v.color.y, v.color.z, v.color.w, v.texcoord0.x, v.texcoord0.y);
}

//вывод потока вершин
void dump (const VertexStream& s)
{
  printf ("vertex stream (%u vertices):\n", s.Size ());

  const CustomVertex* verts = s.Data<CustomVertex> ();
  
  if (!verts)
  {
    printf ("  wrong format!\n");
    return;
  }

  for (size_t i=0; i<s.Size (); i++)
    dump (verts [i]);
}

#endif
