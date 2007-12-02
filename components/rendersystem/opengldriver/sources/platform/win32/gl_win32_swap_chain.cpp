#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
     онструктор / деструктор
*/

SwapChain::SwapChain (IOutput* in_output, const SwapChainDesc& in_desc)
  : output (in_output)
{
    //получение окна вывода
    
  output_window = (HWND)in_desc.window_handle;
  
  if (!output_window)
    RaiseNullArgument ("render::low_level::opengl::SwapChain::SwapChain", "swap_chain_desc.window_handle");  
    
  output_context = ::GetDC (output_window);
  
  if (!output_context)
    raise_error ("GetDC");
    
    //предварительна€ установка SwapChainDesc
    
  desc = in_desc;
    
    //установка состо€ни€ FullScreen

  if (in_desc.fullscreen)
    SetFullscreenState (true);

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

    set_pixel_format (GetDC (), in_desc);        
      
      //получение установленного формата пикселей
      
    get_pixel_format (GetDC (), desc);      
      
      //перенесение дублируемых полей
    
    desc.vsync         = in_desc.vsync;
    desc.window_handle = in_desc.window_handle;
    desc.fullscreen    = GetFullscreenState ();      

      //отмена текущего контекста WGLEW

    set_current_glew_context (0, 0);
  }
  catch (...)
  {
    set_current_glew_context (0, 0);
    throw;
  }
}

SwapChain::~SwapChain ()
{
  ReleaseDC (output_window, output_context);    
  
  try
  {
    if (GetFullscreenState () == desc.fullscreen)
      SetFullscreenState (false);
  }
  catch (...)  
  {
    //подавл€ем все исключени€
  }
}

/*
    ѕолучение дескриптора
*/

void SwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

/*
    ѕолучение устройства вывода с максимальным размером области перекрыти€
*/

IOutput* SwapChain::GetContainingOutput ()
{
  return get_pointer (output);
}

/*
    ќбмен текущего заднего буфера и переднего буфера
*/

void SwapChain::Present ()
{
  if (!SwapBuffers (GetDC ()))
    raise_error (format ("render::low_level::opengl::SwapChain::Present(device-name='%s')", output->GetName ()).c_str ());
}

/*
    ”становка / вз€тие состо€ни€ full-screen mode
*/

void SwapChain::SetFullscreenState (bool state)
{
  if (GetFullscreenState () == state)
    return;
    
  if (state)
  {
    if (!SetWindowPos (output_window, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE))
      raise_error ("SetWindowPos");

    OutputModeDesc mode_desc;

    RECT window_rect;

    if (!GetWindowRect (output_window, &window_rect))
      raise_error ("GetWindowRect");

    mode_desc.width        = window_rect.right - window_rect.left;
    mode_desc.height       = window_rect.bottom - window_rect.top;
    mode_desc.color_bits   = desc.frame_buffer.color_bits;
    mode_desc.refresh_rate = 0;

    output->SetCurrentMode (mode_desc);
  }
  else
  {
    if (ChangeDisplaySettings (0, 0) != DISP_CHANGE_SUCCESSFUL)
      raise_error ("ChangeDisplaySettings(0,0)");
  }
}

bool SwapChain::GetFullscreenState ()
{
  OutputModeDesc mode_desc;
  
  output->GetCurrentMode (mode_desc);

  RECT window_rect;

  if (!GetWindowRect (output_window, &window_rect))
    raise_error ("GetWindowRect");
    
  return window_rect.left == 0 && window_rect.top == 0 && window_rect.right - window_rect.left == mode_desc.width &&
         window_rect.bottom - window_rect.top == mode_desc.height && desc.frame_buffer.color_bits >= mode_desc.color_bits;
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
  HWND window = (HWND)swap_chain_desc.window_handle;
  
  if (!window)
    RaiseNullArgument ("render::low_level::opengl::create_swap_chain", "swap_chain_desc.window_handle");

  IOutput* output = output_manager.FindContainingOutput ((void*)window);

  if (!output)
  {
    char title [128];

    GetWindowText (window, title, sizeof (title));

    RaiseInvalidOperation ("render::low_level::opengl::create_swap_chain", "Can not find containing output for window '%s'", title);
  }

  return new SwapChain (output, swap_chain_desc);
}

}

}

}
