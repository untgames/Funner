#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация - SystemMemoryBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

SystemMemoryBuffer::SystemMemoryBuffer (const BufferDesc&)
{
  buffer = NULL;
}

SystemMemoryBuffer::~SystemMemoryBuffer ()
{
}

///Работа с данными буфера
void SystemMemoryBuffer::SetData (size_t offset, size_t size, const void* data)
{
  common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::SetData");
}

void SystemMemoryBuffer::GetData (size_t offset, size_t size, void* data)
{
  common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::GetData");
}

///Установка буфера в контекст OpenGL
void SystemMemoryBuffer::Bind ()
{
  common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::Bind");
}

///Указатель на данные буфера
void* GetDataPointer ()
{
  common::RaiseNotImplemented("render::low_level::opengl::SystemMemoryBuffer::GetDataPointer");
  return buffer;
}
