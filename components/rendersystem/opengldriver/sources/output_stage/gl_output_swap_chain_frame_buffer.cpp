#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Буфер кадра на основе цепочек обмена
*/

namespace
{

class SwapChainFrameBuffer: public FrameBuffer
{
  public:
      //конструктор
    SwapChainFrameBuffer (const ContextManager& context_manager, ISwapChain* in_swap_chain) :
      FrameBuffer (context_manager),
      swap_chain (in_swap_chain)
    {
      if (!in_swap_chain)
        RaiseNullArgument ("render::low_level::opengl::SwapChainFrameBuffer::SwapChainFrameBuffer", "swap_chain");

        //для проверки совместимости с контекстом

      Bind ();
    }

      //присоединение буфера к контексту OpenGL
    void Bind ()
    {
      GetContextManager ().SetSwapChains (&*swap_chain, &*swap_chain);

      MakeContextCurrent ();
    }
    
      //получение дескриптора
    void GetDesc (FrameBufferDesc& out_desc)
    {
      SwapChainDesc swap_chain_desc;

      swap_chain->GetDesc (swap_chain_desc);

      out_desc = swap_chain_desc.frame_buffer;
    }
    
      //получение буфера цвета
    IColorBuffer* GetColorBuffer (size_t)
    {
      RaiseNotImplemented ("render::low_level::opengl::SwapChainFrameBuffer::GetColorBuffer");
      return 0;
    }
    
      //получение depth-stencil буфера
    IDepthStencilBuffer* GetDepthStencilBuffer ()
    {
      RaiseNotImplemented ("render::low_level::opengl::SwapChainFrameBuffer::GetDepthStencilBuffer");
      return 0;
    }

  private:
    typedef xtl::com_ptr<ISwapChain> SwapChainPtr;

  private:
    SwapChainPtr swap_chain;      //цепочка обмена
};

}

/*
    Создание SwapChainFrameBuffer
*/

FrameBuffer* FrameBuffer::Create (const ContextManager& context_manager, ISwapChain* swap_chain)
{
  return new SwapChainFrameBuffer (context_manager, swap_chain);
}
