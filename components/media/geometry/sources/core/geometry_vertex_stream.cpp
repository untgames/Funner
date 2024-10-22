#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    Описание реализации вершинного массива
*/

struct VertexStream::Impl: public xtl::reference_counter, public xtl::trackable
{
  object_id_t  id;                     //идентификатор потока
  object_id_t  source_id;              //идентификатор потока из которого был клонирован / десериализован данный поток
  VertexFormat format;                 //формат вершин
  uint32_t     vertex_size;            //размер вершины
  Buffer       data_buffer;            //буфер с данными
  uint32_t     vertices_count;         //количество вершин
  unsigned int structure_update_index; //индекс обновления структуры потока (изменение размер)
  unsigned int data_update_index;      //индекс обновления данных потока
  
  Impl ();
  Impl (const Impl& impl);           //used for clone
  Impl (const VertexDeclaration&);
};

/*
    VertexStream::Impl
*/

VertexStream::Impl::Impl ()
  : id (IdPool::AllocateId (ObjectType_VertexStream))
  , source_id (id)
  , vertex_size (0)
  , vertices_count (0)
  , structure_update_index (0)
  , data_update_index (0)
  {}

VertexStream::Impl::Impl (const Impl& source)
  : id (IdPool::AllocateId (ObjectType_VertexStream))
  , source_id (source.source_id)
  , format (source.format.Clone ())
  , vertex_size (source.vertex_size)
  , data_buffer (source.data_buffer)
  , vertices_count (source.vertices_count)
  , structure_update_index (0)
  , data_update_index (0)
  {}

VertexStream::Impl::Impl (const VertexDeclaration& declaration)
  : id (IdPool::AllocateId (ObjectType_VertexStream))
  , source_id (id)
  , format (declaration.Format ())
  , vertex_size (declaration.VertexSize ())
  , vertices_count (0)
  , structure_update_index (0)
  , data_update_index (0)
  {}  

/*
    VertexStream
*/

/*
    Конструкторы / деструктор
*/

VertexStream::VertexStream (Impl* in_impl)
  : impl (in_impl, false)
  {}
  
VertexStream::VertexStream (const VertexDeclaration& declaration)
  : impl (new Impl (declaration), false)
  {}

VertexStream::VertexStream (uint32_t vertices_count, const VertexDeclaration& declaration)
  : impl (new Impl (declaration), false)
{
  Resize (vertices_count);
}

VertexStream::VertexStream (const VertexStream& source, const VertexDeclaration& declaration)
  : impl (new Impl (declaration), false)
{
  Convert (source);
}

VertexStream::VertexStream (const VertexBuffer& src_vb, const VertexDeclaration& declaration)
  : impl (new Impl (declaration), false)
{
  Convert (src_vb);
}

VertexStream::VertexStream (const VertexBuffer& src_vb)
  : impl (new Impl, false)
{
  VertexFormat& dst_format = impl->format;
  uint32_t      offset = 0;

  for (uint32_t i=0, count=src_vb.StreamsCount (); i<count; i++)
  {
    const VertexFormat& src_format = src_vb.Stream (i).Format ();
    
    for (uint32_t j=0, count=src_format.AttributesCount (); j<count; j++)
    {
      const VertexAttribute& attribute = src_format.Attribute (j);
      
      if (!dst_format.FindAttribute (attribute.semantic))
      {
        dst_format.AddAttribute (attribute.semantic, attribute.type, offset);
        
        offset += get_type_size (attribute.type);
      }
    }
  }

  impl->vertex_size = offset;

  Convert (src_vb);
}

VertexStream::VertexStream (const VertexStream& vs)
  : impl (vs.impl)
  {}

VertexStream::~VertexStream ()
{
}

/*
    Присваивание
*/

VertexStream& VertexStream::operator = (const VertexStream& vs)
{
  impl = vs.impl;
  return *this;
}

/*
    Создание копии
*/

VertexStream VertexStream::Clone () const
{
  return VertexStream (new Impl (*impl));
}

/*
    Идентификатор потока
*/

object_id_t VertexStream::Id () const
{
  return impl->id;
}

/*
   Идентификатор потока из которого был клонирован / десериализован данный поток
*/

object_id_t VertexStream::SourceId () const
{
  return impl->source_id;
}

/*
    Формат
*/

