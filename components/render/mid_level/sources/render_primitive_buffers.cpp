#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

//TODO: index types

/*
    Описание реализации PrimitiveBuffers
*/

namespace
{

template <class T> struct BufferHolder
{
  T                 source;
  LowLevelBufferPtr buffer;
  MeshBufferUsage   usage;
  
  BufferHolder (const T& in_source, const LowLevelBufferPtr& in_buffer, MeshBufferUsage in_usage)
    : source (in_source)
    , buffer (in_buffer)
    , usage (in_usage)
  {
  }
};

struct VertexBufferHolder: public BufferHolder<media::geometry::VertexStream>
{
  size_t vertex_size;
  size_t vertices_count;

  VertexBufferHolder (const media::geometry::VertexStream& vs, const LowLevelBufferPtr& buffer, MeshBufferUsage usage)
    : BufferHolder<media::geometry::VertexStream> (vs, buffer, usage)
    , vertex_size (vs.VertexSize ())
    , vertices_count (vs.Size ())
  {
  }
};

struct IndexBufferHolder: public BufferHolder<media::geometry::IndexBuffer>
{
  size_t indices_count;
  
  IndexBufferHolder (const media::geometry::IndexBuffer& ib, const LowLevelBufferPtr& buffer, MeshBufferUsage usage)
    : BufferHolder<media::geometry::IndexBuffer> (ib, buffer, usage)
    , indices_count (ib.Size ())
  {
  }
};

typedef stl::hash_map<size_t, VertexBufferHolder> VertexBufferMap;
typedef stl::hash_map<size_t, IndexBufferHolder>  IndexBufferMap;

}

struct PrimitiveBuffersImpl::Impl
{
  DeviceManagerPtr device_manager; //менеджер устройства отрисовки
  MeshBufferUsage  lines_usage;    //режим использования буферов для линий
  MeshBufferUsage  sprites_usage;  //режим использования буферов для спрайтов
  VertexBufferMap  vertex_buffers; //кэш вершинных буферов
  IndexBufferMap   index_buffers;  //кэш индексных буферов
  
  Impl (const DeviceManagerPtr& in_device_manager, MeshBufferUsage in_lines_usage, MeshBufferUsage in_sprites_usage)
    : device_manager (in_device_manager)
    , lines_usage (in_lines_usage)
    , sprites_usage (in_sprites_usage)
  {
    switch (lines_usage)
    {
      case MeshBufferUsage_Static:
      case MeshBufferUsage_Dynamic:
      case MeshBufferUsage_Stream:      
        break;
      default:
        throw xtl::make_argument_exception ("", "lines_usage", lines_usage);
    }
    
    switch (sprites_usage)
    {
      case MeshBufferUsage_Static:
      case MeshBufferUsage_Dynamic:
      case MeshBufferUsage_Stream:      
        break;
      default:
        throw xtl::make_argument_exception ("", "sprites_usage", sprites_usage);
    }    
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

PrimitiveBuffersImpl::PrimitiveBuffersImpl (const DeviceManagerPtr& device_manager, MeshBufferUsage lines_usage, MeshBufferUsage sprites_usage)
  : impl (new Impl (device_manager, lines_usage, sprites_usage))
{
}

PrimitiveBuffersImpl::~PrimitiveBuffersImpl ()
{
}

/*
    Создание отображений буферов
*/

LowLevelBufferPtr PrimitiveBuffersImpl::CreateVertexBuffer (const media::geometry::VertexStream& vs, MeshBufferUsage usage)
{
  try
  {
    VertexBufferMap::iterator iter = impl->vertex_buffers.find (vs.Id ());
    
    if (iter != impl->vertex_buffers.end ())
      return iter->second.buffer;
      
    BufferDesc desc;

    memset (&desc, 0, sizeof desc);

    desc.size         = vs.Size () * vs.VertexSize ();
    desc.bind_flags   = BindFlag_VertexBuffer;
    desc.access_flags = AccessFlag_Read | AccessFlag_Write;
    
    switch (usage)
    {
      case MeshBufferUsage_Static:
        desc.usage_mode = UsageMode_Static;
        break;
      case MeshBufferUsage_Dynamic:
        desc.usage_mode = UsageMode_Dynamic;
        break;
      case MeshBufferUsage_Stream:      
        desc.usage_mode = UsageMode_Stream;
        break;
      default:
        throw xtl::make_argument_exception ("", "usage", usage);
    }
    
    LowLevelBufferPtr buffer (impl->device_manager->Device ().CreateBuffer (desc), false);

    buffer->SetData (0, desc.size, vs.Data ());
    
    return buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::PrimitiveBuffersImpl::CreateVertexBuffer");
    throw;
  }
}

LowLevelBufferPtr PrimitiveBuffersImpl::CreateIndexBuffer (const media::geometry::IndexBuffer& ib, MeshBufferUsage usage)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (ib.Id ());

    if (iter != impl->index_buffers.end ())
      return iter->second.buffer;
      
    BufferDesc desc;

    memset (&desc, 0, sizeof desc);

    desc.size         = ib.Size () * sizeof (unsigned int);
    desc.bind_flags   = BindFlag_IndexBuffer;
    desc.access_flags = AccessFlag_ReadWrite;
    
    switch (usage)
    {
      case MeshBufferUsage_Static:
        desc.usage_mode = UsageMode_Static;
        break;
      case MeshBufferUsage_Dynamic:
        desc.usage_mode = UsageMode_Dynamic;
        break;
      case MeshBufferUsage_Stream:      
        desc.usage_mode = UsageMode_Stream;
        break;
      default:
        throw xtl::make_argument_exception ("", "usage", usage);
    }
    
    LowLevelBufferPtr buffer (impl->device_manager->Device ().CreateBuffer (desc), false);

    buffer->SetData (0, desc.size, ib.Data ());
    
    return buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::PrimitiveBuffersImpl::CreateIndexBuffer");
    throw;
  }
}

