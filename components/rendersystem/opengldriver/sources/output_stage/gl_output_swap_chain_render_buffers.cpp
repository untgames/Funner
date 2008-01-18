#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
===================================================================================================
    SwapChainRenderBuffer
===================================================================================================
*/

/*
    Конструктор
*/

SwapChainRenderBuffer::SwapChainRenderBuffer (const ContextManager& manager, RenderTargetType target_type)
  : RenderBuffer (manager, target_type)
{
  PixelFormat format;
  size_t      bind_flags;

  switch (target_type)
  {
    case RenderTargetType_Color:
      format     = PixelFormat_RGBA8;
      bind_flags = BindFlag_RenderTarget;
      break;
    case RenderTargetType_DepthStencil:
      format     = PixelFormat_D24S8;
      bind_flags = BindFlag_DepthStencil;
      break;
    default:
      RaiseInvalidArgument ("render::low_level::opengl::SwapChainRenderBuffer::SwapChainRenderBuffer", "target_type", target_type);
      break;
  }
  
  memset (&desc, 0, sizeof (desc));  

  desc.dimension            = TextureDimension_2D;
  desc.width                = 0;
  desc.height               = 0;
  desc.layers               = 1;
  desc.format               = format;
  desc.generate_mips_enable = false;
  desc.access_flags         = AccessFlag_Read | AccessFlag_Write;  
  desc.bind_flags           = bind_flags;
  desc.usage_mode           = UsageMode_Static;
}

/*
    Получение дескриптора
*/

void SwapChainRenderBuffer::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
  
  GetSize (out_desc.width, out_desc.height);
}

/*
===================================================================================================
    SwapChainColorBuffer
===================================================================================================
*/

/*
    Конструктор
*/

SwapChainColorBuffer::SwapChainColorBuffer (const ContextManager& manager, ISwapChain* in_swap_chain, size_t in_buffer_index)
  : SwapChainRenderBuffer (manager, RenderTargetType_Color),
    swap_chain (in_swap_chain),
    buffer_index (in_buffer_index)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainColorBuffer::SwapChainColorBuffer";

  if (!swap_chain)
    RaiseNullArgument (METHOD_NAME, "swap_chain");
    
  SwapChainDesc desc;

  swap_chain->GetDesc (desc);

  if (buffer_index >= desc.buffers_count)
    RaiseOutOfRange (METHOD_NAME, "buffer_index", buffer_index, desc.buffers_count);
    
  switch (buffer_index)
  {
    case 0:  buffer_type = GL_FRONT; break;
    default: buffer_type = GL_BACK;  break;
  }
}

/*
    Получение размеров буфера
*/

void SwapChainColorBuffer::GetSize (size_t& width, size_t& height)
{
  SwapChainDesc desc;

  swap_chain->GetDesc (desc);
  
  width  = desc.frame_buffer.width;
  height = desc.frame_buffer.height;
}
    
/*
    Установка в контекст OpenGL
*/

void SwapChainColorBuffer::Bind ()
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainColorBuffer::Bind";

  try    
  {
      //выбор текущего контекста
    
    GetContextManager ().SetContext (0, swap_chain.get (), swap_chain.get ());
    MakeContextCurrent ();
    
      //установка буфера кадра по умолчанию
      
    static Extension EXT_framebuffer_object = "GL_EXT_framebuffer_object";
    
    if (IsSupported (EXT_framebuffer_object))
      glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

      //настройка буферов отрисовки и чтения

    glReadBuffer (buffer_type);
    glDrawBuffer (buffer_type);

      //проверка ошибок

    CheckErrors ("");
  }
  catch (common::Exception& exception)
  {
    exception.Touch (METHOD_NAME);
    
    throw;
  }
}

/*
===================================================================================================
    SwapDepthStencilBuffer
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer (const ContextManager& manager, ISwapChain* swap_chain)
  : SwapChainRenderBuffer (manager, RenderTargetType_DepthStencil)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer";

  if (!swap_chain)
    RaiseNullArgument (METHOD_NAME, "swap_chain");
  
  SwapChainDesc swap_chain_desc;

  swap_chain->GetDesc (swap_chain_desc);

  width  = swap_chain_desc.frame_buffer.width;
  height = swap_chain_desc.frame_buffer.height;

  context_id = GetContextManager ().CreateContext (swap_chain);  
}

SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer (const ContextManager& manager, ISwapChain* swap_chain, size_t in_width, size_t in_height)
  : SwapChainRenderBuffer (manager, RenderTargetType_DepthStencil),
    width (in_width),
    height (in_height)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainDepthStencilBuffer::SwapChainDepthStencilBuffer";

  if (!swap_chain)
    RaiseNullArgument (METHOD_NAME, "swap_chain");
    
  SwapChainDesc swap_chain_desc;

  swap_chain->GetDesc (swap_chain_desc);
  
  if (width > swap_chain_desc.frame_buffer.width)
    RaiseNotSupported (METHOD_NAME, "Depth-stencil buffer width=%u is greater than swap-chain width=%u", width,
      swap_chain_desc.frame_buffer.width);

  if (height > swap_chain_desc.frame_buffer.height)
    RaiseNotSupported (METHOD_NAME, "Depth-stencil buffer height=%u is greater than swap-chain height=%u", height,
      swap_chain_desc.frame_buffer.height);

  context_id = GetContextManager ().CreateContext (swap_chain);  
}

SwapChainDepthStencilBuffer::~SwapChainDepthStencilBuffer ()
{
  GetContextManager ().DeleteContext (context_id);
}

/*
    Получение размеров буфера
*/

void SwapChainDepthStencilBuffer::GetSize (size_t& out_width, size_t& out_height)
{
  out_width  = width;
  out_height = height;
}
    
/*
    Установка в контекст OpenGL
*/

void SwapChainDepthStencilBuffer::Bind ()
{
  try
  {
      //выбор текущего контекста
    
    GetContextManager ().SetContext (context_id, 0, 0);
    MakeContextCurrent ();
    
      //установка буфера кадра по умолчанию
      
    static Extension EXT_framebuffer_object = "GL_EXT_framebuffer_object";
    
    if (IsSupported (EXT_framebuffer_object))
      glBindFramebufferEXT (GL_FRAMEBUFFER_EXT, 0);

      //проверка ошибок
      
    CheckErrors ("");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainDepthStencilBuffer::Bind");
    
    throw;
  }  
}
