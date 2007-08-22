#include "shared.h"

using namespace medialib::geometry;
using namespace common;

/*
    Описание реализации массива весов
*/

struct VertexWeightStream::Impl: public InstanceResource
{
  MeshBuffer data_buffer; //буфер данных
};

/*
    Конструкторы / деструктор
*/

VertexWeightStream::VertexWeightStream ()
  : impl (new Impl)
  {}

VertexWeightStream::VertexWeightStream (size_t weights_count)
  : impl (new Impl)
{
  Resize (weights_count);
}
  
VertexWeightStream::VertexWeightStream (const VertexWeightStream& vws, BufferCloneMode mode)
  : impl (clone_resource (vws.impl, mode, "medialib::VertexWeightStream::VertexWeightStream"))
  {}

VertexWeightStream::~VertexWeightStream ()
{
  release_resource (impl);
}

/*
    Присваивание
*/

void VertexWeightStream::Assign (const VertexWeightStream& vws, BufferCloneMode mode)
{
  VertexWeightStream (vws, mode).Swap (*this);
}

VertexWeightStream& VertexWeightStream::operator = (const VertexWeightStream& vsw)
{
  Assign (vsw);

  return *this;
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

size_t VertexWeightStream::Size () const
{
  return impl->data_buffer.Size () / sizeof (VertexWeight);
}

void VertexWeightStream::Resize (size_t weights_count)
{
  impl->data_buffer.Resize (weights_count * sizeof (VertexWeight));
}

/*
    Очистка массива
*/

void VertexWeightStream::Clear ()
{
  impl->data_buffer.Resize (0);
}

/*
    Резервирование памяти
*/

size_t VertexWeightStream::Capacity () const
{
  return impl->data_buffer.Capacity () / sizeof (VertexWeight);
}

void VertexWeightStream::Reserve (size_t weights_count)
{
  impl->data_buffer.Reserve (weights_count * sizeof (VertexWeight));
}

/*
    Обмен
*/

void VertexWeightStream::Swap (VertexWeightStream& vws)
{
  Impl* tmp = impl;
  impl      = vws.impl;
  vws.impl  = tmp;
}

namespace medialib
{

namespace geometry
{

void swap (VertexWeightStream& vws1, VertexWeightStream& vws2)
{
  vws1.Swap (vws2);
}

}

}
