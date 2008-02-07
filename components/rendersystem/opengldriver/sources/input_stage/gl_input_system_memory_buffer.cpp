#include "shared.h"
#include <memory.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const ContextManager& context_manager, const BufferDesc& desc)
  : Buffer(context_manager, desc), buffer (buffer_desc.size)
  {}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
}

///Работа с данными буфера
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
   // проверяем смещение
  if (offset >= buffer_desc.size)
    return;

    // размер массива
  size = offset + size > buffer_desc.size ? buffer_desc.size - offset : size;
  // само копирование
  memcpy (buffer.data () + offset, data, size);
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
    // проверяем смещение
  if (offset >= buffer_desc.size)
    return;

   // размер массива
  size = offset + size > buffer_desc.size ? buffer_desc.size - offset : size;
  // копирование
  memcpy (data, buffer.data () + offset, size);
}

///Установка буфера в контекст OpenGL
void SystemMemoryBuffer::Bind ()
{
  //установка не требуется
}

///Указатель на данные буфера
void* SystemMemoryBuffer::GetDataPointer ()
{
  return buffer.data ();  // возвращаем начало буфера
}
