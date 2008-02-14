#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

//check for ARB or version 1.5!!!!!!

/*
    Конструктор / деструктор
*/

VboBuffer::VboBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& in_desc)
  : Buffer (context_manager, in_desc), target (in_target), buffer_id (0)
{
  try
  {
      //определение режима использования буфера

    GLenum gl_usage_mode;    

    switch (in_desc.usage_mode)
    {
      case UsageMode_Static:
        gl_usage_mode = GL_STATIC_DRAW;
        break;
      case UsageMode_Default:
      case UsageMode_Dynamic:
        gl_usage_mode = GL_DYNAMIC_DRAW;
        break;
      case UsageMode_Stream:
        gl_usage_mode = GL_STREAM_DRAW;
        break;  
      default:
        RaiseInvalidArgument ("", "desc.usage_mode", in_desc.usage_mode);
        break;
    }

      //выбор текущего контекста

    MakeContextCurrent ();

      //создание буфера
      
        //ARB !ARB!!!!

    glGenBuffers (1, &buffer_id);

    if (!buffer_id)
      RaiseError ("");
      
    glBindBuffer (target, buffer_id);    
    glBufferData (target, in_desc.size, 0, gl_usage_mode);
    
    CheckErrors ("");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::VboBuffer::VboBuffer");
    throw;
  }
}

VboBuffer::~VboBuffer ()
{
    //проверка на исключения!!!!

  MakeContextCurrent();
  glDeleteBuffers(1, &buffer_id);     // уничтожаем буфер

  CheckErrors("render::low_level::opengl::VboBuffer::~VboBuffer ()");
}

/*
    Установка / чтение данных после отсечения
*/

void VboBuffer::SetDataCore (size_t offset, size_t size, const void* data)
{
  Bind ();
  
  glBufferSubData (target, offset, size, data);

  CheckErrors ("render::low_level::opengl::VboBuffer::SetData");
}

void VboBuffer::GetDataCore (size_t offset, size_t size, void* data)
{
  Bind ();

  glGetBufferSubData (target, offset, size, data);

  CheckErrors ("render::low_level::opengl::VboBuffer::GetData");
}

/*
    Установка буфера в контекст OpenGL
*/

void VboBuffer::Bind ()
{
  MakeContextCurrent ();
  
  glBindBuffer (target, buffer_id);

  CheckErrors ("render::low_level::opengl::VboBuffer::Bind");
}

/*
    Получение указателя на начало буфера
*/

void* VboBuffer::GetDataPointer ()
{
  return 0; //возвращается смещение от начала буфера
}
