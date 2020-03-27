#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

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

struct VertexStreamHolder: public BufferHolder<media::geometry::VertexStream>
{
  size_t vertex_size;
  size_t vertices_count;

  VertexStreamHolder (const media::geometry::VertexStream& vs, const LowLevelBufferPtr& buffer, MeshBufferUsage usage)
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

struct VertexBufferHolder
{
  media::geometry::VertexBuffer source;
  VertexBufferPtr               buffer;
  
  VertexBufferHolder (const media::geometry::VertexBuffer& in_source, const VertexBufferPtr& in_buffer)
    : source (in_source)
    , buffer (in_buffer)
  {
  }
};

typedef stl::hash_map<media::geometry::object_id_t, VertexStreamHolder> VertexStreamMap;
typedef stl::hash_map<media::geometry::object_id_t, VertexBufferHolder> VertexBufferMap;
typedef stl::hash_map<media::geometry::object_id_t, IndexBufferHolder>  IndexBufferMap;
typedef stl::hash_map<media::geometry::object_id_t, LowLevelBufferPtr>  CacheBufferMap;
typedef stl::hash_map<media::geometry::object_id_t, VertexBufferPtr>    CacheVertexBufferMap;

}

struct PrimitiveBuffersImpl::Impl
{
  DeviceManagerPtr                        device_manager;       //менеджер устройства отрисовки
  VertexStreamMap                         vertex_streams;       //вершинные потоки
  VertexBufferMap                         vertex_buffers;       //вершинные буферы
  IndexBufferMap                          index_buffers;        //индексные буферы
  CacheBufferMap                          vertex_streams_cache; //кэш вершинных потоков
  CacheBufferMap                          index_buffers_cache;  //кэш индексных буферов
  CacheVertexBufferMap                    vertex_buffers_cache; //кэш вершинных буферов
  bool                                    cache_state;          //состояние кэша
  stl::auto_ptr<manager::BatchingManager> batching_manager;     //менеджер пакетирования
  
  Impl (const DeviceManagerPtr& in_device_manager)
    : device_manager (in_device_manager)   
    , cache_state (false)
  {
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

PrimitiveBuffersImpl::PrimitiveBuffersImpl (const DeviceManagerPtr& device_manager)
  : impl (new Impl (device_manager))
{
}

PrimitiveBuffersImpl::~PrimitiveBuffersImpl ()
{
}

/*
    Создание отображений буферов
*/

LowLevelBufferPtr PrimitiveBuffersImpl::CreateVertexStream (const media::geometry::VertexStream& vs, MeshBufferUsage usage, bool updatable)
{
  try
  {
    VertexStreamMap::iterator iter = impl->vertex_streams.find (vs.SourceId ());
    
    if (iter != impl->vertex_streams.end ())
      return iter->second.buffer;
      
    if (impl->cache_state)
    {
      CacheBufferMap::iterator iter = impl->vertex_streams_cache.find (vs.SourceId ());
      
      if (iter != impl->vertex_streams_cache.end ())
      {
        if (updatable)
          impl->vertex_streams.insert_pair (vs.SourceId (), VertexStreamHolder (vs, iter->second, usage));

        return iter->second;
      }
    }
      
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
    
    if (impl->cache_state)
      impl->vertex_streams_cache.insert_pair (vs.SourceId (), buffer);
    
    if (updatable)
      impl->vertex_streams.insert_pair (vs.SourceId (), VertexStreamHolder (vs, buffer, usage));

    return buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::CreateVertexStream");
    throw;
  }
}

VertexBufferPtr PrimitiveBuffersImpl::CreateVertexBuffer (const media::geometry::VertexBuffer& vb, MeshBufferUsage usage, bool updatable)
{
  try
  {
    VertexBufferMap::iterator iter = impl->vertex_buffers.find (vb.SourceId ());
    
    if (iter != impl->vertex_buffers.end ())
      return iter->second.buffer;
      
    if (impl->cache_state)
    {
      CacheVertexBufferMap::iterator iter = impl->vertex_buffers_cache.find (vb.SourceId ());
      
      if (iter != impl->vertex_buffers_cache.end ())
      {
        if (updatable)
        {
          VertexBufferPtr& buffer = iter->second;

          if (vb.StreamsCount () != buffer->StreamsCount ())
            throw xtl::format_operation_exception ("", "Streams count differs in passed buffer and cached buffer");

          const LowLevelBufferPtr* stream_buffer = buffer->Streams ();

          for (unsigned int i = 0, count = buffer->StreamsCount (); i < count; i++, stream_buffer++)
          {
            const media::geometry::VertexStream& vs = vb.Stream (i);

            impl->vertex_streams.insert_pair (vs.SourceId (), VertexStreamHolder (vs, *stream_buffer, usage));
          }

          impl->vertex_buffers.insert_pair (vb.SourceId (), VertexBufferHolder (vb, buffer));
        }

        return iter->second;
      }
    }
      
    VertexBufferPtr buffer (new VertexBuffer (vb, *this, impl->device_manager, usage, updatable), false);
    
    if (impl->cache_state)
      impl->vertex_buffers_cache.insert_pair (vb.SourceId (), buffer);

    if (updatable)
      impl->vertex_buffers.insert_pair (vb.SourceId (), VertexBufferHolder (vb, buffer));

    return buffer;      
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::CreateVertexBuffer");
    throw;
  }
}

LowLevelBufferPtr PrimitiveBuffersImpl::CreateIndexBuffer (const media::geometry::IndexBuffer& ib, MeshBufferUsage usage, bool updatable)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (ib.SourceId ());

    if (iter != impl->index_buffers.end ())
      return iter->second.buffer;      
      
    if (impl->cache_state)
    {
      CacheBufferMap::iterator iter = impl->index_buffers_cache.find (ib.SourceId ());
      
      if (iter != impl->index_buffers_cache.end ())
      {
        if (updatable)
          impl->index_buffers.insert_pair (ib.SourceId (), IndexBufferHolder (ib, iter->second, usage));

        return iter->second;
      }
    }
      
    BufferDesc desc;

    memset (&desc, 0, sizeof desc);

    desc.size         = ib.Size () * get_index_type_size (ib.DataType ());
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
    
    if (impl->cache_state)
      impl->index_buffers_cache.insert_pair (ib.SourceId (), buffer);
        
    if (updatable)
      impl->index_buffers.insert_pair (ib.SourceId (), IndexBufferHolder (ib, buffer, usage));

    return buffer;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::CreateIndexBuffer");
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
    VertexStreamMap::iterator iter = impl->vertex_streams.find (vs.SourceId ());
    
    if (iter != impl->vertex_streams.end ())
      return;    
    
    LowLevelBufferPtr buffer = CreateVertexStream (vs, usage, false);
    
    impl->vertex_streams.insert_pair (vs.SourceId (), VertexStreamHolder (vs, buffer, usage));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Add(const VertexStream&,MeshBufferUsage)");
    throw;
  }
}

