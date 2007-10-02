#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

const size_t DEFAULT_VERTEX_ARRAY_RESERVE = 4; //резервируемый размер вершинного массива

/*
    Описание реализации VertexBuffer
*/

typedef ResourceHolder<VertexStream>       VertexStreamHolder;
typedef ResourceHolder<VertexWeightStream> VertexWeightStreamHolder;
typedef stl::vector<VertexStreamHolder>    VertexStreamArray;

struct VertexBuffer::Impl: public xtl::reference_counter
{
  VertexStreamArray        streams;   //вершинные массивы
  VertexWeightStreamHolder weights;   //массив весов

  Impl ();
  Impl (const Impl&);
};

/*
    VertexBuffer::Impl
*/

VertexBuffer::Impl::Impl ()
{
  streams.reserve (DEFAULT_VERTEX_ARRAY_RESERVE);
}
  
VertexBuffer::Impl::Impl (const Impl& impl)
  : weights (impl.weights, ForceClone)
{
  streams.reserve (impl.streams.size ());

  for (VertexStreamArray::const_iterator i=impl.streams.begin (), end=impl.streams.end (); i!=end; ++i)
    streams.push_back (VertexStreamHolder (*i, ForceClone));
}

/*
    Конструкторы / деструктор
*/

VertexBuffer::VertexBuffer ()
  : impl (new Impl)
  {}

VertexBuffer::VertexBuffer (const VertexBuffer& vb, CloneMode mode)
  : impl (clone (vb.impl, mode, "media::geometry::VertexBuffer::VertexBuffer"))
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
    Идентификатор буфера
*/

size_t VertexBuffer::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    Количество массивов вершинных атрибутов
*/

size_t VertexBuffer::StreamsCount () const
{
  return impl->streams.size ();
}

/*
    Получение массива
*/

const VertexStream& VertexBuffer::Stream (size_t index) const
{
  if (index >= impl->streams.size ())
    RaiseOutOfRange ("media::geometry::VertexBuffer::Stream", "index", index, impl->streams.size ());
    
  return impl->streams [index].Resource ();
}

VertexStream& VertexBuffer::Stream (size_t index)
{
  return const_cast<VertexStream&> (const_cast<const VertexBuffer&> (*this).Stream (index));
}

/*
    Получение массива весов
*/

const VertexWeightStream& VertexBuffer::Weights () const
{
  return impl->weights.Resource ();
}

VertexWeightStream& VertexBuffer::Weights ()
{
  return impl->weights.Resource ();
}

/*
    Присоединение/отсоединение массивов
*/

size_t VertexBuffer::Attach (VertexStream& vs, CloneMode mode)
{
  impl->streams.push_back (VertexStreamHolder (vs, mode));

  return impl->streams.size () - 1;
}

void VertexBuffer::Detach (size_t index)
{
  if (index >= impl->streams.size ())
    return;
    
  impl->streams.erase (impl->streams.begin () + index);
}

void VertexBuffer::AttachWeights (VertexWeightStream& vws, CloneMode mode)
{
  impl->weights.Attach (vws, mode);
}

void VertexBuffer::DetachWeights ()
{
  impl->weights.Attach (VertexWeightStream (), CloneMode_Instance);
}

void VertexBuffer::Clear ()
{
  impl->streams.clear ();
  DetachWeights ();
}

/*
    Количество вершин (минимум среди всех вершинных массивов)
*/

size_t VertexBuffer::VerticesCount () const
{
  if (impl->streams.empty ())
    return 0;

  size_t vertices_count = size_t (-1);
  
  for (VertexStreamArray::iterator i=impl->streams.begin (), end=impl->streams.end (); i!=end; ++i)
  {
    size_t size = i->Resource ().Size ();
    
    if (size < vertices_count)
      vertices_count = size;
  }
  
  return vertices_count;
}

/*
    Суммарный размер вершины
*/

size_t VertexBuffer::VertexSize () const
{
  size_t vertex_size = 0;

  for (VertexStreamArray::iterator i=impl->streams.begin (), end=impl->streams.end (); i!=end; ++i)
    vertex_size += i->Resource ().VertexSize ();
    
  return vertex_size;
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

void swap (VertexBuffer& vb1, VertexBuffer& vb2)
{
  vb1.Swap (vb2);
}

}

}
