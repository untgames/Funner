#ifndef GEEKINFO_GLCONTEXT_H__
#define GEEKINFO_GLCONTEXT_H__

#include "platform.h"

#if defined(PLATFORM_MACOSX)
#include <AGL/agl.h>
#elif defined(PLATFORM_WINDOWS)
#include <windows.h>
#include <GL/gl.h>
#endif

namespace geekinfo {
  
class GLContext {
public:
  GLContext();
  ~GLContext();
  
protected:
#if defined(PLATFORM_MACOSX)
  AGLContext m_ctx;
  AGLContext m_octx;
#elif defined(PLATFORM_WINDOWS)
  HWND m_hwnd;
  HDC m_dc;
  HGLRC m_rc;
#endif
};

} // namespace geekinfo

#endif // GEEKINFO_GLCONTEXT_H__