const VertexFormat& VertexStream::Format () const
{
  return impl->format;
}

/*
    Размер вершины
*/

uint32_t VertexStream::VertexSize () const
{
  return impl->vertex_size;
}

/*
    Получение данных
*/

const void* VertexStream::Data () const
{
  return impl->data_buffer.Data ();
}

void* VertexStream::Data ()
{
  return impl->data_buffer.Data ();
}

/*
    Количество вершин
*/

uint32_t VertexStream::Size () const
{
  return impl->vertices_count;
}

void VertexStream::Resize (uint32_t vertices_count)
{
  impl->data_buffer.Resize (vertices_count * impl->vertex_size);
  
  impl->vertices_count = vertices_count;

  impl->structure_update_index++;
}

/*
    Очистка массива
*/

void VertexStream::Clear ()
{
  Resize (0);
}

/*
    Резервирование памяти
*/

uint32_t VertexStream::Capacity () const
{
  return impl->vertex_size ? impl->data_buffer.Capacity () / impl->vertex_size : 0;
}

void VertexStream::Reserve (uint32_t vertices_count)
{
  impl->data_buffer.Reserve (vertices_count * impl->vertex_size);
}

/*
   Увеличение индекса обновления / Текущий индекс обновления
*/

void VertexStream::InvalidateData ()
{
  impl->data_update_index++;
}

unsigned int VertexStream::CurrentStructureUpdateIndex () const
{
  return impl->structure_update_index;
}

unsigned int VertexStream::CurrentDataUpdateIndex () const
{
  return impl->data_update_index;
}

/*
   Сериализация / десериализация
*/

size_t VertexStream::SerializationSize () const
{
  return sizeof (impl->source_id) + impl->format.SerializationSize () + sizeof (impl->vertex_size) + sizeof (impl->vertices_count) + SerializationDataSize ();
}

size_t VertexStream::SerializationDataSize () const
{
  return impl->data_buffer.Size ();
}

