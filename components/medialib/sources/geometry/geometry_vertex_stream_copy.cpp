#include "shared.h"

using namespace medialib::geometry;
using namespace common;
using namespace math;

namespace
{

/*
    Генерация исключения несовместимости форматов преобразования
*/

void raise_incompatible (VertexAttributeType src_type, VertexAttributeType dst_type)
{
  RaiseNotSupported ("medialib::geometry::copy", "Convertion from vertex type '%s' to vertex type '%s' not supported",
                     get_type_name (src_type), get_type_name (dst_type));
}

/*
    Шаблонная функция копирования
*/

inline void copy_attribute (const VertexInfluence& src, VertexInfluence& dst)
{
  dst = src;
}

template <class T1, size_t Size1, class T2, size_t Size2>
inline void copy_attribute (const vec<T1, Size1>& src, vec<T2, Size2>& dst)
{
  size_t min_size, max_size, i;
  
  if (Size1 < Size2) min_size = Size1, max_size = Size2;
  else               min_size = Size2, max_size = Size1;

  for (i=0; i<min_size; i++) dst [i] = static_cast<T2> (src [i]);
  for (; i<max_size; i++)    dst [i] = static_cast<T2> (0);
}

template <class SrcT, class DstT>
inline void copy_internal (size_t vertices_count, const void* source, size_t source_stride, void* destination, size_t destination_stride)
{
  const char* src_ptr = static_cast<const char*> (source);
        char* dst_ptr = static_cast<char*> (destination);
        
  for (;vertices_count--; src_ptr += source_stride, dst_ptr += destination_stride)
    copy_attribute (*reinterpret_cast<const SrcT*> (src_ptr), *reinterpret_cast<DstT*> (dst_ptr));
}

template <class SrcT>
void copy_internal
 (size_t              vertices_count,
  const void*         source,
  VertexAttributeType source_type,
  size_t              source_stride,
  void*               destination,
  VertexAttributeType destination_type,
  size_t              destination_stride)
{
  switch (destination_type)
  {
    case VertexAttributeType_Float2:
      copy_internal<SrcT, vec2f> (vertices_count, source, source_stride, destination, destination_stride);
      break;
    case VertexAttributeType_Float3:
      copy_internal<SrcT, vec3f> (vertices_count, source, source_stride, destination, destination_stride);
      break;
    case VertexAttributeType_Float4:
      copy_internal<SrcT, vec4f> (vertices_count, source, source_stride, destination, destination_stride);
      break;
    case VertexAttributeType_Short2:
      copy_internal<SrcT, vec2s> (vertices_count, source, source_stride, destination, destination_stride);
      break;
    case VertexAttributeType_Short3:
      copy_internal<SrcT, vec3s> (vertices_count, source, source_stride, destination, destination_stride);
      break;
    case VertexAttributeType_Short4:
      copy_internal<SrcT, vec4s> (vertices_count, source, source_stride, destination, destination_stride);
      break;
    case VertexAttributeType_UByte4:
      copy_internal<SrcT, vec4ub> (vertices_count, source, source_stride, destination, destination_stride);
      break;
    default:
      raise_incompatible (source_type, destination_type);
      break;
  }
}

}

/*
    Копирование массивов атрибутов с преобразованием формата
*/

namespace medialib
{

namespace geometry
{

void copy
 (size_t              vertices_count,
  const void*         source,
  VertexAttributeType source_type,
  size_t              source_stride,
  void*               destination,
  VertexAttributeType destination_type,
  size_t              destination_stride)
{
  if (!source && vertices_count)
    RaiseNullArgument ("medialib::geometry::copy", "source");
    
  if (!destination && vertices_count)
    RaiseNullArgument ("medialib::geometry::copy", "destination");
    
  switch (source_type)
  {
    case VertexAttributeType_Float2:
      copy_internal<vec2f> (vertices_count, source, source_type, source_stride, destination, destination_type, destination_stride);
      break;
    case VertexAttributeType_Float3:
      copy_internal<vec3f> (vertices_count, source, source_type, source_stride, destination, destination_type, destination_stride);
      break;
    case VertexAttributeType_Float4:
      copy_internal<vec4f> (vertices_count, source, source_type, source_stride, destination, destination_type, destination_stride);
      break;
    case VertexAttributeType_Short2:
      copy_internal<vec2s> (vertices_count, source, source_type, source_stride, destination, destination_type, destination_stride);
      break;
    case VertexAttributeType_Short3:
      copy_internal<vec3s> (vertices_count, source, source_type, source_stride, destination, destination_type, destination_stride);
      break;
    case VertexAttributeType_Short4:
      copy_internal<vec4s> (vertices_count, source, source_type, source_stride, destination, destination_type, destination_stride);
      break;
    case VertexAttributeType_UByte4:
      copy_internal<vec4ub> (vertices_count, source, source_type, source_stride, destination, destination_type, destination_stride);
      break;
    case VertexAttributeType_Influence:
       switch (destination_type)
       {
         case VertexAttributeType_Influence:
           copy_internal<VertexInfluence, VertexInfluence> (vertices_count, source, source_stride, destination, destination_stride);
           break;
         default:
           raise_incompatible (source_type, destination_type);
           break;
       }
    
      break;
    default:
      raise_incompatible (source_type, destination_type);
      break;
  }
}

}

}
