#ifndef MEDIALIB_MESH_TESTS_HEADER
#define MEDIALIB_MESH_TESTS_HEADER

#include <stdio.h>
#include <exception>
#include <media/mesh/mesh.h>
#include <media/mesh/vertex.h>

using namespace medialib;
using namespace math;

typedef vertex::Vertex<vertex::Position3f, vertex::Normalf, vertex::Color4ub, vertex::TexChannel<0>::Coord2f> CustomVertex;

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
    dump (format [i]);
}

#endif
