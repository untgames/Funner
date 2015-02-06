#ifndef MEDIALIB_GEOMETRY_DEFINES_HEADER
#define MEDIALIB_GEOMETRY_DEFINES_HEADER

#include <cstddef>

namespace media
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///¬ершинный вес
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexWeight
{
  uint32_t joint_index;  //индекс соединени€
  float    joint_weight; //вес соединени€
  
  VertexWeight ();
  VertexWeight (uint32_t joint_index, float joint_weight);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///»ндексы отрезка вершинных весов
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexInfluence
{
  uint32_t first_weight, weights_count;
  
  VertexInfluence ();
  VertexInfluence (uint32_t first_weight, uint32_t weights_count);
};

#include <media/geometry/detail/defs.inl>

}

}

#endif
