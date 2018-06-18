#include "shared.h"

using namespace media::geometry;
using namespace media;
using namespace common;

/*
    Описание реализации массива весов
*/

struct VertexWeightStream::Impl: public xtl::reference_counter, public xtl::trackable
{
  Buffer       data_buffer;            //буфер данных
  unsigned int structure_update_index; //текущий индекс обновления структуры потока (изменение размера)
  unsigned int data_update_index;      //текущий индекс обновления данных потока

  Impl ()
    : structure_update_index (0)
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
