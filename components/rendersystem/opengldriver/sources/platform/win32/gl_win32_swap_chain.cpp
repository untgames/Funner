#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
     онструктор / деструктор
*/

SwapChain::SwapChain (OutputManager&, const SwapChainDesc& in_desc)
  : output (0), output_window ((HWND)in_desc.window_handle)
{
  if (!output_window)
    RaiseNullArgument ("render::low_level::opengl::SwapChain::SwapChain", "swap_chain_desc.window_handle");

    //получение контекста устройства вывода
    
  output_device_context = ::GetDC (output_window);
  
  if (!output_device_context)
    raise_error ("GetDC");

  try
  {
    ThreadLock lock;
    
      //инициализаци€ и установка контекста WGLEW

    try
    {
      init_wglew_context       (in_desc, &wglew_context);
      set_current_glew_context (0, &wglew_context);
    }
    catch (std::exception& exception)
    {
          //заменить printf на вывод в лог-поток!!!!!
      printf ("SwapChain::SwapChain: Error at initialize WGLEWContext. %s\n", exception.what ());      
    }
    catch (...)
    {
      printf ("SwapChain::SwapChain: Unknown exception at initialize WGLEWContext\n");
    }    

      //установка формата пикселей

    set_pixel_format (output_device_context, in_desc);
    
      //получение установленного формата пикселей
      
    get_pixel_format (output_device_context, desc);
    
      //перенесение дублируемых полей
    
    desc.vsync         = in_desc.vsync;
    desc.window_handle = in_desc.window_handle;

      //добавить установку/вз€тие состо€ни€ FullScreen-mode!!!       

      //отмена текущего контекста WGLEW

    set_current_glew_context (0, 0);
  }
  catch (...)
  {
    set_current_glew_context (0, 0);
    ReleaseDC (output_window, output_device_context);
    throw;
  }
}

SwapChain::~SwapChain ()
{
  ReleaseDC (output_window, output_device_context);
}

/*
    ѕолучение дескриптора
*/

void SwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

/*
    ѕолучение устройства вывода
*/

IOutput* SwapChain::GetOutput ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::GetOutput");
  return get_pointer (output);
}

/*
    ќбмен текущего заднего буфера и переднего буфера
*/

void SwapChain::Present ()
{
  if (!SwapBuffers (output_device_context))
    raise_error (format ("render::low_level::opengl::SwapChain::Present(device-name='%s')", output->GetName ()).c_str ());
}

/*
    ”становка / вз€тие состо€ни€ full-screen mode
*/

void SwapChain::SetFullscreenState (bool state)
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::SetFullscreenState");
}

bool SwapChain::GetFullscreenState ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChain::SetFullscreenState");
  return false;
}

/*
    —оздание цепочки обмена
*/

namespace render
{

namespace low_level
{

namespace opengl
{

ISwapChain* create_swap_chain (OutputManager& output_manager, const SwapChainDesc& swap_chain_desc)
{
  return new SwapChain (output_manager, swap_chain_desc);
}

}

}

}
