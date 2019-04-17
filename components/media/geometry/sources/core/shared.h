#ifndef MEDIALIB_GEOMETRY_SHARED_HEADER
#define MEDIALIB_GEOMETRY_SHARED_HEADER

#include <cmath>

#include <float.h>
#include <string.h>

#include <stl/string>
#include <stl/vector>

#include <xtl/array>
#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/reference_counter.h>
#include <xtl/trackable.h>
#include <xtl/type.h>

#include <math/vector.h>

#include <common/component.h>
#include <common/hash.h>
#include <common/string.h>
#include <common/strlib.h>

#include <media/geometry/mesh_library.h>
#include <media/geometry/mesh_observer.h>
#include <media/resource_library.h>

namespace media
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Тип объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
enum ObjectType
{
  ObjectType_Mesh,
  ObjectType_MaterialMap,
  ObjectType_IndexBuffer,
  ObjectType_VertexBuffer,
  ObjectType_VertexStream,
  ObjectType_VertexWeightStream,

  ObjectType_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация пула идентификаторов
///////////////////////////////////////////////////////////////////////////////////////////////////
class IdPool
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Выделение идентификаторов
///////////////////////////////////////////////////////////////////////////////////////////////////
    static object_id_t AllocateId (ObjectType type);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буфер
///////////////////////////////////////////////////////////////////////////////////////////////////
class Buffer
{
  public:
    Buffer  ();
    Buffer  (uint32_t size);
    Buffer  (const Buffer&);
    ~Buffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер буфера / объём буфера (количество доступной памяти)
///////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t Size     () const { return (uint32_t)(finish - start); }
    uint32_t Capacity () const { return (uint32_t)(end_of_storage - start); }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Data () { return start; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение размера буфера / резервирование памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Resize  (uint32_t new_size);
    void Reserve (uint32_t new_size);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Buffer&);
    
  private:
    Buffer& operator = (const Buffer&); //no impl
  
  private:      
    char *start, *finish, *end_of_storage;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Копирование массивов атрибутов с преобразованием формата
///////////////////////////////////////////////////////////////////////////////////////////////////
void copy (size_t vertices_count, const void* source, VertexAttributeType source_type, size_t source_stride,
           void* destination, VertexAttributeType destination_type, size_t destination_stride);

}

}

#endif
