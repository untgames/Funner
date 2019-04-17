#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    Описание реализации массива весов
*/

struct VertexWeightStream::Impl: public xtl::reference_counter, public xtl::trackable
{
  object_id_t  id;                     //идентификатор потока
  object_id_t  source_id;              //идентификатор потока из которого был клонирован / десериализован данный поток
  Buffer       data_buffer;            //буфер данных
  unsigned int structure_update_index; //текущий индекс обновления структуры потока (изменение размера)
  unsigned int data_update_index;      //текущий индекс обновления данных потока

  Impl ()
    : id (IdPool::AllocateId (ObjectType_VertexWeightStream))
    , source_id (id)
    , structure_update_index (0)
    , data_update_index (0)
    {}

  Impl (const Impl& source)
    : id (IdPool::AllocateId (ObjectType_VertexWeightStream))
    , source_id (source.source_id)
    , data_buffer (source.data_buffer)
    , structure_update_index (0)
    , data_update_index (0)
    {}
};

/*
    Конструкторы / деструктор
*/

VertexWeightStream::VertexWeightStream ()
  : impl (new Impl, false)
  {}

VertexWeightStream::VertexWeightStream (Impl* in_impl)
  : impl (in_impl, false)
  {}  

VertexWeightStream::VertexWeightStream (uint32_t weights_count)
  : impl (new Impl, false)
{
  Resize (weights_count);
}
  
VertexWeightStream::VertexWeightStream (const VertexWeightStream& vws)
  : impl (vws.impl)
  {}

VertexWeightStream::~VertexWeightStream ()
{
}

/*
    Присваивание
*/

VertexWeightStream& VertexWeightStream::operator = (const VertexWeightStream& vws)
{
  impl = vws.impl;

  return *this;
}

/*
    Создание копии
*/

VertexWeightStream VertexWeightStream::Clone () const
{
  return VertexWeightStream (new Impl (*impl));  
}

/*
    Идентификатор потока
*/

object_id_t VertexWeightStream::Id () const
{
  return impl->id;
}

/*
   Идентификатор потока из которого был клонирован / десериализован данный поток
*/

object_id_t VertexWeightStream::SourceId () const
{
  return impl->source_id;
}

/*
    Получение данных
*/

const VertexWeight* VertexWeightStream::Data () const
{
  return (const VertexWeight*)impl->data_buffer.Data ();
}

VertexWeight* VertexWeightStream::Data ()
{
  return (VertexWeight*)impl->data_buffer.Data ();
}

/*
    Количество вершин
*/

uint32_t VertexWeightStream::Size () const
{
  return impl->data_buffer.Size () / sizeof (VertexWeight);
}

void VertexWeightStream::Resize (uint32_t weights_count)
{
  impl->data_buffer.Resize (weights_count * sizeof (VertexWeight));

  impl->structure_update_index++;
}

/*
    Очистка массива
*/

void VertexWeightStream::Clear ()
{
  impl->data_buffer.Resize (0);

  impl->structure_update_index++;
}

/*
    Резервирование памяти
*/

uint32_t VertexWeightStream::Capacity () const
{
  return impl->data_buffer.Capacity () / sizeof (VertexWeight);
}

void VertexWeightStream::Reserve (uint32_t weights_count)
{
  impl->data_buffer.Reserve (weights_count * sizeof (VertexWeight));
}

/*
   Увеличение индекса обновления / Текущий индекс обновления
*/

void VertexWeightStream::InvalidateData ()
{
  impl->data_update_index++;
}

unsigned int VertexWeightStream::CurrentStructureUpdateIndex () const
{
  return impl->structure_update_index;
}

unsigned int VertexWeightStream::CurrentDataUpdateIndex () const
{
  return impl->data_update_index;
}

/*
   Сериализация / десериализация
*/

size_t VertexWeightStream::SerializationSize () const
{
  return sizeof (impl->source_id) + sizeof (uint32_t) + SerializationDataSize ();
}

size_t VertexWeightStream::SerializationDataSize () const
{
  return impl->data_buffer.Size ();
}

size_t VertexWeightStream::Write (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::VertexWeightStream::Write";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (buffer_size < SerializationSize ())
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing data");

  size_t bytes_written = 0;

  memcpy (buffer, &impl->source_id, sizeof (impl->source_id));

  bytes_written += sizeof (impl->source_id);

  uint32_t data_size = impl->data_buffer.Size ();

  memcpy ((char*)buffer + bytes_written, &data_size, sizeof (data_size));

  bytes_written += sizeof (data_size);

  return bytes_written + WriteData ((char*)buffer + bytes_written, buffer_size - bytes_written);
}

size_t VertexWeightStream::WriteData (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::VertexWeightStream::WriteData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t data_size = SerializationDataSize ();

  if (buffer_size < data_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing data");

  memcpy (buffer, impl->data_buffer.Data (), data_size);

  return data_size;
}

size_t VertexWeightStream::Read (void* buffer, size_t buffer_size)
{
  size_t bytes_read;

  VertexWeightStream::CreateFromSerializedData (buffer, buffer_size, bytes_read).Swap (*this);

  return bytes_read;
}

size_t VertexWeightStream::ReadData (void* buffer, size_t buffer_size)
{
  static const char* METHOD_NAME = "media::geometry::VertexWeightStream::ReadData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t data_size = SerializationDataSize ();

  if (buffer_size < data_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading data");

  memcpy (impl->data_buffer.Data (), buffer, data_size);

  InvalidateData ();

  return data_size;
}

VertexWeightStream VertexWeightStream::CreateFromSerializedData (void* buffer, size_t buffer_size, size_t& out_bytes_read)
{
  static const char* METHOD_NAME = "media::geometry::VertexWeightStream::CreateFromSerializedData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_read = 0;

  object_id_t source_id;

  if (sizeof (source_id) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading source id");

  memcpy (&source_id, buffer, sizeof (source_id));

  bytes_read += sizeof (source_id);

  uint32_t data_size;

  if (sizeof (data_size) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading data size");

  memcpy (&data_size, (char*)buffer + bytes_read, sizeof (data_size));

  bytes_read += sizeof (data_size);

  VertexWeightStream new_stream;

  new_stream.impl->data_buffer.Resize (data_size);

  bytes_read += new_stream.ReadData ((char*)buffer + bytes_read, buffer_size - bytes_read);

  new_stream.impl->source_id = source_id;

  out_bytes_read = bytes_read;

  return new_stream;
}

/*
   Объект оповещения об удалении
*/

xtl::trackable& VertexWeightStream::Trackable () const
{
  return *impl;
}


/*
    Обмен
*/

void VertexWeightStream::Swap (VertexWeightStream& vws)
{
  swap (impl, vws.impl);
}

namespace media
{

namespace geometry
{

xtl::trackable& get_trackable (const VertexWeightStream& vws)
{
  return vws.Trackable ();
}

void swap (VertexWeightStream& vws1, VertexWeightStream& vws2)
{
  vws1.Swap (vws2);
}

}

}
