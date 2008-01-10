#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
===================================================================================================
    Внутренние интерфейсы
===================================================================================================
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация буфера кадра для цепочки обмена
///////////////////////////////////////////////////////////////////////////////////////////////////
class SwapChainFrameBuffer: public FrameBuffer, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainFrameBuffer (const ContextManager&        manager,
                          SwapChainColorBuffer*        color_buffer,
                          SwapChainDepthStencilBuffer* depth_stencil_buffer);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка буфера в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление целевых буферов отрисовки
///////////////////////////////////////////////////////////////////////////////////////////////////
    void UpdateRenderTargets ();
    
  private:
    typedef xtl::com_ptr<SwapChainColorBuffer>        ColorBufferPtr;
    typedef xtl::com_ptr<SwapChainDepthStencilBuffer> DepthStencilBufferPtr;

  private:
    ColorBufferPtr        color_buffer;
    DepthStencilBufferPtr depth_stencil_buffer;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация фабрики ресурсов выходного подуровня OpenGL по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultOutputStageResourceFactory: public OutputStageResourceFactory, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DefaultOutputStageResourceFactory  (const ContextManager& manager);
    ~DefaultOutputStageResourceFactory ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание целевых буферов вывода
///////////////////////////////////////////////////////////////////////////////////////////////////
    ITexture* CreateTexture             (const TextureDesc&);
    ITexture* CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index);
    ITexture* CreateDepthStencilTexture (ISwapChain* swap_chain);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание фрейм буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    FrameBuffer* CreateFrameBuffer (NullView, NullView);
    FrameBuffer* CreateFrameBuffer (NullView, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (NullView, IBindableTexture*);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, NullView);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, IBindableTexture*);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, NullView);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, IBindableTexture*);
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Поиск совместимых теневых буферов вывода
///////////////////////////////////////////////////////////////////////////////////////////////////
    SwapChainColorBuffer*        FindShadowBuffer (SwapChainDepthStencilBuffer*);
    SwapChainDepthStencilBuffer* FindShadowBuffer (SwapChainColorBuffer*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработчики удаления теневых буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void RemoveColorBuffer        (SwapChainColorBuffer*);
    void RemoveDepthStencilBuffer (SwapChainDepthStencilBuffer*);
    
  private:
/*    struct ColorBufferHolder
    {
      SwapChainColorBuffer*     buffer;
      Trackable::DestroyHandler on_destroy;
      
      ColorBufferHolder (SwapChainColorBuffer* in_buffer, xtl::slot<void ()>& destroy_handler)
        : buffer (in_buffer), on_destroy (destroy_handler)
      {
        on_destroy
      }
    };*/
  
    typedef stl::list<SwapChainColorBuffer*>        ColorBufferList;
    typedef stl::list<SwapChainDepthStencilBuffer*> DepthStencilBufferList;

  private:
    ColorBufferList           shadow_color_buffers;
    DepthStencilBufferList    shadow_depth_stencil_buffers;
    Trackable::SlotType on_destroy_color_buffer;
    Trackable::SlotType on_destroy_depth_stencil_buffer;
};

/*
===================================================================================================
    SwapChainFrameBuffer
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

SwapChainFrameBuffer::SwapChainFrameBuffer
 (const ContextManager&        manager,
  SwapChainColorBuffer*        in_color_buffer,
  SwapChainDepthStencilBuffer* in_depth_stencil_buffer)
    : ContextObject (manager),
      color_buffer (in_color_buffer),
      depth_stencil_buffer (in_depth_stencil_buffer)
{
}

/*
    Установка буфера в контекст OpenGL
*/

void SwapChainFrameBuffer::Bind ()
{
  size_t      context_id  = depth_stencil_buffer ? depth_stencil_buffer->GetContextId () : 0;
  ISwapChain* swap_chain  = color_buffer ? color_buffer->GetSwapChain () : 0;
  GLenum      buffer_type = color_buffer ? color_buffer->GetBufferType () : GL_BACK;
  
    //установка активного контекста

  GetContextManager ().SetContext (context_id, swap_chain, swap_chain);
  
    //установка текущего контекста OpenGL

  MakeContextCurrent ();
  
    //установка текущего буфера чтения и отрисовки

  glReadBuffer (buffer_type);
  glDrawBuffer (buffer_type);
  
    //проверка ошибок

  CheckErrors ("render::low_level::opengl::SwapChainFrameBuffer::Bind");
}

/*
    Обновление целевых буферов отрисовки
*/

void SwapChainFrameBuffer::UpdateRenderTargets ()
{
  RaiseNotImplemented ("render::low_level::opengl::SwapChainDepthStencilBuffer::UpdateRenderTargets");
}

/*
===================================================================================================
    DefaultOutputStageResourceFactory
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

DefaultOutputStageResourceFactory::DefaultOutputStageResourceFactory (const ContextManager& manager)
  : ContextObject (manager)
{
}

DefaultOutputStageResourceFactory::~DefaultOutputStageResourceFactory ()
{
}

/*
    Поиск совместимых теневых буферов вывода
*/

/*SwapChainColorBuffer* DefaultOutputStageResourceFactory::FindShadowBuffer (SwapChainDepthStencilBuffer*)
{
}

SwapChainDepthStencilBuffer* DefaultOutputStageResourceFactory::FindShadowBuffer (SwapChainColorBuffer*)
{
} */

/*
    Создание целевого буфера вывода    
*/

ITexture* DefaultOutputStageResourceFactory::CreateTexture (const TextureDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateTexture");
  return 0;
}

ITexture* DefaultOutputStageResourceFactory::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return new SwapChainColorBuffer (GetContextManager (), swap_chain, buffer_index);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateRenderTargetTexture");

    throw;
  }
}

ITexture* DefaultOutputStageResourceFactory::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return new SwapChainDepthStencilBuffer (GetContextManager (), swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateDepthStencilTexture");

    throw;
  }
}

/*
    Создание фрейм буферов
*/

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (NullView, NullView)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), 0, 0);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(NullView,NullView)");
    
    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (NullView, SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), 0, depth_stencil_buffer);    
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(NullView,SwapChainDepthStencilBuffer*)");
    
    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (NullView, IBindableTexture*)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(NullView,IBindableTexture*)");
  return 0;
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, NullView)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer, 0);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(SwapChainColorBuffer*,NullView)");

    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer
 (SwapChainColorBuffer*        color_buffer,
  SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer, depth_stencil_buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(SwapChainColorBuffer*,SwapChainDepthStencilBuffer*)");

    throw;
  }
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (SwapChainColorBuffer*, IBindableTexture*)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(SwapChainColorBuffer*,IBindableTexture*)");
  return 0;
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (IBindableTexture*, NullView)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(IBindableTexture*,NullView)");
  return 0;
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (IBindableTexture*, SwapChainDepthStencilBuffer*)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(IBindableTexture*,SwapChainDepthStencilBuffer*)");
  return 0;
}

FrameBuffer* DefaultOutputStageResourceFactory::CreateFrameBuffer (IBindableTexture*, IBindableTexture*)
{
  RaiseNotImplemented ("render::low_level::opengl::DefaultOutputStageResourceFactory::CreateFrameBuffer(IBindableTexture*,IBindableTexture*)");
  return 0;
}

/*
    Создание фабрики ресурсов выходного уровня по умолчанию
*/

OutputStageResourceFactory* OutputStageResourceFactory::CreateDefaultFactory (const ContextManager& manager)
{
  return new DefaultOutputStageResourceFactory (manager);
}
