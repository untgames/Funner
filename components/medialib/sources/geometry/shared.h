#ifndef MEDIALIB_GEOMETRY_SHARED_HEADER
#define MEDIALIB_GEOMETRY_SHARED_HEADER

#include <memory.h>
#include <float.h>

#include <stl/string>
#include <stl/vector>
#include <stl/hash_map>

#include <xtl/function.h>
#include <xtl/iterator_range.h>
#include <xtl/stride_ptr.h>
#include <xtl/bind.h>
#include <xtl/ref.h>
#include <xtl/reference_counter.h>

#include <mathlib.h>

#include <common/strlib.h>
#include <common/hash.h>
#include <common/exception.h>
#include <common/xml_writer.h>
#include <common/parser.h>

#include <media/geometry/mesh_model.h>

#include <shared/resource_holder.h>
#include <shared/clone.h>
#include <shared/resource_manager.h>

namespace media
{

namespace geometry
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Буфер
///////////////////////////////////////////////////////////////////////////////////////////////////
class Buffer
{
  public:
    Buffer  ();
    Buffer  (size_t size);
    Buffer  (const Buffer&);
    ~Buffer ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размер буфера / объём буфера (количество доступной памяти)
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Size     () const { return finish - start; }
    size_t Capacity () const { return end_of_storage - start; }
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///Данные
///////////////////////////////////////////////////////////////////////////////////////////////////
    void* Data () { return start; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Изменение размера буфера / резервирование памяти
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Resize  (size_t new_size);
    void Reserve (size_t new_size);
    
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

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сохранение / загрузка мешей
///////////////////////////////////////////////////////////////////////////////////////////////////
void xmesh_save_model (const char* file_name, const MeshModel& model);
void xmesh_load_model (const char* file_name, MeshModel& model);
           
///////////////////////////////////////////////////////////////////////////////////////////////////
///Система управления меш-моделями
///////////////////////////////////////////////////////////////////////////////////////////////////
class MeshSystemImpl: public ResourceManager<MeshSystem::LoadHandler, MeshSystem::SaveHandler>
{
  public:
    MeshSystemImpl ();
};

typedef common::Singleton<MeshSystemImpl> MeshSystemSingleton;

}

}

#endif
