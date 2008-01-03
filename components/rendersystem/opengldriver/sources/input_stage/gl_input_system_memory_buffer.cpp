#include "shared.h"
#include <memory.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const BufferDesc& desc)
  : buffer_desc(desc)
{
  buffer = (void*)new char[buffer_desc.size]; // резервируем память
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
  delete[] buffer;                          // буффер уже не нужен
}

void SystemMemoryBuffer::GetDesc(BufferDesc& desc)
{
  desc = buffer_desc;
}
  
///Работа с данными буфера
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  if (offset >= buffer_desc.size)      // проверяем смещение
    return;
  
  char* begin = (char*)buffer + offset;             // начало массива
  size        = offset + size > buffer_desc.size ?   // размер массива
                  buffer_desc.size - offset : size;
  // само копирование
  memcpy((void*) begin, data, size);
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  if (offset >= buffer_desc.size)      // проверяем смещение
    return;

  char* begin = (char*)buffer + offset;
  size        = offset + size > buffer_desc.size ?   // размер массива
                  buffer_desc.size - offset : size;
  // копирование
  memcpy(data, (void*) begin, size);
}

///Установка буфера в контекст OpenGL
void SystemMemoryBuffer::Bind ()
{
  //установка не требуется
}

///Указатель на данные буфера
void* SystemMemoryBuffer::GetDataPointer ()
{
  return buffer;  // возвращаем начало буфера
}
