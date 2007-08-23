#ifndef MEDIALIB_GEOMETRY_DEFINES_HEADER
#define MEDIALIB_GEOMETRY_DEFINES_HEADER

#include <stddef.h>

namespace medialib
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Режим копирования буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
enum CloneMode
{
  CloneMode_Copy,     //копирование
  CloneMode_Instance, //инстанцирование (увеличение числа ссылок)
  CloneMode_Source,   //сохраняется режим копирования источника  
  
  CloneMode_Default = CloneMode_Copy
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Вершинный вес
///////////////////////////////////////////////////////////////////////////////////////////////////
struct VertexWeight
{
  size_t joint_index;  //индекс соединения
  float  joint_weight; //вес соединения
  
  VertexWeight ();
  VertexWeight (size_t joint_index, float joint_weight);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Индексы отрезка вершинных весов
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