void PrimitiveBuffersImpl::Add (const media::geometry::VertexBuffer& vb, MeshBufferUsage usage)
{
  try
  {
    VertexBufferMap::iterator iter = impl->vertex_buffers.find (vb.SourceId ());
    
    if (iter != impl->vertex_buffers.end ())
      return;    
    
    VertexBufferPtr buffer = CreateVertexBuffer (vb, usage, false);
    
    impl->vertex_buffers.insert_pair (vb.SourceId (), VertexBufferHolder (vb, buffer));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Add(const VertexBuffer&,MeshBufferUsage)");
    throw;
  }
}

void PrimitiveBuffersImpl::Add (const media::geometry::IndexBuffer& ib, MeshBufferUsage usage)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (ib.SourceId ());
    
    if (iter != impl->index_buffers.end ())
      return;    
    
    LowLevelBufferPtr buffer = CreateIndexBuffer (ib, usage, false);
    
    impl->index_buffers.insert_pair (ib.SourceId (), IndexBufferHolder (ib, buffer, usage));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Add(const IndexBuffer&,MeshBufferUsage)");
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
    VertexStreamMap::iterator iter = impl->vertex_streams.find (vs.SourceId ());
    
    if (iter == impl->vertex_streams.end ())
      throw xtl::make_argument_exception ("", "buffer.SourceId()", (size_t)vs.SourceId (), "Vertex stream not added to primitive buffers");
      
    VertexStreamHolder& holder = iter->second;
    
    if (holder.vertex_size != vs.VertexSize ())
      throw xtl::format_operation_exception ("", "Vertex size %u differs from source vertex stream size %u", vs.VertexSize (), holder.vertex_size);
      
    if (holder.vertices_count != vs.Size ())
      throw xtl::format_operation_exception ("", "Vertices count %u differs from source vertices count %u", vs.Size (), holder.vertices_count);
          
    holder.buffer->SetData (0, vs.Size () * vs.VertexSize (), vs.Data ());    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Update(const VertexStream&)");
    throw;
  }
}

