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
    Получние номера экрана
*/

Screen* get_screen (Window window)
{
  Display* display = (Display*) syslib::x11::DisplayManager::DisplayHandle ();

  DisplayLock lock (display);

  XWindowAttributes window_attributes_return;
  
  Status result = XGetWindowAttributes (display, window, &window_attributes_return);
  
  if (result < Success)
    throw xtl::format_operation_exception ("render::low_level::opengl::glx::GetScreenNumber",
      "XGetWindowAttributes failed");
      
  // back pointer to correct screen
  Screen *screen = window_attributes_return.screen;
  
  if (!screen)
    throw xtl::format_operation_exception ("render::low_level::opengl::glx::GetScreenNumber",
      "incorrect screen pointer");
      
  return screen;
}

int get_screen_number (Window window)
{
  return XScreenNumberOfScreen (get_screen (window));
}

/*
    Проверка ошибок
*/

int glxErrorHandler (Display *display, XErrorEvent *error_event)
{
  return 0;
}

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
