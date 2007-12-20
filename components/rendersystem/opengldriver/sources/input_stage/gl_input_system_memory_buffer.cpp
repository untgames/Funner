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
  BufferDesc d;
  GetDesc(d);
  buffer = (void*)new char[d.size];
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
  delete buffer;
}

///Работа с данными буфера
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  BufferDesc bd;
  GetDesc(bd);
  
  if (offset < 0 || offset >= bd.size)
    return;
    
  char* begin = (char*)buffer + offset;
  char* end   = bd.size > offset + size ? (char*)begin + size : (char*)buffer + bd.size;
  char* ptr   = (char*)data;
  
  for (; begin < end; begin++, ptr++)
    *begin = *ptr;
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  BufferDesc bd;
  GetDesc(bd);

  if (offset < 0 || offset >= bd.size)
    return;

  char* begin = (char*)buffer + offset;
  char* end   = (char*)buffer + offset + size;
  char* ptr   = (char*)data;
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
  return buffer;
}
