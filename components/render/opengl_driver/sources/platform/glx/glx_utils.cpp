#include "shared.h"

namespace render
{

namespace low_level
{

namespace opengl
{

namespace glx
{

/*
    Проверка ошибок
*/

void check_errors (const char* source)
{
/*  EGLint status = eglGetError ();
  
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
  }*/
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
