#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

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
        throw xtl::make_argument_exception ("", "desc.usage_mode", in_desc.usage_mode);
        break;
    }

      //выбор текущего контекста

    MakeContextCurrent ();
    
      //получение информации о достпных расширениях
      
    PFNGLGENBUFFERSPROC glGenBuffers_fn = glGenBuffers ? glGenBuffers : glGenBuffersARB;
    PFNGLBINDBUFFERPROC glBindBuffer_fn = glBindBuffer ? glBindBuffer : glBindBufferARB;
    PFNGLBUFFERDATAPROC glBufferData_fn = glBufferData ? glBufferData : glBufferDataARB;

      //создание буфера
      
    glGenBuffers_fn (1, &buffer_id);

    if (!buffer_id)
      RaiseError ("");
      
    glBindBuffer_fn (target, buffer_id);    
    glBufferData_fn (target, in_desc.size, 0, gl_usage_mode);
    
    CheckErrors ("");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::VboBuffer::VboBuffer");
    throw;
  }
}

VboBuffer::~VboBuffer ()
{
  try
  {
    MakeContextCurrent ();
    
    PFNGLDELETEBUFFERSPROC glDeleteBuffers_fn = glDeleteBuffers ? glDeleteBuffers : glDeleteBuffersARB;
    
    glDeleteBuffers_fn (1, &buffer_id);
    CheckErrors        ("");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::VboBuffer::~VboBuffer");
    
    LogPrintf ("%s", exception.what ());
  }
  catch (std::exception& exception)
  {
    LogPrintf ("%s", exception.what ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Установка / чтение данных после отсечения
*/

void VboBuffer::SetDataCore (size_t offset, size_t size, const void* data)
{
  Bind ();
  
  PFNGLBUFFERSUBDATAPROC glBufferSubData_fn = glBufferSubData ? glBufferSubData : glBufferSubDataARB;
  
  glBufferSubData_fn (target, offset, size, data);

  CheckErrors ("render::low_level::opengl::VboBuffer::SetDataCore");
}

void VboBuffer::GetDataCore (size_t offset, size_t size, void* data)
{
  Bind ();
  
  PFNGLGETBUFFERSUBDATAPROC glGetBufferSubData_fn = glGetBufferSubData ? glGetBufferSubData : glGetBufferSubDataARB;  

  glGetBufferSubData_fn (target, offset, size, data);

  CheckErrors ("render::low_level::opengl::VboBuffer::GetDataCore");
}

/*
    Установка буфера в контекст OpenGL
*/

void VboBuffer::Bind ()
{
  size_t& current_id = GetContextDataTable (Stage_Input)[InputStageCache_BindedVboBuffer];

  if (current_id == GetId ())
    return;

  MakeContextCurrent ();
  
  const ContextCaps& caps = GetCaps ();    

  caps.glBindBuffer_fn (target, buffer_id);

  CheckErrors ("render::low_level::opengl::VboBuffer::Bind");

    //установка кэш-переменной

  current_id = GetId ();
}

/*
    Получение указателя на начало буфера
*/

void* VboBuffer::GetDataPointer ()
{
  return 0; //возвращается смещение от начала буфера
}
