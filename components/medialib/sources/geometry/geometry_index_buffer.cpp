#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    Описание реализации индексного буфера
*/

struct IndexBuffer::Impl: public SharedResource
{
  Buffer indices; //массив индексов
};

/*
    Конструкторы / деструктор
*/

IndexBuffer::IndexBuffer ()
  : impl (new Impl)
  {}

IndexBuffer::IndexBuffer (size_t indices_count)
  : impl (new Impl)
{
  try
  {
    Resize (indices_count);
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}
  
IndexBuffer::IndexBuffer (const IndexBuffer& ib, CloneMode mode)
  : impl (clone_resource (ib.impl, mode, "media::geometry::IndexBuffer::IndexBuffer"))
  {}

IndexBuffer::~IndexBuffer ()
{
  release_resource (impl);
}

/*
    Присваивание
*/

void IndexBuffer::Assign (const IndexBuffer& ib, CloneMode mode)
{
  IndexBuffer (ib, mode).Swap (*this);
}

IndexBuffer& IndexBuffer::operator = (const IndexBuffer& ib)
{
  Assign (ib);

  return *this;
}

/*
    Данные
*/

const size_t* IndexBuffer::Data () const
{
  return (const size_t*)impl->indices.Data ();
}

size_t* IndexBuffer::Data ()
{
  return (size_t*)impl->indices.Data ();
}

/*
    Количество индексов
*/

size_t IndexBuffer::Size () const
{
  return impl->indices.Size () / sizeof (size_t);
}

void IndexBuffer::Resize (size_t indices_count)
{
  impl->indices.Resize (indices_count * sizeof (size_t));
}

/*
    Очистка буфера
*/

void IndexBuffer::Clear ()
{
  impl->indices.Resize (0);
}

/*
    Резервирование памяти
*/

size_t IndexBuffer::Capacity () const
{
  return impl->indices.Capacity () / sizeof (size_t);
}

void IndexBuffer::Reserve (size_t indices_count)
{
  impl->indices.Reserve (indices_count * sizeof (size_t));
}

/*
    Обмен
*/

void IndexBuffer::Swap (IndexBuffer& ib)
{
  Impl* tmp = ib.impl;
  ib.impl   = impl;
  impl      = tmp;
}

namespace media
{

namespace geometry
{
    
void swap (IndexBuffer& ib1, IndexBuffer& ib2)
{
  ib1.Swap (ib2);
}

}

}
