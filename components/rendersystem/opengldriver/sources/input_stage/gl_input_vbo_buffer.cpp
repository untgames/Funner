#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация - VboBuffer
///////////////////////////////////////////////////////////////////////////////////////////////////

///Конструктор / деструктор
VboBuffer::VboBuffer (const ContextManager&, GLenum target, const BufferDesc&)
{
}

VboBuffer::~VboBuffer ()
{
}

///Работа с данными буфера
void VboBuffer::SetData (size_t offset, size_t size, const void* data)
{
  common::RaiseNotImplemented("render::low_level::opengl::VboBuffer::SetData");
}

void VboBuffer::GetData (size_t offset, size_t size, void* data)
{
  common::RaiseNotImplemented("render::low_level::opengl::VboBuffer::GetData");
}

///Установка буфера в контекст OpenGL
void VboBuffer::Bind ()
{
  common::RaiseNotImplemented("render::low_level::opengl::VboBuffer::Bind");
}

///Cмещение от начала буфера (для аппаратных буферов)
void* VboBuffer::GetDataPointer ()
{
  common::RaiseNotImplemented("render::low_level::opengl::VboBuffer::GetDataPointer");
  return 0;
}
