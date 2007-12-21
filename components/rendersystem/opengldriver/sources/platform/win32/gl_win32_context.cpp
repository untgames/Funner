#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Описание реализации контекста
*/

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

struct Context::Impl
{
  HGLRC                     gl_context;                 //контекст OpenGL
  int                       pixel_format;               //формат пикселей, требуемый контекстом
  ISwapChain*               draw_swap_chain;            //текущая цепочка обмена (для рисования)
  ISwapChain*               read_swap_chain;            //текущая цепочка обмена (для чтения)
  HDC                       draw_swap_chain_dc;         //контекст устройства текущей цепочки обмена (для рисования)
  HDC                       read_swap_chain_dc;         //контекст устройства текущей цепочки обмена (для чтения)
  GLEWContext               glew_context;               //контекст GLEW
  const WGLEWContext*       wglew_context;              //контекст WGLEW
  xtl::auto_slot<void ()>   on_destroy_draw_swap_chain; //обработчик удаления цепочки обмена (для рисования)
  xtl::auto_slot<void ()>   on_destroy_read_swap_chain; //обработчик удаления цепочки обмена (для чтения)

  Impl (ISwapChain* in_swap_chain, Impl* shared_context=0) : draw_swap_chain (0), read_swap_chain (0),
    on_destroy_draw_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this)),
    on_destroy_read_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this))
  {
    if (!in_swap_chain)
      RaiseNullArgument ("render::low_level::opengl::Context::Context", "swap_chain");
      
    SwapChain* swap_chain = cast_object<SwapChain> (in_swap_chain, "render::low_level::opengl::Context::Context", "swap_chain");
    
    HDC swap_chain_dc = swap_chain->GetDC ();
    
    pixel_format = GetPixelFormat (swap_chain_dc);

    gl_context = wglCreateContext (swap_chain_dc);

    if (!gl_context)
      raise_error ("wglCreateContext");
      
    try
    {
      if (shared_context && !wglShareLists (shared_context->gl_context, gl_context))
        raise_error ("wglShareLists");
      
      if (!wglMakeCurrent (swap_chain_dc, gl_context))
        raise_error ("wglMakeCurrent");
        
      GLenum status = glewContextInit (&glew_context);

      if (status != GLEW_OK)
        RaiseInvalidOperation ("glewContextInit", "%s", glewGetString (status));
        
      SetSwapChains (swap_chain, swap_chain);
    }
    catch (...)
    {
      wglMakeCurrent (0, 0);
      wglDeleteContext (gl_context);
      throw;
    }
  }

  ~Impl ()
  {
    if (wglGetCurrentContext () == gl_context)
    {
      wglewSetContext (wglew_context);

      if (wglewGetContext () && WGLEW_ARB_make_current_read) wglMakeContextCurrentARB (0, 0, 0);
      else                                                   wglMakeCurrent (0, 0);

      wglewSetContext (0);
      glewSetContext (0);
    }

    wglDeleteContext (gl_context);
  }
  
    //обработчик удаления цепочек обмена
  void OnDestroySwapChain ()
  {
    read_swap_chain    = draw_swap_chain = 0;
    read_swap_chain_dc = draw_swap_chain_dc = 0;
    wglew_context      = 0;
  }
  
    //установка активных цепочек обмена
  void SetSwapChains (ISwapChain* in_draw_swap_chain, ISwapChain* in_read_swap_chain)
  {
    SwapChain *casted_draw_swap_chain, *casted_read_swap_chain;

    casted_draw_swap_chain = cast_object<SwapChain> (in_draw_swap_chain, "render::low_level::opengl::Context::Impl::SetSwapChains", "draw_swap_chain");

    if (in_read_swap_chain != in_draw_swap_chain)
      casted_read_swap_chain = cast_object<SwapChain> (in_read_swap_chain, "render::low_level::opengl::Context::Impl::SetSwapChains", "read_swap_chain");
    else
      casted_read_swap_chain = casted_draw_swap_chain;

    draw_swap_chain_dc = casted_draw_swap_chain->GetDC ();
    read_swap_chain_dc = casted_read_swap_chain->GetDC ();
    draw_swap_chain    = in_draw_swap_chain;
    read_swap_chain    = in_read_swap_chain;
    wglew_context      = casted_draw_swap_chain->GetWGLEWContext ();

    casted_draw_swap_chain->RegisterDestroyHandler (on_destroy_draw_swap_chain);
    casted_read_swap_chain->RegisterDestroyHandler (on_destroy_read_swap_chain);
  }

    //установка текущего контекста
  void MakeCurrent (ISwapChain* in_draw_swap_chain, ISwapChain* in_read_swap_chain)
  {
    if (!in_draw_swap_chain)
      RaiseNullArgument ("render::low_level::opengl::Context::MakeCurrent", "draw_swap_chain");

    if (!in_read_swap_chain)
      RaiseNullArgument ("render::low_level::opengl::Context::MakeCurrent", "read_swap_chain");
      
    const GLEWContext*  old_glew_context  = glewGetContext ();
    const WGLEWContext* old_wglew_context = wglewGetContext ();

    try
    {
      if (in_draw_swap_chain != draw_swap_chain || in_read_swap_chain != read_swap_chain)
        SetSwapChains (in_draw_swap_chain, in_read_swap_chain);

      wglewSetContext (wglew_context);
      glewSetContext  (&glew_context);

      if (wglewGetContext () && WGLEW_ARB_make_current_read)
      {
        if (wglGetCurrentContext () == gl_context && wglGetCurrentDC () == draw_swap_chain_dc && wglGetCurrentReadDCARB () == read_swap_chain_dc)
          return;

        if (!wglMakeContextCurrentARB (draw_swap_chain_dc, read_swap_chain_dc, gl_context))
          raise_error ("wglMakeContextCurrentARB");
      }
      else
      {
        if (read_swap_chain_dc != draw_swap_chain_dc)
          RaiseNotSupported ("render::low_level::opengl::Context::MakeCurrent", "WGL_ARB_make_current_read extenstion not supported (could not set different read/write swap chains)");
          
        if (wglGetCurrentContext () == gl_context && wglGetCurrentDC () == draw_swap_chain_dc)
          return;

        if (!wglMakeCurrent (draw_swap_chain_dc, gl_context))
          raise_error ("wglMakeCurrent");
      }
    }
    catch (common::Exception& exception)
    {
      glewSetContext (old_glew_context);
      wglewSetContext (old_wglew_context);
      
      exception.Touch ("render::low_level::opengl::Context::MakeCurrent");
      
      throw;
    }
  }
  
    //проверка является ли контекст текущим
  bool IsCurrent (ISwapChain* in_draw_swap_chain, ISwapChain* in_read_swap_chain)
  {
    if (!in_draw_swap_chain || !in_read_swap_chain)
      return false;
      
    if (wglGetCurrentContext () != gl_context)
      return false;
      
    SwapChain *draw_swap_chain = dynamic_cast<SwapChain*> (in_draw_swap_chain),
              *read_swap_chain = dynamic_cast<SwapChain*> (in_read_swap_chain);
              
    if (draw_swap_chain->GetDC () != wglGetCurrentDC ())
      return false;

    WGLEWScope wglew_scope (read_swap_chain->GetWGLEWContext ());

    if   (wglewGetContext () && WGLEW_ARB_make_current_read) return wglGetCurrentReadDCARB () == read_swap_chain->GetDC ();
    else                                                     return draw_swap_chain == read_swap_chain;
  }
  
    //проверка совместимости цепочки обмена с контекстом
  bool IsCompatible (ISwapChain* in_swap_chain)
  {
    if (!in_swap_chain)
      return false;

    SwapChain* swap_chain = dynamic_cast<SwapChain*> (in_swap_chain);

    if (!swap_chain)
      return false;

    return GetPixelFormat (swap_chain->GetDC ()) == pixel_format;
  }
};

