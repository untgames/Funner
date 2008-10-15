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

typedef Vertex<Position3f, Normalf, Color4ub, TexChannel<0>::Coord2f> CustomVertex;

#endif
