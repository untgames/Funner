#include "shared.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
///Абстрактный класс Buffer
///////////////////////////////////////////////////////////////////////////////////////////////////

///Конструктор / деструктор
Buffer::Buffer (const BufferDesc&)
{
}

///Получение дескриптора
void Buffer::GetDesc (BufferDesc&)
{
  common::RaiseNotImplemented("render::low_level::opengl::Buffer::GetDesc");
}

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
