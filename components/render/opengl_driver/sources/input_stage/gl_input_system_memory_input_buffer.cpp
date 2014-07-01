#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

SystemMemoryInputBuffer::SystemMemoryInputBuffer (const ContextManager& context_manager, GLenum in_target, const BufferDesc& in_desc)
  : SystemMemoryBuffer (context_manager, in_desc),
    target (in_target)
  {}

/*
    Установка буфера в контекст OpenGL
*/

void SystemMemoryInputBuffer::Bind ()
{
  const size_t current_id = GetContextCacheValue (CacheEntry_BindedVboBuffer);

  if (current_id == GetId ())
    return;

  MakeContextCurrent ();

  const ContextCaps& caps = GetCaps ();

  if (caps.has_arb_vertex_buffer_object)
  {
    caps.glBindBuffer_fn (target, 0);

    CheckErrors ("render::low_level::opengl::SystemMemoryInputBuffer::Bind");
  }

    //установка кэш-переменной

  SetContextCacheValue (CacheEntry_BindedVboBuffer, 0);
}

/*
    Установка данных после отсечения
*/

void SystemMemoryInputBuffer::SetDataCore (size_t offset, size_t size, const void* data)
{
  SystemMemoryBuffer::SetDataCore (offset, size, data);

  StageRebindNotify (Stage_Input);
}

#endif