/*
    Добавление буферов
*/

void PrimitiveBuffersImpl::Add (const media::geometry::VertexStream& vs, MeshBufferUsage usage)
{
  try
  {
    VertexBufferMap::iterator iter = impl->vertex_buffers.find (vs.Id ());
    
    if (iter != impl->vertex_buffers.end ())
      return;    
    
    LowLevelBufferPtr buffer = CreateVertexBuffer (vs, usage);
    
    impl->vertex_buffers.insert_pair (vs.Id (), VertexBufferHolder (vs, buffer, usage));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::PrimitiveBuffersImpl::Add(const VertexStream&,MeshBufferUsage)");
    throw;
  }
}

void PrimitiveBuffersImpl::Add (const media::geometry::IndexBuffer& ib, MeshBufferUsage usage)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (ib.Id ());
    
    if (iter != impl->index_buffers.end ())
      return;    
    
    LowLevelBufferPtr buffer = CreateIndexBuffer (ib, usage);
    
    impl->index_buffers.insert_pair (ib.Id (), IndexBufferHolder (ib, buffer, usage));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::PrimitiveBuffersImpl::Add(const IndexBuffer&,MeshBufferUsage)");
    throw;
  }
}

/*
    Обновление буферов
*/

void PrimitiveBuffersImpl::Update (const media::geometry::VertexStream& vs)
{
  try
  {
    VertexBufferMap::iterator iter = impl->vertex_buffers.find (vs.Id ());
    
    if (iter == impl->vertex_buffers.end ())
      throw xtl::make_argument_exception ("", "buffer.Id()", vs.Id (), "Vertex stream not added to primitive buffers");
      
    VertexBufferHolder& holder = iter->second;
    
    if (holder.vertex_size != vs.VertexSize ())
      throw xtl::format_operation_exception ("", "Vertex size %u differes from source vertex stream size %u", vs.VertexSize (), holder.vertex_size);
      
    if (holder.vertices_count != vs.Size ())
      throw xtl::format_operation_exception ("", "Vertices count %u differes from source vertices count %u", vs.Size (), holder.vertices_count);
          
    holder.buffer->SetData (0, vs.Size () * vs.VertexSize (), vs.Data ());    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::PrimitiveBuffersImpl::Update(const VertexStream&)");
    throw;
  }
}

void PrimitiveBuffersImpl::Update (const media::geometry::IndexBuffer& ib)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (ib.Id ());
    
    if (iter == impl->index_buffers.end ())
      throw xtl::make_argument_exception ("", "buffer.Id()", ib.Id (), "Index buffer not added to primitive buffers");
      
    IndexBufferHolder& holder = iter->second;
    
    if (holder.indices_count != ib.Size ())
      throw xtl::format_operation_exception ("", "Indices count %u differes from source indices count %u", ib.Size (), holder.indices_count);
          
    holder.buffer->SetData (0, ib.Size () * sizeof (unsigned int), ib.Data ());    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::PrimitiveBuffersImpl::Update(const IndexBuffer&)");
    throw;
  }
}

/*
    Удаление буферов
*/

void PrimitiveBuffersImpl::Remove (const media::geometry::VertexStream& vs)
{
  impl->vertex_buffers.erase (vs.Id ());
}

void PrimitiveBuffersImpl::Remove (const media::geometry::IndexBuffer& ib)
{
  impl->index_buffers.erase (ib.Id ());
}

void PrimitiveBuffersImpl::RemoveAll ()
{
  impl->vertex_buffers.clear ();
  impl->index_buffers.clear ();
}

/*
    Резервирование вспомогательных примитивов
*/

void PrimitiveBuffersImpl::ReserveLines (size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void PrimitiveBuffersImpl::ReserveSprites (size_t count)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t PrimitiveBuffersImpl::LinesCapacity ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t PrimitiveBuffersImpl::SpritesCapacity ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Режим использования буферов вспомогательных примитивов
*/

MeshBufferUsage PrimitiveBuffersImpl::LinesBufferUsage ()
{
  return impl->lines_usage;
}

MeshBufferUsage PrimitiveBuffersImpl::SpritesBufferUsage ()
{
  return impl->sprites_usage;
}
