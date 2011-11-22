#include "gpu.h"
#include "glcontext.h"
#include <GL/gl.h>

namespace geekinfo {
  
GPU::GPU()
{
  gather_information();
}

void GPU::gather_information()
{
  GLContext context;
  
  m_renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
  m_vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
  m_version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
  m_extensions = reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS));
}

} // namespace geekinfo

