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
#ifndef OPENGL_ES_SUPPORT      
        gl_usage_mode = GL_STREAM_DRAW;
        break;                
#else
        gl_usage_mode = GL_DYNAMIC_DRAW;
        break;
#endif
      default:
        throw xtl::make_argument_exception ("", "desc.usage_mode", in_desc.usage_mode);
    }

      //выбор текущего контекста

    MakeContextCurrent ();
    
      //получение информации о достпных расширениях
      
    const ContextCaps& caps = GetCaps ();
      
    PFNGLGENBUFFERSPROC glGenBuffers_fn = caps.glGenBuffers_fn;
    PFNGLBINDBUFFERPROC glBindBuffer_fn = caps.glBindBuffer_fn;
    PFNGLBUFFERDATAPROC glBufferData_fn = caps.glBufferData_fn;

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
    if (TryMakeContextCurrent ())
    {
      GetCaps ().glDeleteBuffers_fn (1, &buffer_id);

      CheckErrors ("");
    }
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
  
  GetCaps ().glBufferSubData_fn (target, offset, size, data);

  CheckErrors ("render::low_level::opengl::VboBuffer::SetDataCore");
}

void VboBuffer::GetDataCore (size_t offset, size_t size, void* data)
{
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

  Bind ();  

  GetCaps ().glGetBufferSubData_fn (target, offset, size, data);

  CheckErrors ("render::low_level::opengl::VboBuffer::GetDataCore");

#else
  throw xtl::format_not_supported_exception ("render::low_level::opengl::VboBuffer::GetDataCore", "glGetBufferSubData not supported");
#endif
}

/*
    Установка буфера в контекст OpenGL
*/

void VboBuffer::Bind ()
{
  const size_t current_id = GetContextCacheValue (CacheEntry_BindedVboBuffer);

  if (current_id == GetId ())
    return;

  MakeContextCurrent ();
  
  const ContextCaps& caps = GetCaps ();    

  caps.glBindBuffer_fn (target, buffer_id);

  CheckErrors ("render::low_level::opengl::VboBuffer::Bind");

    //установка кэш-переменной

  SetContextCacheValue (CacheEntry_BindedVboBuffer, GetId ());
}

/*
    Получение указателя на начало буфера
*/

void* VboBuffer::GetDataPointer ()
{
  return 0; //возвращается смещение от начала буфера
}
