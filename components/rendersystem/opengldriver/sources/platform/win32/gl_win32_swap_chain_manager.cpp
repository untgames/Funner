#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Создание основной цепочки обмена
*/

ISwapChain* SwapChainManager::CreateSwapChain (OutputManager& output_manager, const SwapChainDesc& swap_chain_desc)
{
  HWND window = (HWND)swap_chain_desc.window_handle;
  
  if (!window)
    RaiseNullArgument ("render::low_level::opengl::SwapChainManager::CreateSwapChain", "swap_chain_desc.window_handle");

  IOutput* output = output_manager.FindContainingOutput ((void*)window);

  if (!output)
  {
    char title [128];

    GetWindowText (window, title, sizeof (title));

    RaiseInvalidOperation ("render::low_level::opengl::SwapChainManager::CreateSwapChain", "Can not find containing output for window '%s'", title);
  }
  
  return new PrimarySwapChain (output, swap_chain_desc);
}

/*
    Создание PBuffer'а
*/

ISwapChain* SwapChainManager::CreatePBuffer (ISwapChain* primary_swap_chain, const SwapChainDesc& pbuffer_desc)
{
  try
  {
    return new PBuffer (cast_object<PrimarySwapChain> (primary_swap_chain, "render::low_level::SwapChainManager::CreatePBuffer", 
      "primary_swap_chain"), pbuffer_desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainManager::CreatePBuffer(%ux%ux%u)",
      pbuffer_desc.frame_buffer.width, pbuffer_desc.frame_buffer.height, pbuffer_desc.frame_buffer.color_bits);

    throw;
  }
}

ISwapChain* SwapChainManager::CreatePBuffer (ISwapChain* swap_chain)
{
  try
  {
    return new PBuffer (cast_object<PrimarySwapChain> (swap_chain, "render::low_level::SwapChainManager::CreatePBuffer", "swap_chain"));
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainManager::CreatePBuffer");

    throw;
  }
}
