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
    SwapChainFrameBuffer (const ContextManager& in_context_manager, ISwapChain* in_swap_chain) :
      swap_chain (in_swap_chain), context_manager (in_context_manager)
    {
      if (!in_swap_chain)
        RaiseNullArgument ("render::low_level::opengl::SwapChainFrameBuffer::SwapChainFrameBuffer", "swap_chain");
        
        //для проверки совместимости с контекстом
       
      Bind ();
    }
    
      //получение менеджера контекстов
    ContextManager& GetContextManager () { return context_manager; }

      //присоединение буфера к контексту OpenGL
    void Bind ()
    {
      context_manager.SetSwapChains (&*swap_chain, &*swap_chain);
      context_manager.MakeContextCurrent ();
    }
    
      //получение дескриптора
    void GetDesc (FrameBufferDesc& out_desc)
    {
      SwapChainDesc swap_chain_desc;

      swap_chain->GetDesc (swap_chain_desc);

      out_desc = swap_chain_desc.frame_buffer;
    }
    
      //получение цветовой поверхности
    IRenderTargetSurface* GetRenderTarget ()
    {
      RaiseNotImplemented ("render::low_level::opengl::SwapChainFrameBuffer::GetRenderTarget");
      return 0;
    }
    
      //получение depth-stencil поверхности
    IDepthStencilSurface* GetDepthStencil ()
    {
      RaiseNotImplemented ("render::low_level::opengl::SwapChainFrameBuffer::GetDepthStencil");
      return 0;
    }

  private:
    typedef xtl::com_ptr<ISwapChain> SwapChainPtr;

  private:
    ContextManager  context_manager; //менеджер контекстов
    SwapChainPtr    swap_chain;      //цепочка обмена
};

}

/*
    Создание SwapChainFrameBuffer
*/

FrameBuffer* FrameBuffer::Create (const ContextManager& context_manager, ISwapChain* swap_chain)
{
  return new SwapChainFrameBuffer (context_manager, swap_chain);
}

