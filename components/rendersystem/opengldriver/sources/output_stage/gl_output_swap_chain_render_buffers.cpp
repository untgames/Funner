#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
===================================================================================================
    SwapChainColorBuffer
===================================================================================================
*/

/*
    Конструктор
*/

SwapChainColorBuffer::SwapChainColorBuffer (const ContextManager& manager, ISwapChain* in_swap_chain, size_t in_buffer_index)
  : RenderBuffer (manager, RenderBufferType_Color),
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
    GetContextManager ().SetContext (0, swap_chain.get (), swap_chain.get ());
    MakeContextCurrent ();

    glReadBuffer (buffer_type);
    glDrawBuffer (buffer_type);

    CheckErrors (METHOD_NAME);
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
  : RenderBuffer (manager, RenderBufferType_DepthStencil)
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
  : RenderBuffer (manager, RenderBufferType_DepthStencil),
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
    GetContextManager ().SetContext (context_id, 0, 0);
    MakeContextCurrent ();
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainDepthStencilBuffer::Bind");
    
    throw;
  }  
}