void PrimitiveBuffersImpl::UpdateVertexStream (media::geometry::object_id_t id, const void* buffer, size_t buffer_size)
{
  try
  {
    VertexStreamMap::iterator iter = impl->vertex_streams.find (id);

    if (iter == impl->vertex_streams.end ())
      throw xtl::make_argument_exception ("", "id", (size_t)id, "Vertex stream not added to primitive buffers");

    VertexStreamHolder& holder = iter->second;

    if (holder.vertex_size * holder.vertices_count != buffer_size)
      throw xtl::format_operation_exception ("", "Buffer size differs from source vertex stream size");

    holder.buffer->SetData (0, buffer_size, buffer);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::UpdateVertexStream");
    throw;
  }
}

void PrimitiveBuffersImpl::Update (const media::geometry::IndexBuffer& ib)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (ib.SourceId ());
    
    if (iter == impl->index_buffers.end ())
      throw xtl::make_argument_exception ("", "buffer.SourceId()", (size_t)ib.SourceId (), "Index buffer not added to primitive buffers");
      
    IndexBufferHolder& holder = iter->second;
    
    if (holder.indices_count != ib.Size ())
      throw xtl::format_operation_exception ("", "Indices count %u differs from source indices count %u", ib.Size (), holder.indices_count);
          
    holder.buffer->SetData (0, ib.Size () * get_index_type_size (ib.DataType ()), ib.Data ());    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Update(const IndexBuffer&)");
    throw;
  }
}

void PrimitiveBuffersImpl::UpdateIndexBuffer (media::geometry::object_id_t id, const void* buffer, size_t buffer_size)
{
  try
  {
    IndexBufferMap::iterator iter = impl->index_buffers.find (id);

    if (iter == impl->index_buffers.end ())
      throw xtl::make_argument_exception ("", "id", (size_t)id, "Index buffer not added to primitive buffers");

    IndexBufferHolder& holder = iter->second;

    if (holder.indices_count * get_index_type_size (holder.source.DataType ()) != buffer_size)
      throw xtl::format_operation_exception ("", "Index buffer size differs from source index buffer size");

    holder.buffer->SetData (0, buffer_size, buffer);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::Update(const IndexBuffer&)");
    throw;
  }
}

/*
    Удаление буферов
*/

void PrimitiveBuffersImpl::Remove (const media::geometry::VertexStream& vs)
{
  impl->vertex_streams.erase (vs.SourceId ());
}

void PrimitiveBuffersImpl::Remove (const media::geometry::VertexBuffer& vb)
{
  impl->vertex_buffers.erase (vb.SourceId ());
}

void PrimitiveBuffersImpl::Remove (const media::geometry::IndexBuffer& ib)
{
  impl->index_buffers.erase (ib.SourceId ());
}

void PrimitiveBuffersImpl::RemoveAllVertexStreams ()
{
  impl->vertex_streams.clear ();
}

void PrimitiveBuffersImpl::RemoveAllVertexBuffers ()
{
  impl->vertex_buffers.clear ();
}

void PrimitiveBuffersImpl::RemoveAllIndexBuffers ()
{
  impl->index_buffers.clear ();
}

void PrimitiveBuffersImpl::RemoveAll ()
{
  impl->vertex_streams.clear ();
  impl->vertex_buffers.clear ();  
  impl->index_buffers.clear ();
}

/*
    Менеджер пакетирования
*/

render::manager::BatchingManager& PrimitiveBuffersImpl::BatchingManager ()
{
  try
  {
    if (impl->batching_manager)
      return *impl->batching_manager;

    impl->batching_manager.reset (new manager::BatchingManager (impl->device_manager));

    return *impl->batching_manager;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::PrimitiveBuffersImpl::BatchingManager");
    throw;
  }
}

bool PrimitiveBuffersImpl::HasBatchingManager ()
{
  return impl->batching_manager.get () != 0;
}

/*
    Управление кэшем
*/

void PrimitiveBuffersImpl::SetCacheState (bool state)
{
  if (state == impl->cache_state)
    return;
    
  if (impl->cache_state)
    FlushCache ();
    
  impl->cache_state = state;
}

bool PrimitiveBuffersImpl::CacheState ()
{
  return impl->cache_state;
}

void PrimitiveBuffersImpl::FlushCache ()
{
  impl->vertex_streams_cache.clear ();
  impl->vertex_buffers_cache.clear ();
  impl->index_buffers_cache.clear ();
}
