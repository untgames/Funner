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

struct IndexBuffer::Impl: public xtl::reference_counter
{
  IndexType data_type;     //тип данных
  size_t    indices_count; //количество индексов
  Buffer    indices; //массив индексов
  
  Impl (IndexType in_data_type)
    : data_type (in_data_type)
    , indices_count (0)
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

IndexBuffer::IndexBuffer (size_t indices_count, IndexType data_type)
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

size_t IndexBuffer::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
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

size_t IndexBuffer::Size () const
{
  return impl->indices_count;
}

void IndexBuffer::Resize (size_t indices_count)
{
  impl->indices.Resize (indices_count * get_index_type_size (impl->data_type));
  
  impl->indices_count = indices_count;
}

/*
    Очистка буфера
*/

void IndexBuffer::Clear ()
{
  impl->indices.Resize (0);
  
  impl->indices_count = 0;
}

/*
    Резервирование памяти
*/

size_t IndexBuffer::Capacity () const
{
  return impl->indices.Capacity () / get_index_type_size (impl->data_type);
}

void IndexBuffer::Reserve (size_t indices_count)
{
  impl->indices.Reserve (indices_count * get_index_type_size (impl->data_type));
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
    
void swap (IndexBuffer& ib1, IndexBuffer& ib2)
{
  ib1.Swap (ib2);
}

const char* get_index_type_name (IndexType type)
{
  switch (type)
  {
    case IndexType_UInt32: return "uint32";
    case IndexType_UInt16: return "uint16";
    case IndexType_UInt8:  return "uint8";
    default:               throw xtl::make_argument_exception ("media::geometry::get_index_type_name", "type", type);
  }
}

size_t get_index_type_size (IndexType type)
{
  switch (type)
  {
    case IndexType_UInt32: return sizeof (unsigned int);
    case IndexType_UInt16: return sizeof (unsigned short);
    case IndexType_UInt8:  return sizeof (unsigned char);
    default:               throw xtl::make_argument_exception ("media::geometry::get_index_type_size", "type", type);
  }
}

}

}
