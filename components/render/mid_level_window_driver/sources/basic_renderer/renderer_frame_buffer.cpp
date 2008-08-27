#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level::window_driver;

namespace
{

typedef xtl::com_ptr<ITexture> TexturePtr;
typedef xtl::com_ptr<IView>    ViewPtr;

}

/*
   Буфер кадра
*/

/*
   Конструктор / деструктор
*/

FrameBuffer::FrameBuffer (IDevice& device, ISwapChain& in_swap_chain)
  : swap_chain (&in_swap_chain)
{
  try
  {
      //получение дескриптора цепочки обмена

    SwapChainDesc swap_chain_desc;

    swap_chain->GetDesc (swap_chain_desc);
    
    TexturePtr texture;
    ViewPtr    view;
    ViewDesc   view_desc;

    memset (&view_desc, 0, sizeof view_desc);
    
      //создание буфера цвета

    texture      = TexturePtr (device.CreateRenderTargetTexture (swap_chain.get (), swap_chain_desc.buffers_count ? 1 : 0), false);
    view         = ViewPtr (device.CreateView (texture.get (), view_desc), false);
    color_buffer = RenderTargetPtr (new RenderTarget (view.get (), RenderTargetType_Color), false);
    
      //создание буфера попиксельного отсечения

    texture              = TexturePtr (device.CreateDepthStencilTexture (swap_chain.get ()), false);
    view                 = ViewPtr (device.CreateView (texture.get (), view_desc), false);
    depth_stencil_buffer = RenderTargetPtr (new RenderTarget (view.get (), RenderTargetType_DepthStencil), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::mid_level::window_driver::FrameBuffer::FrameBuffer");
    throw;
  }
}

FrameBuffer::~FrameBuffer ()
{
}

/*
   Получение буфера цвета и буфера попиксельного отсечения
*/

render::mid_level::IRenderTarget* FrameBuffer::GetColorBuffer ()
{
  return color_buffer.get ();
}

render::mid_level::IRenderTarget* FrameBuffer::GetDepthStencilBuffer ()
{
  return depth_stencil_buffer.get ();
}

/*
   Установка размера
*/

void FrameBuffer::SetSize (size_t width, size_t height)
{
  color_buffer->SetSize (width, height);
  depth_stencil_buffer->SetSize (width, height);
}
