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
    case GL_FRAMEBUFFER_COMPLETE_EXT:
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: incomplete attachment");
      break;
    case GL_FRAMEBUFFER_UNSUPPORTED_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: unsupported framebuffer format");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: missing attachment");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: attached images must have same dimensions");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: attached images must have same format");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: missing draw buffer");
      break;
    case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: missing read buffer");
      break;
    default:
      RaiseInvalidOperation (source, "OpenGL bad framebuffer status: 0x%04x", status);
      break;
  }
}

}

}

}
