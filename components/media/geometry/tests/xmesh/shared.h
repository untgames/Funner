#ifndef MEDIALIB_GEOMETRY_XMESH_TESTS_HEADER
#define MEDIALIB_GEOMETRY_XMESH_TESTS_HEADER

#include <cstdio>
#include <exception>

#include <xtl/function.h>

#include <media/mesh.h>

#include <common/log.h>

using namespace media::geometry;
using namespace media;
using namespace math;

template <class T> struct Tangent: public CustomAttribute<math::vector<T, 3> >
{
  static const char* name () { return "tangent"; }

  math::vector<T, 3> tangent;
};

typedef Vertex<Position3f, Normalf, Color4ub, TexChannel<0>::Coord2f, Tangent<float> > CustomVertex;

#endif
