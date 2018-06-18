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
  : structure_update_index (0)
{
  streams.reserve (DEFAULT_VERTEX_ARRAY_RESERVE);
}
  
VertexBuffer::Impl::Impl (const Impl& impl)
  : weights (impl.weights.Clone ())
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

size_t VertexBuffer::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
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
    
  size_t id = vs.Id ();
    
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
