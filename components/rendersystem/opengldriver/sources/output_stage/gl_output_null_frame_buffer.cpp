#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

NullFrameBuffer::NullFrameBuffer (const ContextManager& manager)
  : ContextObject (manager)
  {}

/*
    Установка буфера в контекст OpenGL
*/

void NullFrameBuffer::Bind (bool& color_buffer_state, bool& depth_stencil_buffer_state)
{
    //выбор текущего контекста

  MakeContextCurrent ();

    //установка буфера кадра по умолчанию

  static Extension EXT_framebuffer_object = "GL_EXT_framebuffer_object";

  if (IsSupported (EXT_framebuffer_object))
    glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

    //отключение буферов рисования и чтения

  glDrawBuffer (GL_NONE);
  glReadBuffer (GL_NONE);

  color_buffer_state         = false;
  depth_stencil_buffer_state = false;
}