/*
    Конструктор / деструктор
*/

Context::Context (ISwapChain* in_swap_chain)
{
  try
  {
    impl = new Impl (in_swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Context::Context");
    throw;
  }
}

Context::Context (ISwapChain* in_swap_chain, const Context& shared_context)
{
  try
  {
    impl = new Impl (in_swap_chain, get_pointer (shared_context.impl));
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Context::Context");
    throw;
  }
}

Context::~Context ()
{
}

/*
    Установка текущего контекста
*/

void Context::MakeCurrent (ISwapChain* swap_chain)
{
  impl->MakeCurrent (swap_chain, swap_chain);
}

void Context::MakeCurrent (ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain)
{
  impl->MakeCurrent (draw_swap_chain, read_swap_chain);
}

bool Context::IsCurrent (ISwapChain* draw_swap_chain, ISwapChain* read_swap_chain) const
{
  return impl->IsCurrent (draw_swap_chain, read_swap_chain);
}

bool Context::IsCurrent (ISwapChain* swap_chain) const
{
  return impl->IsCurrent (swap_chain, swap_chain);
}

/*
    Проверка совместимости цепочки обмена с контекстом
*/

bool Context::IsCompatible (ISwapChain* swap_chain) const
{
  return impl->IsCompatible (swap_chain);
}

/*
    Получение списка расширений, зависящих от текущей цепочки обмена
*/

const char* Context::GetSwapChainExtensionString ()
{
  return WGLEW_ARB_extensions_string ? wglGetExtensionsStringARB (wglGetCurrentDC ()) : "";
}
