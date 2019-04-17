#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

const size_t DEFAULT_VERTEX_ARRAY_RESERVE = 4; //резервируемый размер вершинного массива

/*
    Описание реализации VertexBuffer
*/

typedef stl::vector<VertexStream> VertexStreamArray;

struct VertexBuffer::Impl: public xtl::reference_counter, public xtl::trackable
{
  object_id_t        id;                     //идентификатор буфера
  object_id_t        source_id;              //идентификатор буфера из которого был клонирован / десериализован данный буфер
  VertexStreamArray  streams;                //вершинные массивы
  VertexWeightStream weights;                //массив весов
  unsigned int       structure_update_index; //индекс обновления структуры буфера (вершинные потоки или веса изменились)

  Impl ();
  Impl (const Impl&); //used for clone
};

/*
    VertexBuffer::Impl
*/

VertexBuffer::Impl::Impl ()
  : id (IdPool::AllocateId (ObjectType_VertexBuffer))
  , source_id (id)
  , structure_update_index (0)
{
  streams.reserve (DEFAULT_VERTEX_ARRAY_RESERVE);
}
  
VertexBuffer::Impl::Impl (const Impl& impl)
  : id (IdPool::AllocateId (ObjectType_VertexBuffer))
  , source_id (impl.source_id)
  , weights (impl.weights.Clone ())
  , structure_update_index (0)
{
  streams.reserve (impl.streams.size ());

  for (VertexStreamArray::const_iterator iter = impl.streams.begin (), end = impl.streams.end (); iter != end; ++iter)
    streams.push_back (iter->Clone ());
}

/*
    Конструкторы / деструктор
*/

VertexBuffer::VertexBuffer ()
  : impl (new Impl, false)
  {}

VertexBuffer::VertexBuffer (Impl* in_impl)
  : impl (in_impl, false)
  {}  

VertexBuffer::VertexBuffer (const VertexBuffer& vb)
  : impl (vb.impl)
  {}

VertexBuffer::~VertexBuffer ()
{
}

/*
    Присваивание
*/

VertexBuffer& VertexBuffer::operator = (const VertexBuffer& vb)
{
  impl = vb.impl;

  return *this;
}

/*
    Создание копии
*/

VertexBuffer VertexBuffer::Clone () const
{
  return VertexBuffer (new Impl (*impl));
}

/*
    Идентификатор буфера
*/

object_id_t VertexBuffer::Id () const
{
  return impl->id;
}

/*
   Идентификатор буфера из которого был клонирован / десериализован данный буфер
*/

object_id_t VertexBuffer::SourceId () const
{
  return impl->source_id;
}

/*
    Количество массивов вершинных атрибутов
*/

uint32_t VertexBuffer::StreamsCount () const
{
  return (uint32_t)impl->streams.size ();
}

/*
    Получение массива
*/

const VertexStream& VertexBuffer::Stream (uint32_t index) const
{
  if (index >= impl->streams.size ())
    throw xtl::make_range_exception ("media::geometry::VertexBuffer::Stream", "index", index, impl->streams.size ());
    
  return impl->streams [index];
}

VertexStream& VertexBuffer::Stream (uint32_t index)
{
  return const_cast<VertexStream&> (const_cast<const VertexBuffer&> (*this).Stream (index));
}

/*
    Получение массива весов
*/

const VertexWeightStream& VertexBuffer::Weights () const
{
  return impl->weights;
}

VertexWeightStream& VertexBuffer::Weights ()
{
  return impl->weights;
}

/*
    Присоединение/отсоединение массивов
*/

uint32_t VertexBuffer::Attach (VertexStream& vs)
{
    //проверка зарегистрирован ли канал
    
  object_id_t id = vs.Id ();
    
  for (VertexStreamArray::iterator i=impl->streams.begin (), end=impl->streams.end (); i!=end; ++i)
    if (i->Id () == id)
      return (uint32_t)(i - impl->streams.begin ());

  if (impl->streams.size () == (uint32_t)-1)
    throw xtl::format_operation_exception ("media::geometry::VertexBuffer::Attach", "Streams count exceeded max limit");

  impl->streams.push_back (vs);

  impl->structure_update_index++;

  return (uint32_t)impl->streams.size () - 1;
}

void VertexBuffer::Detach (uint32_t index)
{
  if (index >= impl->streams.size ())
    return;
    
  impl->streams.erase (impl->streams.begin () + index);

  impl->structure_update_index++;
}

void VertexBuffer::AttachWeights (VertexWeightStream& vws)
{
  impl->weights = vws;

  impl->structure_update_index++;
}

void VertexBuffer::DetachWeights ()
{
  impl->weights = VertexWeightStream ();

  impl->structure_update_index++;
}

