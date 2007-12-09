#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

PBuffer::PBuffer (PrimarySwapChain* in_primary_swap_chain, const PBufferDesc& in_desc)
  : primary_swap_chain (in_primary_swap_chain), desc (in_desc), pbuffer (0), output_context (0)
{
  Create ();
}

PBuffer::~PBuffer ()
{
  Destroy ();
}

/*
    Создание / уничтожение буфера
*/

void PBuffer::Create ()
{
    //установка контекста WGLEW

  WGLEWScope wglew_scope (primary_swap_chain->GetWGLEWContext ());

  try
  {
    if (!WGLEW_ARB_pbuffer)
      RaiseNotSupported ("render::low_level::opengl::PBuffer::PBuffer", "PBuffer does not supproted");
      
    HDC primary_device_context = primary_swap_chain->GetDC ();
      
      //поиск подходящего формата пикселей
      
    int pixel_format = choose_pixel_format (primary_device_context, desc);
    
      //создание PBuffer
      
    int pbuffer_attributes [2] = {0, 0};
      
    pbuffer = wglCreatePbufferARB (primary_device_context, pixel_format, desc.width, desc.height, pbuffer_attributes);
    
    if (!pbuffer)
      raise_error ("wglCreatePbufferARB");
      
      //получение контекста вывода
      
    output_context = wglGetPbufferDCARB (pbuffer);
    
    if (!output_context)
      raise_error ("wglGetPbufferDCARB");            
      
      //получение дескриптора формата пикселей

    get_pixel_format (output_context, pixel_format, desc);
 
      //получение размеров созданного PBuffer'а
      
    int width, height;

    if (!wglQueryPbufferARB (pbuffer, WGL_PBUFFER_WIDTH_ARB, &width) || !wglQueryPbufferARB (pbuffer, WGL_PBUFFER_HEIGHT_ARB, &height))
      raise_error ("wglQueryPBufferARB");

    desc.width  = (size_t)width;
    desc.height = (size_t)height;

      //перенесение дублируемых полей

    desc.vsync         = false;
    desc.fullscreen    = false;
    desc.window_handle = 0;
  }
  catch (...)
  {
    if (output_context) wglReleasePbufferDCARB (pbuffer, output_context);
    if (pbuffer)        wglDestroyPbufferARB (pbuffer);
    
    output_context = 0;
    pbuffer = 0;

    throw;
  }
}

void PBuffer::Destroy ()
{
  try
  {
    WGLEWScope wglew_scope (primary_swap_chain->GetWGLEWContext ());

    wglReleasePbufferDCARB (pbuffer, output_context);
    wglDestroyPbufferARB   (pbuffer);
  }
  catch (...)
  {
    //подавляем все исключения
  }
  
  output_context = 0;  
  pbuffer = 0;
}


/*
    Получение дескриптора
*/

void PBuffer::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = desc;
}

void PBuffer::GetDesc (PBufferDesc& out_desc)
{
  out_desc = desc;
}

/*
    Получение устройства вывода с максимальным размером области перекрытия
*/

IOutput* PBuffer::GetContainingOutput ()
{
  return primary_swap_chain->GetContainingOutput ();
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void PBuffer::Present ()
{
}

/*
    Установка / взятие состояния full-screen mode
*/

void PBuffer::SetFullscreenState (bool)
{
}

bool PBuffer::GetFullscreenState ()
{
  return false;
}

/*
    Контекст устройства вывода / WGLEW контекст
*/

HDC PBuffer::GetDC ()
{
  return output_context;
/*  set_current_glew_context (0, primary_swap_chain->GetWGLEWContext ());

  int is_lost;
  
  if (!wglQueryPbufferARB (pbuffer, WGL_PBUFFER_LOST_ARB))
    raise_error ("wglQueryPBufferARB");

  if (is_lost)
  {
    Destroy ();
    Create ();
  }

  return output_context;*/
}

const WGLEWContext* PBuffer::GetWGLEWContext ()
{
  return primary_swap_chain->GetWGLEWContext ();
}
