#include "shared.h"

namespace render
{

namespace low_level
{

namespace opengl
{

namespace egl
{

/*
    Проверка ошибок
*/

const char* get_error_message (EGLint status)
{
  switch (status)
  {
    case EGL_SUCCESS:              return "No errors";
    case EGL_NOT_INITIALIZED:      return "EGL is not initialized, or could not be initialized, for the specified EGL display connection";
    case EGL_BAD_ACCESS:           return "EGL cannot access a requested resource (for example a context is bound in another thread)";
    case EGL_BAD_ALLOC:            return "EGL failed to allocate resources for the requested operation";
    case EGL_BAD_ATTRIBUTE:        return "An unrecognized attribute or attribute value was passed in the attribute list";
    case EGL_BAD_CONFIG:           return "An EGLConfig argument does not name a valid EGL frame buffer configuration";
    case EGL_BAD_CONTEXT:          return "An EGLContext argument does not name a valid EGL rendering context";
    case EGL_BAD_CURRENT_SURFACE:  return "The current surface of the calling thread is a window, pixel buffer or pixmap that is no longer valid";
    case EGL_BAD_DISPLAY:          return "An EGLDisplay argument does not name a valid EGL display connection";
    case EGL_BAD_MATCH:            return "Arguments are inconsistent (for example, a valid context requires buffers not supplied by a valid surface)";
    case EGL_BAD_NATIVE_PIXMAP:    return "A NativePixmapType argument does not refer to a valid native pixmap";
    case EGL_BAD_NATIVE_WINDOW:    return "A NativeWindowType argument does not refer to a valid native window";
    case EGL_BAD_PARAMETER:        return "One or more argument values are invalid";
    case EGL_BAD_SURFACE:          return "An EGLSurface argument does not name a valid surface (window, pixel buffer or pixmap) configured for GL rendering";
    case EGL_CONTEXT_LOST:         return "Context is lost";
    default:                       return 0;
  }
}

void check_errors (const char* source)
{
  DisplayLock lock;    

  EGLint status = eglGetError ();
  
  switch (status)
  {
    case EGL_SUCCESS:
      return;
    default:
    {
      const char* message = get_error_message (status);
      
      if (message)
        throw xtl::format_operation_exception (source, "%s", message);
        
      throw xtl::format_operation_exception (source, "EGL error %04x", status);
    }
  }
}

void raise_error (const char* source)
{
  check_errors (source);

  throw xtl::format_operation_exception (source, "Operation failed");
}

}

}

}

}