size_t VertexStream::Write (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::VertexStream::Write";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_written = 0;

  if (sizeof (impl->source_id) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing source id");

  memcpy (buffer, &impl->source_id, sizeof (impl->source_id));

  bytes_written += sizeof (impl->source_id);

  bytes_written += impl->format.Write ((char*)buffer + bytes_written, buffer_size - bytes_written);

  if (sizeof (impl->vertex_size) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing vertex size");

  memcpy ((char*)buffer + bytes_written, &impl->vertex_size, sizeof (impl->vertex_size));

  bytes_written += sizeof (impl->vertex_size);

  if (sizeof (impl->vertices_count) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing vertices count");

  memcpy ((char*)buffer + bytes_written, &impl->vertices_count, sizeof (impl->vertices_count));

  bytes_written += sizeof (impl->vertices_count);

  return bytes_written + WriteData ((char*)buffer + bytes_written, buffer_size - bytes_written);
}

size_t VertexStream::WriteData (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::VertexStream::WriteData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t data_size = SerializationDataSize ();

  if (buffer_size < data_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing data");

  memcpy (buffer, impl->data_buffer.Data (), data_size);

  return data_size;
}

size_t VertexStream::Read (const void* buffer, size_t buffer_size)
{
  size_t bytes_read = 0;

  VertexStream::CreateFromSerializedData (buffer, buffer_size, bytes_read).Swap (*this);

  return bytes_read;
}

size_t VertexStream::ReadData (const void* buffer, size_t buffer_size)
{
  static const char* METHOD_NAME = "media::geometry::VertexStream::ReadData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t data_size = SerializationDataSize ();

  if (buffer_size < data_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading data");

  memcpy (impl->data_buffer.Data (), buffer, data_size);

  InvalidateData ();

  return data_size;
}

VertexStream VertexStream::CreateFromSerializedData (const void* buffer, size_t buffer_size, size_t& out_bytes_read)
{
  static const char* METHOD_NAME = "media::geometry::VertexStream::CreateFromSerializedData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_read = 0;

  object_id_t source_id;

  if (sizeof (source_id) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading source id");

  memcpy (&source_id, buffer, sizeof (source_id));

  bytes_read += sizeof (source_id);

  VertexFormat vertex_format;

  bytes_read += vertex_format.Read ((char*)buffer + bytes_read, buffer_size - bytes_read);

  uint32_t vertex_size;

  if (sizeof (vertex_size) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading vertex size");

  memcpy (&vertex_size, (char*)buffer + bytes_read, sizeof (vertex_size));

  bytes_read += sizeof (vertex_size);

  uint32_t vertices_count;

  if (sizeof (vertices_count) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading vertices count");

  memcpy (&vertices_count, (char*)buffer + bytes_read, sizeof (vertices_count));

  bytes_read += sizeof (vertices_count);

  VertexStream new_vs (vertices_count, VertexDeclaration (vertex_format, vertex_size));

  bytes_read += new_vs.ReadData ((char*)buffer + bytes_read, buffer_size - bytes_read);

  new_vs.impl->source_id = source_id;

  out_bytes_read = bytes_read;

  return new_vs;
}

/*
   Объект оповещения об удалении
*/

xtl::trackable& VertexStream::Trackable () const
{
  return *impl;
}

/*
    Обмен
*/

void VertexStream::Swap (VertexStream& vs)
{
  swap (impl, vs.impl);
}

namespace media
{

namespace geometry
{

xtl::trackable& get_trackable (const VertexStream& vs)
{
  return vs.Trackable ();
}

void swap (VertexStream& vs1, VertexStream& vs2)
{
  vs1.Swap (vs2);
}

}
    
}

/*
    Преобразование вершинных массивов
*/

void VertexStream::Convert (const VertexStream& src_stream)
{
  if (!impl->format.AttributesCount ())
  {
    Clear ();
    return;
  }
  
    //определение количества вершин
    
  uint32_t vertices_count = src_stream.Size ();
  
    //имезенение размера массива вершин
  
  Resize (vertices_count);
  
    //очистка массива
    
  memset (impl->data_buffer.Data (), 0, impl->data_buffer.Size ());
  
    //копирование данных из вершинного буфера
  
  for (uint32_t i=0, count=impl->format.AttributesCount (); i<count; i++)
  {
    const VertexAttribute&    dst_attribute = impl->format.Attribute (i);
          void*               dst_ptr       = static_cast<char*> (impl->data_buffer.Data ()) + dst_attribute.offset;
    const VertexAttributeType dst_type      = dst_attribute.type;
    const size_t              dst_stride    = impl->vertex_size;
    const VertexAttribute*    src_attribute = src_stream.Format ().FindAttribute (dst_attribute.semantic);
      
    if (!src_attribute)
      continue;
        
    const void*               src_ptr    = static_cast<const char*> (src_stream.Data ()) + src_attribute->offset;
    const size_t              src_stride = src_stream.VertexSize ();
    const VertexAttributeType src_type   = src_attribute->type;
    
    copy (vertices_count, src_ptr, src_type, src_stride, dst_ptr, dst_type, dst_stride);
  }
}

void VertexStream::Convert (const VertexBuffer& vb)
{
  if (!impl->format.AttributesCount ())
  {
    Clear ();
    return;
  }
  
    //определение количества вершин
    
  uint32_t vertices_count = vb.VerticesCount ();
  
    //имезенение размера массива вершин
  
  Resize (vertices_count);
  
    //очистка массива
    
  memset (impl->data_buffer.Data (), 0, impl->data_buffer.Size ());
  
    //копирование данных из вершинного буфера
  
  for (uint32_t i=0, count=impl->format.AttributesCount (); i<count; i++)
  {
    const VertexAttribute&    dst_attribute = impl->format.Attribute (i);
          void*               dst_ptr       = static_cast<char*> (impl->data_buffer.Data ()) + dst_attribute.offset;
    const VertexAttributeType dst_type      = dst_attribute.type;
    const size_t              dst_stride    = impl->vertex_size;

    for (uint32_t j=0, count=vb.StreamsCount (); j<count; j++)
    {
      const VertexStream&    src_stream    = vb.Stream (j);
      const VertexAttribute* src_attribute = src_stream.Format ().FindAttribute (dst_attribute.semantic);
      
      if (!src_attribute)
        continue;
        
      const void*               src_ptr    = static_cast<const char*> (src_stream.Data ()) + src_attribute->offset;
      const size_t              src_stride = src_stream.VertexSize ();
      const VertexAttributeType src_type   = src_attribute->type;
      
      copy (vertices_count, src_ptr, src_type, src_stride, dst_ptr, dst_type, dst_stride);
    }
  }
}
