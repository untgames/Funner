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
  /*
  BufferDesc bd;
  GetDesc(bd);
  
  if (offset < 0 || offset >= bd.size)
    return;
    
  void* begin = buffer + offset;
  void* end   = bd.size > offset + size ? begin + size : buffer + bd.size;
  void* ptr   = data;
  
  for (; begin < end; begin++, ptr++)
    *begin = *ptr;
  */
    
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  /*
  BufferDesc bd;
  GetDesc(bd);

  if (offset < 0 || offset >= bd.size)
    return;

  void* begin = buffer + offset;
  void* end   = buffer + offset + size;
  void* ptr   = data;
  for (; begin < end; begin ++, ptr++)
    *ptr = *begin;
  */
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
