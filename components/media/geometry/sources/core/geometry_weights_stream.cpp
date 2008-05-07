#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    Описание реализации массива весов
*/

struct VertexWeightStream::Impl: public xtl::reference_counter
{
  Buffer data_buffer; //буфер данных
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

VertexWeightStream::VertexWeightStream (size_t weights_count)
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

size_t VertexWeightStream::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
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
  swap (impl, vws.impl);
}

namespace media
{

namespace geometry
{

void swap (VertexWeightStream& vws1, VertexWeightStream& vws2)
{
  vws1.Swap (vws2);
}

}

}