void VertexBuffer::Clear ()
{
  impl->streams.clear ();

  DetachWeights ();
}

/*
    Количество вершин (минимум среди всех вершинных массивов)
*/

uint32_t VertexBuffer::VerticesCount () const
{
  if (impl->streams.empty ())
    return 0;

  uint32_t vertices_count = uint32_t (-1);
  
  for (VertexStreamArray::iterator i=impl->streams.begin (), end=impl->streams.end (); i!=end; ++i)
  {
    uint32_t size = i->Size ();
    
    if (size < vertices_count)
      vertices_count = size;
  }
  
  return vertices_count;
}

/*
    Суммарный размер вершины
*/

uint32_t VertexBuffer::VertexSize () const
{
  uint32_t vertex_size = 0;

  for (VertexStreamArray::iterator i=impl->streams.begin (), end=impl->streams.end (); i!=end; ++i)
    vertex_size += i->VertexSize ();

  return vertex_size;
}

/*
   Текущий индекс обновления
*/

unsigned int VertexBuffer::CurrentStructureUpdateIndex () const
{
  return impl->structure_update_index;
}

/*
   Сериализация / десериализация
*/

size_t VertexBuffer::SerializationSize () const
{
  uint32_t vertex_streams_size = 0;

  for (VertexStreamArray::iterator iter = impl->streams.begin (), end = impl->streams.end (); iter != end; ++iter)
    vertex_streams_size += iter->SerializationSize ();

  return sizeof (impl->source_id) + sizeof (uint32_t) + vertex_streams_size + impl->weights.SerializationSize ();
}

size_t VertexBuffer::Write (void* buffer, size_t buffer_size) const
{
  static const char* METHOD_NAME = "media::geometry::VertexBuffer::Write";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_written = 0;

  if (sizeof (impl->source_id) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing source id");

  memcpy (buffer, &impl->source_id, sizeof (impl->source_id));

  bytes_written += sizeof (impl->source_id);

  uint32_t streams_count = impl->streams.size ();

  if (sizeof (streams_count) + bytes_written > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for writing streams count");

  memcpy ((char*)buffer + bytes_written, &streams_count, sizeof (streams_count));

  bytes_written += sizeof (streams_count);

  for (VertexStreamArray::iterator iter = impl->streams.begin (), end = impl->streams.end (); iter != end; ++iter)
    bytes_written += iter->Write ((char*)buffer + bytes_written, buffer_size - bytes_written);

  return bytes_written + impl->weights.Write ((char*)buffer + bytes_written, buffer_size - bytes_written);
}

size_t VertexBuffer::Read (void* buffer, size_t buffer_size)
{
  size_t bytes_read = 0;

  VertexBuffer::CreateFromSerializedData (buffer, buffer_size, bytes_read).Swap (*this);

  return bytes_read;
}

VertexBuffer VertexBuffer::CreateFromSerializedData (void* buffer, size_t buffer_size, size_t& out_bytes_read)
{
  static const char* METHOD_NAME = "media::geometry::VertexBuffer::CreateFromSerializedData";

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  size_t bytes_read = 0;

  object_id_t source_id;

  if (sizeof (source_id) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading source id");

  memcpy (&source_id, buffer, sizeof (source_id));

  bytes_read += sizeof (source_id);

  uint32_t streams_count;

  if (sizeof (streams_count) + bytes_read > buffer_size)
    throw xtl::make_argument_exception (METHOD_NAME, "buffer_size", buffer_size, "Not enough size for reading streams count");

  memcpy (&streams_count, (char*)buffer + bytes_read, sizeof (streams_count));

  bytes_read += sizeof (streams_count);

  VertexBuffer new_vb;

  new_vb.impl->streams.reserve (streams_count);

  for (uint32_t i = 0; i < streams_count; i++)
  {
    size_t vs_read;

    new_vb.impl->streams.push_back (VertexStream::CreateFromSerializedData ((char*)buffer + bytes_read, buffer_size - bytes_read, vs_read));

    bytes_read += vs_read;
  }

  bytes_read += new_vb.impl->weights.Read ((char*)buffer + bytes_read, buffer_size - bytes_read);

  new_vb.impl->source_id = source_id;

  out_bytes_read = bytes_read;

  return new_vb;
}

/*
   Объект оповещения об удалении
*/

xtl::trackable& VertexBuffer::Trackable () const
{
  return *impl;
}

/*
    Обмен
*/

void VertexBuffer::Swap (VertexBuffer& vb)
{
  swap (vb.impl, impl);
}

namespace media
{

namespace geometry
{

xtl::trackable& get_trackable (const VertexBuffer& vb)
{
  return vb.Trackable ();
}

void swap (VertexBuffer& vb1, VertexBuffer& vb2)
{
  vb1.Swap (vb2);
}

}

}
