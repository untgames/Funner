#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const BufferDesc& desc)
  : Buffer(desc)
{
  BufferDesc d;                     // путь смы и проинитили дескриптор,
  GetDesc(d);                       // вытаскивать его приходится вот так извратно
  buffer = (void*)new char[d.size]; // резервируем память
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
  delete buffer;                    // буффер уж не нужен
}

///Работа с данными буфера
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  BufferDesc bd;                        // опять тащить дескриптор
  GetDesc(bd);
  
  if (offset < 0 || offset >= bd.size)  // проверяем смещение
    return;
  
  // комплект указателей для копирования данных
  char* begin = (char*)buffer + offset; 
  char* end   = bd.size > offset + size ? (char*)begin + size : (char*)buffer + bd.size;
  char* ptr   = (char*)data;
  // само копирование
  for (; begin < end; begin++, ptr++)
    *begin = *ptr;
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  BufferDesc bd;                        // и снова дескриптор
  GetDesc(bd);

  if (offset < 0 || offset >= bd.size)  // смещение
    return;

  // указатели
  char* begin = (char*)buffer + offset;
  char* end   = (char*)buffer + offset + size;
  char* ptr   = (char*)data;
  // копирование
  for (; begin < end; begin ++, ptr++)
    *ptr = *begin;
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
