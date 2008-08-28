#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

typedef xtl::com_ptr<ISwapChain> SwapChainPtr;

/*
    Конструктор / деструктор
*/

SwapChainFrameBufferManager::SwapChainFrameBufferManager (const FrameBufferManager& manager)
  : frame_buffer_manager (manager)

{
}

SwapChainFrameBufferManager::~SwapChainFrameBufferManager ()
{
}

/*
    Работа с теневыми буферами
*/

ColorBufferPtr SwapChainFrameBufferManager::CreateShadowColorBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  ContextManager& context_manager = frame_buffer_manager.GetContextManager ();

  xtl::com_ptr<ISwapChain> swap_chain (context_manager.CreateCompatibleSwapChain (depth_stencil_buffer->GetSwapChain ()), false);

  return ColorBufferPtr (new SwapChainColorBuffer (frame_buffer_manager, swap_chain.get (), 1), false);
}

DepthStencilBufferPtr SwapChainFrameBufferManager::CreateShadowDepthStencilBuffer (SwapChainColorBuffer* color_buffer)
{
  ContextManager& context_manager = frame_buffer_manager.GetContextManager ();

  return DepthStencilBufferPtr (new SwapChainDepthStencilBuffer (frame_buffer_manager, color_buffer->GetSwapChain ()), false);
}

ColorBufferPtr SwapChainFrameBufferManager::GetShadowBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
    //попытка вернуть существующий теневой буфер цвета
    
  if (shadow_color_buffer)
    return shadow_color_buffer.get ();

    //создание нового буфера цвета

  ColorBufferPtr color_buffer = CreateShadowColorBuffer (depth_stencil_buffer);

  shadow_color_buffer = color_buffer.get ();
  
  return color_buffer;
}

DepthStencilBufferPtr SwapChainFrameBufferManager::GetShadowBuffer (SwapChainColorBuffer* color_buffer)
{
    //попытка вернуть существующий теневой буфер попиксельного отсечения
    
  if (shadow_depth_stencil_buffer)
    return shadow_depth_stencil_buffer.get ();

    //создание нового буфера попиксельного отсечения

  DepthStencilBufferPtr depth_stencil_buffer = CreateShadowDepthStencilBuffer (color_buffer);
  
  shadow_depth_stencil_buffer = depth_stencil_buffer.get ();
  
  return depth_stencil_buffer;
}

void SwapChainFrameBufferManager::GetShadowBuffers (ColorBufferPtr& color_buffer, DepthStencilBufferPtr& depth_stencil_buffer)
{
    //попытка вернуть существующие буферы
    
  if (shadow_depth_stencil_buffer && shadow_color_buffer)
  {
    color_buffer         = shadow_color_buffer.get ();
    depth_stencil_buffer = shadow_depth_stencil_buffer.get ();

    return;
  }
      
    //создание буфера цвета

  if (!shadow_color_buffer && shadow_depth_stencil_buffer)
  {
    DepthStencilBufferPtr new_depth_stencil_buffer = shadow_depth_stencil_buffer.get ();
    ColorBufferPtr        new_color_buffer         = GetShadowBuffer (new_depth_stencil_buffer.get ());

    color_buffer         = new_color_buffer;
    depth_stencil_buffer = new_depth_stencil_buffer;
    
    return;
  }

    //создание буфера попиксельного отсечения

  if (shadow_color_buffer && !shadow_depth_stencil_buffer)
  {
    ColorBufferPtr        new_color_buffer         = shadow_color_buffer.get ();
    DepthStencilBufferPtr new_depth_stencil_buffer = GetShadowBuffer (new_color_buffer.get ());
    
    color_buffer         = new_color_buffer;
    depth_stencil_buffer = new_depth_stencil_buffer;    
    
    return;
  }

    //если оба буфера отсутствуют - создание PBuffer-а

  SwapChainPtr swap_chain (frame_buffer_manager.GetContextManager ().CreateCompatibleSwapChain (GetDefaultSwapChain ()), false);

    //создание теневого буфера цвета

  ColorBufferPtr new_color_buffer (new SwapChainColorBuffer (frame_buffer_manager, swap_chain.get (), 1), false);  

  shadow_color_buffer = new_color_buffer.get ();    

    //создание теневого буфера попиксельного отсечения

  DepthStencilBufferPtr new_depth_stencil_buffer = GetShadowBuffer (new_color_buffer.get ());

    //возврат значений

  color_buffer         = new_color_buffer;
  depth_stencil_buffer = new_depth_stencil_buffer;
}

/*
    Создание буферов рендеринга
*/

