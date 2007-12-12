#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///Конструктор / деструктор
VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& desc)
  : Buffer (desc), ContextObject (context_manager), target (in_target)
{
}

VboBuffer::~VboBuffer ()
{
}

///Работа с данными буфера
void VboBuffer::SetData (size_t offset, size_t size, const void* data)
{
  RaiseNotImplemented ("render::low_level::opengl::VboBuffer::SetData");
}

void VboBuffer::GetData (size_t offset, size_t size, void* data)
{
  RaiseNotImplemented ("render::low_level::opengl::VboBuffer::GetData");
}

///Установка буфера в контекст OpenGL
void VboBuffer::Bind ()
{
  RaiseNotImplemented ("render::low_level::opengl::VboBuffer::Bind");
}

///Cмещение от начала буфера (для аппаратных буферов)
void* VboBuffer::GetDataPointer ()
{
  return 0; //возвращается смещение от начала буфера
}
