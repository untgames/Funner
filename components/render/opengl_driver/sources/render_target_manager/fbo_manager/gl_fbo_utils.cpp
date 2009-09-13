#include "shared.h"

using namespace common;

namespace render
{

namespace low_level
{

namespace opengl
{

//проверка состояния текущего буфера кадра
void check_frame_buffer_status (const char* source, GLenum status)
{
  switch (status)
  {
    case GL_FRAMEBUFFER_COMPLETE:
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
      throw xtl::format_operation_exception (source, "OpenGL bad framebuffer status: incomplete attachment");
    case GL_FRAMEBUFFER_UNSUPPORTED:
      throw xtl::format_operation_exception (source, "OpenGL bad framebuffer status: unsupported framebuffer format");
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
      throw xtl::format_operation_exception (source, "OpenGL bad framebuffer status: missing attachment");
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
      throw xtl::format_operation_exception (source, "OpenGL bad framebuffer status: attached images must have same dimensions");
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
      throw xtl::format_operation_exception (source, "OpenGL bad framebuffer status: attached images must have same format");
#ifndef OPENGL_ES_SUPPORT
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
      throw xtl::format_operation_exception (source, "OpenGL bad framebuffer status: missing draw buffer");
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
      throw xtl::format_operation_exception (source, "OpenGL bad framebuffer status: missing read buffer");
#endif
    default:
      throw xtl::format_operation_exception (source, "OpenGL bad framebuffer status: 0x%04x", status);
  }
}

}

}

}