ITexture* SwapChainFrameBufferManager::CreateRenderBuffer (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBufferManager::CreateRenderBuffer";
  
  ContextManager& context_manager = frame_buffer_manager.GetContextManager ();

  switch (desc.format)
  {
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    {
      try
      {
        SwapChainDesc swap_chain_desc;
        ISwapChain*   default_swap_chain = GetDefaultSwapChain ();

        default_swap_chain->GetDesc (swap_chain_desc);

        swap_chain_desc.frame_buffer.width  = desc.width;
        swap_chain_desc.frame_buffer.height = desc.height;

        SwapChainPtr swap_chain (context_manager.CreateCompatibleSwapChain (default_swap_chain, swap_chain_desc), false);

        return new SwapChainColorBuffer (frame_buffer_manager, swap_chain.get (), desc);
      }
      catch (xtl::exception& exception)
      {
        exception.touch (METHOD_NAME);
        
        throw;
      }
    }
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    {
      try
      {
        return new SwapChainFakeDepthStencilBuffer (frame_buffer_manager.GetContextManager (), desc);
      }
      catch (xtl::exception& exception)
      {
        exception.touch (METHOD_NAME);

        throw;
      }
    }
    case PixelFormat_S8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create output-stage texture with format=%s", get_name (desc.format));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.format", desc.format);
  }
}

ITexture* SwapChainFrameBufferManager::CreateColorBuffer (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return new SwapChainColorBuffer (frame_buffer_manager, swap_chain, buffer_index);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBufferManager::CreateColorBuffer");

    throw;
  }
}

ITexture* SwapChainFrameBufferManager::CreateDepthStencilBuffer (ISwapChain* swap_chain)
{
  try
  {
    return new SwapChainDepthStencilBuffer (frame_buffer_manager, swap_chain);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBufferManager::CreateDepthStencilBuffer");

    throw;
  }
}

/*
    Создание буферов кадра
*/

//проверка возможности создания буфера кадра
bool SwapChainFrameBufferManager::IsSupported (View* color_view, View* depth_stencil_view)
{
    //проверка совместимости цветового отображения

  if (color_view)
  {
    ITexture* base_texture = color_view->GetTexture ();
    
    if (!dynamic_cast<IRenderTargetTexture*> (base_texture) && !dynamic_cast<SwapChainColorBuffer*> (base_texture))
      return false;
  }

    //проверка совместимости отображения буфера попиксельного отсечения
    
  if (depth_stencil_view)
  {
    ITexture* base_texture = depth_stencil_view->GetTexture ();
    
    if (!dynamic_cast<IRenderTargetTexture*> (base_texture) && !dynamic_cast<SwapChainDepthStencilBuffer*> (base_texture) &&
        !dynamic_cast<SwapChainFakeDepthStencilBuffer*> (base_texture))
      return false;
  }

  return true;  
}

//создание буфера кадра
IFrameBuffer* SwapChainFrameBufferManager::CreateFrameBuffer (View* color_view, View* depth_stencil_view)
{
  try
  {
    if (!color_view && !depth_stencil_view)
      return new SwapChainNullFrameBuffer (GetFrameBufferManager ());

    return new SwapChainFrameBuffer (*this, color_view, depth_stencil_view);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::SwapChainFrameBufferManager::CreateFrameBuffer");

    throw;
  }  
}

/*
    Получение цепочки обмена по умолчанию
*/

ISwapChain* SwapChainFrameBufferManager::GetDefaultSwapChain () const
{
  return frame_buffer_manager.GetDefaultSwapChain ();
}

/*
    Регистрация обработчиков создания буферов рендеринга и буферов кадра
*/

namespace render
{

namespace low_level
{

namespace opengl
{

void register_swap_chain_manager (FrameBufferManager& frame_buffer_manager)
{
  xtl::com_ptr<SwapChainFrameBufferManager> manager (new SwapChainFrameBufferManager (frame_buffer_manager), false);

    //регистрация обработчиков создания буферов рендеринга

  frame_buffer_manager.RegisterCreater (ColorBufferCreater (xtl::bind (&SwapChainFrameBufferManager::CreateColorBuffer, manager, _1, _2)));
  frame_buffer_manager.RegisterCreater (DepthStencilBufferCreater (xtl::bind (&SwapChainFrameBufferManager::CreateDepthStencilBuffer, manager, _1)));
  frame_buffer_manager.RegisterCreater (RenderBufferCreater (xtl::bind (&SwapChainFrameBufferManager::CreateRenderBuffer, manager, _1)));

    //регистрация обработчиков создания буферов кадра

  frame_buffer_manager.RegisterCreater (&SwapChainFrameBufferManager::IsSupported, xtl::bind (&SwapChainFrameBufferManager::CreateFrameBuffer, manager, _1, _2));  
}

}

}

}
