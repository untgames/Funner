#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор
*/

SourceConstantBuffer::SourceConstantBuffer (const BufferDesc& in_desc, const void* data)
  : desc (in_desc)
  , hash ()
  , need_update_hash (true)
{
  try
  {
      //проверка корректности режима использования буфера

    switch (desc.usage_mode)
    {
      case UsageMode_Default:
      case UsageMode_Static:
      case UsageMode_Dynamic:
      case UsageMode_Stream:
        break;
      default:
        throw xtl::make_argument_exception ("", "desc.usage_mode", desc.usage_mode);
    }

      //проверка корректности флагов доступа

    switch (desc.access_flags)
    {
      case 0:
      case AccessFlag_Read:
      case AccessFlag_Write:
      case AccessFlag_Read | AccessFlag_Write:
        break;
      default:
        throw xtl::make_argument_exception ("", "desc.access_flags", desc.access_flags);
    }

    if (desc.bind_flags != BindFlag_ConstantBuffer)
      throw xtl::make_argument_exception ("", "desc.bind_flags", desc.bind_flags, "BindFlag_ConstantBuffer have to be used");

    if (!desc.size)
      throw xtl::make_null_argument_exception ("", "desc.size");

    buffer.resize (desc.size);

    if (data)
      memcpy (buffer.data (), data, desc.size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SourceConstantBuffer::SourceConstantBuffer");
    throw;
  }
}

/*
    Получение дескриптора буфера
*/

void SourceConstantBuffer::GetDesc (BufferDesc& out_desc)
{
  out_desc = desc;
}

/*
    Чтение / запись из буфера
*/

void SourceConstantBuffer::SetData (size_t offset, size_t size, const void* data, IDeviceContext* context)
{
  try
  {
    cast_object<DeviceObject> (context, "", "context");

      //проверка возможности установки данных
      
    if (!(desc.access_flags & AccessFlag_Write))
      throw xtl::format_operation_exception ("", "Can't set buffer data (no AccessFlag_Write in desc.access_flags)");

      //отсечение

    if (offset >= desc.size)
      return;
      
    if (desc.size - offset < size)
      size = desc.size - offset;
      
    if (!size)
      return;

      //проверка корректности переданных данных

    if (!data)
      throw xtl::make_null_argument_exception ("", "data");

      //установка данных

    memcpy (buffer.data () + offset, data, size);

    need_update_hash = true;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SourceConstantBuffer::SetData");
    throw;
  }
}

void SourceConstantBuffer::GetData (size_t offset, size_t size, void* data, IDeviceContext* context)
{
  try
  {
    cast_object<DeviceObject> (context, "", "context");

      //проверка возможности установки данных
      
    if (!(desc.access_flags & AccessFlag_Read))
      throw xtl::format_operation_exception ("", "Can't get buffer data (no AccessFlag_Read in desc.access_flags)");

      //отсечение

    if (offset >= desc.size)
      return;

    if (desc.size - offset < size)
      size = desc.size - offset;

    if (!size)
      return;

      //проверка корректности результирующего буфера

    if (!data)
      throw xtl::make_null_argument_exception ("", "data");

      //чтение данных

    memcpy (data, buffer.data () + offset, size);   
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::SourceConstantBuffer::GetData");
    throw;
  }
}

/*
    Хэш данных
*/

size_t SourceConstantBuffer::GetHash () const
{
  if (!need_update_hash)
    return hash;

  hash             = buffer.size () ? common::crc32 (buffer.data (), buffer.size ()) : 0;
  need_update_hash = false;

  return hash;
}
