#include "shared.h"

using namespace common;

namespace
{

const GLEWContext*  current_glew_context  = 0; //текущий контекст GLEW
const WGLEWContext* current_wglew_context = 0; //текущий контекст WGLEW

}

namespace render
{

namespace low_level
{

namespace opengl
{

//установка текущего контекста GLEW
void glewSetContext (const GLEWContext* glew_context)
{
  current_glew_context  = glew_context;
}

//установка текущего контекста WGLEW
void wglewSetContext (const WGLEWContext* wglew_context)
{
  current_wglew_context = wglew_context;
}

//получение текущего контекста GLEW
const GLEWContext* glewGetContext ()
{
  return current_glew_context;
}

//получение текущего контекста WGLEW
const WGLEWContext* wglewGetContext ()
{
  return current_wglew_context;
}

namespace
{

//создание вспомогательного окна длЯ инициализации расширений WGL
HWND create_dummy_window (HWND parent)
{
  HWND window = CreateWindow ("static", "", WS_POPUP, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    parent, 0, GetModuleHandle (0), 0);

  if (!window)
    raise_error ("CreateWindow");
    
  return window;
}

}

//инициализациЯ контекста WGLEW
void init_wglew_context (const SwapChainDesc& swap_chain_desc, WGLEWContext* wglew_context)
{
  WGLEWScope wglew_scope;

    //создание вспомогательного контекста OpenGL

  HWND  window        = (HWND)swap_chain_desc.window_handle,
        dummy_window  = 0;
  HDC   dummy_dc      = 0;
  HGLRC dummy_context = 0;

    //отмена текущего WGLEW контекста
    
  wglewSetContext (0);
  
  try
  {
    dummy_window = create_dummy_window (window);
    dummy_dc     = GetDC (dummy_window);
    
    if (!dummy_dc)
      raise_error ("GetDC");

    int pixel_format = choose_pixel_format (dummy_dc, swap_chain_desc); //выбор формата пикселей без использованиЯ расширений
    
    PIXELFORMATDESCRIPTOR pfd;
  
    if (!DescribePixelFormat (dummy_dc, pixel_format, sizeof (pfd), &pfd))
      raise_error ("DescribePixelFormat");

    if (!SetPixelFormat (dummy_dc, pixel_format, &pfd))
      raise_error ("SetPixelFormat");

    dummy_context = wglCreateContext (dummy_dc);

    if (!dummy_context)
      raise_error ("wglCreateContext");
      
    make_current_context (dummy_context, dummy_dc, dummy_dc);

      //инициализациЯ расширений WGL

    GLenum status = wglewContextInit (wglew_context);

    if (status != GLEW_OK)
      throw xtl::format_operation_exception ("wglewContextInit", "%s", glewGetString (status));

      //освобождение ресурсов

    make_current_context (0, 0, 0);
    wglDeleteContext (dummy_context);
    ReleaseDC        (dummy_window, dummy_dc);
    DestroyWindow    (dummy_window);    
  }
  catch (...)
  {
    make_current_context (0, 0, 0);

    if (dummy_context) wglDeleteContext (dummy_context);
    if (dummy_dc)      ReleaseDC        (dummy_window, dummy_dc);
    if (dummy_window)  DestroyWindow    (dummy_window);

    throw;
  }  
}

/*
    WGLEWScope
*/

WGLEWScope::WGLEWScope ()
  : old_context (wglewGetContext ())
{
}

WGLEWScope::WGLEWScope (const WGLEWContext* new_context)
  : old_context (wglewGetContext ())
{
  wglewSetContext (new_context);
}

WGLEWScope::~WGLEWScope ()
{
  wglewSetContext (old_context);
}

}

}

}
