#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    Описание реализации вершинного массива
*/

struct VertexStream::Impl: public xtl::reference_counter
{
  VertexFormat format;         //формат вершин
  size_t       vertex_size;    //размер вершины
  Buffer       data_buffer;    //буфер с данными
  size_t       vertices_count; //количество вершин
  
  Impl ();
  Impl (const VertexDeclaration&);
};

/*
    VertexStream::Impl
*/

VertexStream::Impl::Impl (const VertexDeclaration& declaration)
  : format (declaration.Format ()), vertex_size (declaration.VertexSize ()), vertices_count (0)
  {}
  
VertexStream::Impl::Impl ()
  : vertex_size (0), vertices_count (0)
  {}

/*
    VertexStream
*/

/*
    Конструкторы / деструктор
*/
  
VertexStream::VertexStream (const VertexDeclaration& declaration)
  : impl (new Impl (declaration), false)
  {}

VertexStream::VertexStream (size_t vertices_count, const VertexDeclaration& declaration)
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
  size_t       offset = 0;

  for (size_t i=0, count=src_vb.StreamsCount (); i<count; i++)
  {
    const VertexFormat& src_format = src_vb.Stream (i).Format ();
    
    for (size_t j=0, count=src_format.AttributesCount (); j<count; j++)
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

VertexStream::VertexStream (const VertexStream& vs, CloneMode mode)
  : impl (clone (vs.impl, mode, "media::geometry::VertexStream::VertexStream"))
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
    Идентификатор потока
*/

size_t VertexStream::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
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

size_t VertexStream::VertexSize () const
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

size_t VertexStream::Size () const
{
  return impl->vertices_count;
}

void VertexStream::Resize (size_t vertices_count)
{
  impl->data_buffer.Resize (vertices_count * impl->vertex_size);
  
  impl->vertices_count = vertices_count;
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

size_t VertexStream::Capacity () const
{
  return impl->vertex_size ? impl->data_buffer.Capacity () / impl->vertex_size : 0;
}

void VertexStream::Reserve (size_t vertices_count)
{
  impl->data_buffer.Reserve (vertices_count * impl->vertex_size);
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
    
  size_t vertices_count = src_stream.Size ();
  
    //имезенение размера массива вершин
  
  Resize (vertices_count);
  
    //очистка массива
    
  memset (impl->data_buffer.Data (), 0, impl->data_buffer.Size ());
  
    //копирование данных из вершинного буфера
  
  for (size_t i=0, count=impl->format.AttributesCount (); i<count; i++)
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
    
  size_t vertices_count = vb.VerticesCount ();
  
    //имезенение размера массива вершин
  
  Resize (vertices_count);
  
    //очистка массива
    
  memset (impl->data_buffer.Data (), 0, impl->data_buffer.Size ());
  
    //копирование данных из вершинного буфера
  
  for (size_t i=0, count=impl->format.AttributesCount (); i<count; i++)
  {
    const VertexAttribute&    dst_attribute = impl->format.Attribute (i);
          void*               dst_ptr       = static_cast<char*> (impl->data_buffer.Data ()) + dst_attribute.offset;
    const VertexAttributeType dst_type      = dst_attribute.type;
    const size_t              dst_stride    = impl->vertex_size;

    for (size_t j=0, count=vb.StreamsCount (); j<count; j++)
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
