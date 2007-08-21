#include <stl/vector>
#include "shared.h"

using namespace medialib;
using namespace common;

const size_t DEFAULT_VERTEX_ARRAY_RESERVE = 4; //резервируемый размер вершинного массива

/*
    Обёртка для хранения вершинных массивов
*/

class VertexStreamWrapper: public VertexStream
{
  public:
    VertexStreamWrapper (const VertexStreamWrapper& vs) : VertexStream (vs, BufferCloneMode_Instance), clone_mode (vs.clone_mode) {}
    VertexStreamWrapper (const VertexStream& vs, BufferCloneMode mode) : VertexStream (vs, mode), clone_mode (mode) {}

    VertexStreamWrapper& operator = (const VertexStream& vs)
    {
      Assign (vs, BufferCloneMode_Instance);
      return *this;
    }

    BufferCloneMode clone_mode; //режим копирования
};

/*
    Описание реализации VertexBuffer
*/

typedef stl::vector<VertexStreamWrapper> VertexStreamArray;

struct VertexBuffer::Impl: public InstanceResource
{
  VertexStreamArray  streams;   //вершинные массивы
  VertexWeightStream weights;   //массив весов
  
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
  : weights (impl.weights)
{
  streams.reserve (impl.streams.size ());

  for (VertexStreamArray::const_iterator i=impl.streams.begin (), end=impl.streams.end (); i!=end; ++i)
    streams.push_back (VertexStreamWrapper (*i, i->clone_mode));
}

/*
    Конструкторы / деструктор
*/

VertexBuffer::VertexBuffer ()
  : impl (new Impl)
  {}

VertexBuffer::VertexBuffer (const VertexBuffer& vb, BufferCloneMode mode)
  : impl (clone_resource (vb.impl, mode, "medialib::VertexBuffer::VertexBuffer"))
  {}

VertexBuffer::~VertexBuffer ()
{
  release_resource (impl);
}

/*
    Присваивание
*/

void VertexBuffer::Assign (const VertexBuffer& vb, BufferCloneMode mode)
{
  VertexBuffer (vb, mode).Swap (*this);
}

VertexBuffer& VertexBuffer::operator = (const VertexBuffer& vb)
{
  Assign (vb);

  return *this;
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
    RaiseOutOfRange ("medialib::VertexBuffer::Stream", "index", index, impl->streams.size ());
    
  return impl->streams [index];
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
  return impl->weights;
}

VertexWeightStream& VertexBuffer::Weights ()
{
  return impl->weights;
}

/*
    Присоединение/отсоединение массивов
*/

size_t VertexBuffer::Attach (VertexStream& vs, BufferCloneMode mode)
{
  switch (mode)
  {
    case BufferCloneMode_Copy:
    case BufferCloneMode_Instance:
      break;
    default:
      RaiseInvalidArgument ("medialib::VertexBuffer::Attach", "mode", mode);
      break;
  }

  impl->streams.push_back (VertexStreamWrapper (vs, mode));

  return impl->streams.size () - 1;
}

void VertexBuffer::Detach (size_t index)
{
  if (index >= impl->streams.size ())
    return;
    
  impl->streams.erase (impl->streams.begin () + index);
}

void VertexBuffer::AttachWeights (VertexWeightStream& vws, BufferCloneMode mode)
{
  impl->weights.Assign (vws, mode);
}

void VertexBuffer::DetachWeights ()
{
  VertexWeightStream ().Swap (impl->weights);
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
    size_t size = i->Size ();
    
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
    vertex_size += i->VertexSize ();
    
  return vertex_size;
}
    
/*
    Обмен
*/

void VertexBuffer::Swap (VertexBuffer& vb)
{
  stl::swap (vb.impl, impl);
}

namespace medialib
{

void swap (VertexBuffer& vb1, VertexBuffer& vb2)
{
  vb1.Swap (vb2);
}

}
