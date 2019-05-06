#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    Утилиты
*/

namespace
{

template <class SrcT, class DstT> void convert (const SrcT* src, DstT* dst, size_t count, IndexType src_type, IndexType dst_type)
{
  for (size_t i=0; i<count; i++, src++, dst++)
  {
    if (DstT (*src) != *src)
      throw xtl::format_operation_exception ("media::geometry::convert", "Can't change index buffer type from %s to %s. Too large index [%u]=%u", get_index_type_name (src_type),
        get_index_type_name (dst_type), i, *src);
        
    *dst = static_cast<DstT> (*src);
  }
}

template <class SrcT> void convert_dispatch (const SrcT* src, void* dst, size_t count, IndexType src_type, IndexType dst_type)
{
  switch (dst_type)
  {
    case IndexType_UInt32:
      convert (src, static_cast<unsigned int*> (dst), count, src_type, dst_type);
      break;
    case IndexType_UInt16:
      convert (src, static_cast<unsigned short*> (dst), count, src_type, dst_type);
      break;      
    case IndexType_UInt8:
      convert (src, static_cast<unsigned char*> (dst), count, src_type, dst_type);
      break;
    default:
      throw xtl::make_argument_exception ("media::geometry::convert_dispatch", "dst_type", dst_type);
  }
}

}

/*
    Описание реализации индексного буфера
*/

struct IndexBuffer::Impl: public xtl::reference_counter, public xtl::trackable
{
  object_id_t  id;                     //идентификатор буфера
  object_id_t  source_id;              //идентификатор буфера из которого был клонирован / десериализован данный буфер
  IndexType    data_type;              //тип данных
  uint32_t     indices_count;          //количество индексов
  Buffer       indices;                //массив индексов
  unsigned int structure_update_index; //индекс обновления структуры буфера (тип или размер)
  unsigned int data_update_index;      //индекс обновления данных буфера
  
  Impl (IndexType in_data_type)
    : id (IdPool::AllocateId (ObjectType_IndexBuffer))
    , source_id (id)
    , data_type (in_data_type)
    , indices_count (0)
    , structure_update_index (0)
    , data_update_index (0)
  {
    switch (data_type)
    {
      case IndexType_UInt32:
      case IndexType_UInt16:
      case IndexType_UInt8:
        break;
      default:
        throw xtl::make_argument_exception ("media::geometry::IndexBuffer::Impl::Impl", "data_type", data_type);
    }
  }

  Impl (const Impl& source)
    : id (IdPool::AllocateId (ObjectType_IndexBuffer))
    , source_id (source.source_id)
    , data_type (source.data_type)
    , indices_count (source.indices_count)
    , indices (source.indices)
    , structure_update_index (0)
    , data_update_index (0)
    {}
};

/*
    Конструкторы / деструктор
*/

IndexBuffer::IndexBuffer ()
  : impl (new Impl (IndexType_Default), false)
  {}
  
IndexBuffer::IndexBuffer (Impl* in_impl)
  : impl (in_impl, false)
  {}

IndexBuffer::IndexBuffer (uint32_t indices_count, IndexType data_type)
  : impl (new Impl (data_type), false)
{
  Resize (indices_count);
}
  
IndexBuffer::IndexBuffer (const IndexBuffer& ib)
  : impl (ib.impl)
  {}
  
IndexBuffer::IndexBuffer (const IndexBuffer& ib, IndexType new_type)
  : impl (new Impl (*ib.impl), false)
{
  SetDataType (new_type);
}  

IndexBuffer::~IndexBuffer ()
{
}

/*
    Присваивание
*/

IndexBuffer& IndexBuffer::operator = (const IndexBuffer& ib)
{
  impl = ib.impl;

  return *this;
}

/*
    Создание копии
*/

IndexBuffer IndexBuffer::Clone () const
{
  return IndexBuffer (new Impl (*impl));
}

/*
    Идентификатор буфера
*/

object_id_t IndexBuffer::Id () const
{
  return impl->id;
}

/*
   Идентификатор буфера из которого был клонирован / десериализован данный буфер
*/

object_id_t IndexBuffer::SourceId () const
{
  return impl->source_id;
}

/*
    Тип данных
*/

IndexType IndexBuffer::DataType () const
{
  return impl->data_type;
}

void IndexBuffer::SetDataType (IndexType type)
{
  if (type == impl->data_type)
    return;
    
  try
  {
    Buffer new_indices (get_index_type_size (type) * impl->indices_count);

    switch (impl->data_type)
    {
      case IndexType_UInt32:
        convert_dispatch (static_cast<const unsigned int*> (impl->indices.Data ()), new_indices.Data (), impl->indices_count, impl->data_type, type);
        break;
      case IndexType_UInt16:
        convert_dispatch (static_cast<const unsigned short*> (impl->indices.Data ()), new_indices.Data (), impl->indices_count, impl->data_type, type);
        break;    
      case IndexType_UInt8:
        convert_dispatch (static_cast<const unsigned char*> (impl->indices.Data ()), new_indices.Data (), impl->indices_count, impl->data_type, type);
        break;
      default:
        throw xtl::make_argument_exception ("", "type", type);
    }

    new_indices.Swap (impl->indices);

    impl->data_type = type;

    impl->structure_update_index++;
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::geometry::IndexBuffer::SetDataType");
    throw;
  }
}

