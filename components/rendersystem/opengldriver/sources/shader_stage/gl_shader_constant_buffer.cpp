#include "shared.h"
#include <memory.h>

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор
*/

ConstantBuffer::ConstantBuffer (const ContextManager& manager, const BufferDesc& in_desc)
  : ContextObject (manager), buffer (in_desc.size), desc (in_desc)
{
}

/*
   Получение дескриптора
*/

void ConstantBuffer::GetDesc (BufferDesc& target_desc)
{
  target_desc = desc;
}

/*
   Чтение / запись из буфера
*/

void ConstantBuffer::SetData (size_t offset, size_t size, const void* data)
{
   // проверяем смещение
  if (offset >= desc.size)
    return;

    // размер массива
  size = offset + size > desc.size ? desc.size - offset : size;
  // само копирование
  memcpy (buffer.data () + offset, data, size);
}

void ConstantBuffer::GetData (size_t offset, size_t size, void* data)
{
    // проверяем смещение
  if (offset >= desc.size)
    return;

   // размер массива
  size = offset + size > desc.size ? desc.size - offset : size;
  // копирование
  memcpy (data, buffer.data () + offset, size);
}
