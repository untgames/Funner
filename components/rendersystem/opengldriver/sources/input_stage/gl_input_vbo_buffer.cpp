#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

///Конструктор / деструктор
VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& desc)
  : Buffer(context_manager, desc), target (in_target)
{
  MakeContextCurrent();

  glGenBuffers(1, &buffer_id);          // зарезервируем номер буфера
  
  if (!buffer_id)
    return;
  
  glBindBuffer(target, buffer_id);      // сделаем его активным
  
  size_t buffermode;                    // переменная под режим буфера
  
  switch (buffer_desc.usage_mode)       // что поделаешь, под каждую пару доступ-режим - отдельный флаг
  {
    case UsageMode_Default:             // по дефолту делаем динамик, это перестраховка, а вообще 
      switch (buffer_desc.access_flags) // надо дефолт нафик убрать! потому как в реализации его нет
      {
        case AccessFlag_Read:
          buffermode = GL_DYNAMIC_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_DYNAMIC_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_DYNAMIC_COPY;
          break;
      }
      break;
    case UsageMode_Static:              // статический буфер
      switch (buffer_desc.access_flags)
      {
        case AccessFlag_Read:
          buffermode = GL_STATIC_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_STATIC_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_STATIC_COPY;
          break;
      }
      break;
    case UsageMode_Dynamic:             // динамический буфер
      switch (buffer_desc.access_flags)
      {
        case AccessFlag_Read:
          buffermode = GL_DYNAMIC_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_DYNAMIC_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_DYNAMIC_COPY;
          break;
      }
      break;
    case UsageMode_Stream:              // поточный буфер
      switch (buffer_desc.access_flags)
      {
        case AccessFlag_Read:
          buffermode = GL_STREAM_READ;
          break;
        case AccessFlag_Write:
          buffermode = GL_STREAM_DRAW;
          break;
        case AccessFlag_Read | AccessFlag_Write:
          buffermode = GL_STREAM_COPY;
          break;
      }
      break;  
  }
  // инициализируем буфер
  glBufferData(target, buffer_desc.size, NULL, buffermode);
  
  CheckErrors("render::low_level::opengl::VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& desc)");
}

VboBuffer::~VboBuffer ()
{
  MakeContextCurrent();
  glDeleteBuffers(1, &buffer_id);     // уничтожаем буфер

  CheckErrors("render::low_level::opengl::VboBuffer::~VboBuffer ()");
}

///Работа с данными буфера
void VboBuffer::SetData (size_t offset, size_t size, const void* data)
{
  Bind();
  
  if (offset >= buffer_desc.size)
    return;
  
  size = offset + size > buffer_desc.size ?   // размер массива
           buffer_desc.size - offset : size;

  glBufferSubData(target, offset, size, data);

  CheckErrors("render::low_level::opengl::VboBuffer::SetData (size_t offset, size_t size, const void* data)");
}

void VboBuffer::GetData (size_t offset, size_t size, void* data)
{
  Bind();

  if (offset >= buffer_desc.size)
    return;
  
  size = offset + size > buffer_desc.size ?   // размер массива
           buffer_desc.size - offset : size;

  glGetBufferSubData(target, offset, size, data);

  CheckErrors("render::low_level::opengl::VboBuffer::GetData (size_t offset, size_t size, void* data)");
}

///Установка буфера в контекст OpenGL
void VboBuffer::Bind ()
{
  MakeContextCurrent();
  glBindBuffer(target, buffer_id);

  CheckErrors("render::low_level::opengl::VboBuffer::Bind ()");
}

///Cмещение от начала буфера (для аппаратных буферов)
void* VboBuffer::GetDataPointer ()
{
  return 0; //возвращается смещение от начала буфера
}
