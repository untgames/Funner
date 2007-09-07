#ifndef MEDIALIB_GEOMETRY_DEFINES_HEADER
#define MEDIALIB_GEOMETRY_DEFINES_HEADER

#include <stddef.h>
#include <media/clone_mode.h>

namespace media
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///¬ершинный вес
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexWeight
{
  size_t joint_index;  //индекс соединени€
  float  joint_weight; //вес соединени€
  
  VertexWeight ();
  VertexWeight (size_t joint_index, float joint_weight);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///»ндексы отрезка вершинных весов
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexInfluence
{
  size_t first_weight, weights_count;
  
  VertexInfluence ();
  VertexInfluence (size_t first_weight, size_t weights_count);
};

#include <media/geometry/detail/defs.inl>

}

}

#endif
