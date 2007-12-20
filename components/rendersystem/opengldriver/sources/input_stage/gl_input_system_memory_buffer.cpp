#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const BufferDesc& desc)
{
  bufferdesc = desc;
  buffer = new void[bufferdesc.size];
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
  delete buffer;
}

///Работа с данными буфера
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  if (offset < 0 || offset >= bufferdesc.size)
    return;
    
  void* begin = buffer + offset;
  void* end   = bufferdesc.size > offset + size ? begin + size : buffer + bufferdesc.size;
  void* ptr   = data;
  
  for (; begin < end; begin++, ptr++)
    *begin = *ptr;
    
  //common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::SetData");
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  if (offset < 0 || offset >= bufferdesc.size)
    return;

  void* begin = buffer + offset;
  void* end   = buffer + offset + size;
  void* ptr   = data;
  for (; begin < end; begin ++, ptr++)
    *ptr = *begin;
  //common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::GetData");
}

///Установка буфера в контекст OpenGL
void SystemMemoryBuffer::Bind ()
{
  //установка не требуется
}

///Указатель на данные буфера
void* SystemMemoryBuffer::GetDataPointer ()
{
  //common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::GetDataPointer");
  return buffer;
}
