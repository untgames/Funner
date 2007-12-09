#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
     онструктор / деструктор
*/

PrimarySwapChain::PrimarySwapChain (IOutput* in_output, const SwapChainDesc& in_desc)
  : output (in_output)
{
    //получение окна вывода
    
  output_window = (HWND)in_desc.window_handle;
  
  if (!output_window)
    RaiseNullArgument ("render::low_level::opengl::PrimarySwapChain::PrimarySwapChain", "swap_chain_desc.window_handle");  
    
  output_context = ::GetDC (output_window);
  
  if (!output_context)
    raise_error ("GetDC");
    
    //предварительна€ установка SwapChainDesc
    
  desc = in_desc;
    
    //установка состо€ни€ FullScreen

  if (in_desc.fullscreen)
    SetFullscreenState (true);

    //инициализаци€ и установка контекста WGLEW

  init_wglew_context (in_desc, &wglew_context);

  WGLEWScope wglew_scope (&wglew_context);

    //установка текущего формата пикселей дл€ окна
    
  int pixel_format = choose_pixel_format (output_context, in_desc);
    
  PIXELFORMATDESCRIPTOR pfd;    
  
  if (!DescribePixelFormat (output_context, pixel_format, sizeof (pfd), &pfd))
    raise_error ("DescribePixelFormat");

  if (!SetPixelFormat (output_context, pixel_format, &pfd))
    raise_error ("SetPixelFormat");

    //получение установленного формата пикселей

  get_pixel_format (output_context, pixel_format, desc);

    //перенесение дублируемых полей
  
  desc.vsync         = in_desc.vsync;
  desc.window_handle = in_desc.window_handle;
  desc.fullscreen    = GetFullscreenState ();
}

PrimarySwapChain::~PrimarySwapChain ()
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

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

/*
    ѕолучение устройства вывода с максимальным размером области перекрыти€
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return get_pointer (output);
}

/*
    ќбмен текущего заднего буфера и переднего буфера
*/

void PrimarySwapChain::Present ()
{
  if (!SwapBuffers (output_context))
    raise_error (format ("render::low_level::opengl::PrimarySwapChain::Present(device-name='%s')", output->GetName ()).c_str ());
}

/*
    ”становка / вз€тие состо€ни€ full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
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

bool PrimarySwapChain::GetFullscreenState ()
{
  OutputModeDesc mode_desc;
  
  output->GetCurrentMode (mode_desc);

  RECT window_rect;

  if (!GetWindowRect (output_window, &window_rect))
    raise_error ("GetWindowRect");
    
  return window_rect.left == 0 && window_rect.top == 0 && window_rect.right - window_rect.left == mode_desc.width &&
         window_rect.bottom - window_rect.top == mode_desc.height && desc.frame_buffer.color_bits >= mode_desc.color_bits;
}