/*
    Данные
*/

const void* IndexBuffer::Data () const
{
  return impl->indices.Data ();
}

void* IndexBuffer::Data ()
{
  return impl->indices.Data ();
}

/*
    Количество индексов
*/

uint32_t IndexBuffer::Size () const
{
  return impl->indices_count;
}

void IndexBuffer::Resize (uint32_t indices_count)
{
  impl->indices.Resize (indices_count * get_index_type_size (impl->data_type));
  
  impl->indices_count = indices_count;

  impl->structure_update_index++;
}

/*
    Очистка буфера
*/

void IndexBuffer::Clear ()
{
  impl->indices.Resize (0);
  
  impl->indices_count = 0;

  impl->structure_update_index++;
}

/*
    Резервирование памяти
*/

uint32_t IndexBuffer::Capacity () const
{
  return impl->indices.Capacity () / get_index_type_size (impl->data_type);
}

void IndexBuffer::Reserve (uint32_t indices_count)
{
  impl->indices.Reserve (indices_count * get_index_type_size (impl->data_type));
}

/*
   Увеличение индекса обновления / Текущий индекс обновления
*/

void IndexBuffer::InvalidateData ()
{
  impl->data_update_index++;
}

unsigned int IndexBuffer::CurrentStructureUpdateIndex () const
{
  return impl->structure_update_index;
}

unsigned int IndexBuffer::CurrentDataUpdateIndex () const
{
  return impl->data_update_index;
}

/*
   Сериализация / десериализация
*/

size_t IndexBuffer::SerializationSize () const
{
  return sizeof (impl->source_id) + sizeof (impl->data_type) + sizeof (impl->indices_count) + SerializationDataSize ();
}

size_t IndexBuffer::SerializationDataSize () const
{
  return impl->indices.Size ();
}

size_t IndexBuffer::Write (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::IndexBuffer::Write";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (buffer_size < SerializationSize ())
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing data");

  size_t bytes_written = 0;

  memcpy (buffer, &impl->source_id, sizeof (impl->source_id));

  bytes_written += sizeof (impl->source_id);

  memcpy ((char*)buffer + bytes_written, &impl->data_type, sizeof (impl->data_type));

  bytes_written += sizeof (impl->data_type);

  memcpy ((char*)buffer + bytes_written, &impl->indices_count, sizeof (impl->indices_count));

  bytes_written += sizeof (impl->indices_count);

  return bytes_written + WriteData ((char*)buffer + bytes_written, buffer_size - bytes_written);
}

size_t IndexBuffer::WriteData (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::IndexBuffer::WriteData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t data_size = SerializationDataSize ();

  if (buffer_size < data_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing data");

  memcpy (buffer, impl->indices.Data (), data_size);

  return data_size;
}

size_t IndexBuffer::Read (const void* buffer, size_t buffer_size)
{
  size_t bytes_read = 0;

  CreateFromSerializedData (buffer, buffer_size, bytes_read).Swap (*this);

  return bytes_read;
}

size_t IndexBuffer::ReadData (const void* buffer, size_t buffer_size)
{
  static const char* METHOD_NAME = "media::geometry::IndexBuffer::ReadData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t data_size = SerializationDataSize ();

  if (buffer_size < data_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading data");

  memcpy (impl->indices.Data (), buffer, data_size);

  InvalidateData ();

  return data_size;
}

IndexBuffer IndexBuffer::CreateFromSerializedData (const void* buffer, size_t buffer_size, size_t& out_bytes_read)
{
  static const char* METHOD_NAME = "media::geometry::IndexBuffer::CreateFromSerializedData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_read = 0;

  object_id_t source_id;

  if (sizeof (source_id) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading source id");

  memcpy (&source_id, buffer, sizeof (source_id));

  bytes_read += sizeof (source_id);

  IndexType data_type;

  if (sizeof (data_type) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading data type");

  memcpy (&data_type, (char*)buffer + bytes_read, sizeof (data_type));

  bytes_read += sizeof (data_type);

  uint32_t indices_count;

  if (sizeof (indices_count) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading indices count");

  memcpy (&indices_count, (char*)buffer + bytes_read, sizeof (indices_count));

  bytes_read += sizeof (indices_count);

  IndexBuffer new_ib (indices_count, data_type);

  bytes_read += new_ib.ReadData ((char*)buffer + bytes_read, buffer_size - bytes_read);

  new_ib.impl->source_id = source_id;

  out_bytes_read = bytes_read;

  return new_ib;
}

/*
   Объект оповещения об удалении
*/

xtl::trackable& IndexBuffer::Trackable () const
{
  return *impl;
}


/*
    Обмен
*/

void IndexBuffer::Swap (IndexBuffer& ib)
{
  impl.swap (ib.impl);
}

namespace media
{

namespace geometry
{

xtl::trackable& get_trackable (const IndexBuffer& ib)
{
  return ib.Trackable ();
}

void swap (IndexBuffer& ib1, IndexBuffer& ib2)
{
  ib1.Swap (ib2);
}

}

}
