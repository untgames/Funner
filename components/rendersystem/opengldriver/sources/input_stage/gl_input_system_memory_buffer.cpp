#include "shared.h"
#include <memory.h>

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const BufferDesc& desc)
  : bufferdesc(desc)
{
  buffer = (void*)new char[bufferdesc.size]; // резервируем память
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
  delete[] buffer;                          // буффер уж не нужен
}
  
///Работа с данными буфера
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  if (offset < 0 || offset >= bufferdesc.size)      // проверяем смещение
    return;
  
  char* begin = (char*)buffer + offset;             // начало массива
  size        = offset + size > bufferdesc.size ?   // размер массива
                  bufferdesc.size - offset : size;
  // само копирование
  memcpy((void*) begin, data, size);
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  if (offset < 0 || offset >= bufferdesc.size)      // проверяем смещение
    return;

  char* begin = (char*)buffer + offset;
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
