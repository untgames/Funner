#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

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

  xtl::com_ptr<ISwapChain> swap_chain (context_manager.CreateCompatibleSwapChain (depth_stencil_buffer->GetContextId ()), false);

  return ColorBufferPtr (new SwapChainColorBuffer (frame_buffer_manager, swap_chain.get (), 1), false);
}

DepthStencilBufferPtr SwapChainFrameBufferManager::CreateShadowDepthStencilBuffer (SwapChainColorBuffer* color_buffer)
{
  ContextManager& context_manager = frame_buffer_manager.GetContextManager ();

  return DepthStencilBufferPtr (new SwapChainDepthStencilBuffer (frame_buffer_manager, color_buffer->GetSwapChain ()), false);
}

namespace
{

template <class List> class list_remover
{
  public:
    typedef typename List::iterator Iter;  

    list_remover (List& in_list, Iter in_iter) : list (in_list), iter (in_iter) {}

    void operator () () const { list.erase (iter); }

  private:
    List& list;
    Iter  iter;
};

}

ColorBufferPtr SwapChainFrameBufferManager::GetShadowBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  ContextManager& manager    = frame_buffer_manager.GetContextManager ();
  size_t          context_id = depth_stencil_buffer->GetContextId ();
  
    //попытка найти совместимый по формату буфер цвета
  
  for (ColorBufferList::iterator iter=shadow_color_buffers.begin (), end=shadow_color_buffers.end (); iter!=end; ++iter)
    if (manager.IsCompatible (context_id, (*iter)->GetSwapChain ()))
    {
        //оптимизация: перемещение найденного узла в начало списка для ускорения повторного поиска

      shadow_color_buffers.splice (shadow_color_buffers.begin (), shadow_color_buffers, iter);

      return *iter;
    }

    //создание нового буфера цвета

  ColorBufferPtr color_buffer = CreateShadowColorBuffer (depth_stencil_buffer);
  
  shadow_color_buffers.push_front (color_buffer.get ());

  try
  {
    color_buffer->RegisterDestroyHandler (list_remover<ColorBufferList> (shadow_color_buffers, shadow_color_buffers.begin ()), GetTrackable ());
  }
  catch (...)
  {
    shadow_color_buffers.pop_front ();
    throw;
  }

  return color_buffer;
}

DepthStencilBufferPtr SwapChainFrameBufferManager::GetShadowBuffer (SwapChainColorBuffer* color_buffer)
{
  ContextManager& manager    = frame_buffer_manager.GetContextManager ();
  ISwapChain*     swap_chain = color_buffer->GetSwapChain ();

    //попытка найти совместимый по формату буфер глубина-трафарет        
    
  for (DepthStencilBufferList::iterator iter=shadow_depth_stencil_buffers.begin (), end=shadow_depth_stencil_buffers.end (); iter!=end; ++iter)
    if (manager.IsCompatible ((*iter)->GetContextId (), swap_chain))
    {
        //оптимизация: перемещение найденного узла в начало списка для ускорения повторного поиска

      shadow_depth_stencil_buffers.splice (shadow_depth_stencil_buffers.begin (), shadow_depth_stencil_buffers, iter);

      return *iter;
    }    

    //создание нового буфера глубина-трафарет
    
  DepthStencilBufferPtr depth_stencil_buffer = CreateShadowDepthStencilBuffer (color_buffer);  

  shadow_depth_stencil_buffers.push_front (depth_stencil_buffer.get ());  

  try
  {
    depth_stencil_buffer->RegisterDestroyHandler (list_remover<DepthStencilBufferList> (shadow_depth_stencil_buffers,
      shadow_depth_stencil_buffers.begin ()), GetTrackable ());
  }
  catch (...)
  {
    shadow_depth_stencil_buffers.pop_front ();
    throw;
  }

  return depth_stencil_buffer;
}

void SwapChainFrameBufferManager::GetShadowBuffers (ColorBufferPtr& color_buffer, DepthStencilBufferPtr& depth_stencil_buffer)
{
  ContextManager& manager = frame_buffer_manager.GetContextManager ();    

  if (!shadow_color_buffers.empty () && !shadow_depth_stencil_buffers.empty ())
  { 
    for (ColorBufferList::iterator i=shadow_color_buffers.begin (), end=shadow_color_buffers.end (); i!=end; ++i)
    {
      ISwapChain* swap_chain = (*i)->GetSwapChain ();
      
      for (DepthStencilBufferList::iterator j=shadow_depth_stencil_buffers.begin (), end=shadow_depth_stencil_buffers.end (); j!=end; ++j)
        if (manager.IsCompatible ((*j)->GetContextId (), swap_chain))
        {
          color_buffer         = *i;
          depth_stencil_buffer = *j;

            //оптимизация: перемещение найденных узлов в начало списка для ускорения повторного поиска

          shadow_color_buffers.splice (shadow_color_buffers.begin (), shadow_color_buffers, i);
          shadow_depth_stencil_buffers.splice (shadow_depth_stencil_buffers.begin (), shadow_depth_stencil_buffers, j);          
          
          return;
        }
    }
  }

  if (!shadow_color_buffers.empty ())
  {
    color_buffer         = shadow_color_buffers.front ();
    depth_stencil_buffer = GetShadowBuffer (color_buffer.get ());    
    
    return;
  }

  if (!shadow_depth_stencil_buffers.empty ())
  {
    depth_stencil_buffer = shadow_depth_stencil_buffers.front ();
    color_buffer         = GetShadowBuffer (depth_stencil_buffer.get ());
    
    return;
  }

    //создание новой пары буферов

  SwapChainPtr          swap_chain (manager.CreateCompatibleSwapChain (GetDefaultSwapChain ()), false);
  ColorBufferPtr        new_color_buffer (new SwapChainColorBuffer (frame_buffer_manager, swap_chain.get (), 1), false);  
  DepthStencilBufferPtr new_depth_stencil_buffer = GetShadowBuffer (new_color_buffer.get ());

  shadow_color_buffers.push_front (new_color_buffer.get ());

  try
  {
    new_color_buffer->RegisterDestroyHandler (list_remover<ColorBufferList> (shadow_color_buffers, shadow_color_buffers.begin ()), GetTrackable ());
  }
  catch (...)
  {
    shadow_color_buffers.pop_front ();
    throw;
  }

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
      catch (common::Exception& exception)
      {
        exception.Touch (METHOD_NAME);
        
        throw;
      }
    }
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    {
      try
      {
        ISwapChain* default_swap_chain = GetDefaultSwapChain ();

        return new SwapChainDepthStencilBuffer (frame_buffer_manager, default_swap_chain, desc);
      }
      catch (common::Exception& exception)
      {
        exception.Touch (METHOD_NAME);
        
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
      RaiseNotSupported (METHOD_NAME, "Can't create output-stage texture with format=%s", get_name (desc.format));
      return 0;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
      return 0;
  }
}

ITexture* SwapChainFrameBufferManager::CreateColorBuffer (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return new SwapChainColorBuffer (frame_buffer_manager, swap_chain, buffer_index);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainFrameBufferManager::CreateColorBuffer");

    throw;
  }
}

ITexture* SwapChainFrameBufferManager::CreateDepthStencilBuffer (ISwapChain* swap_chain)
{
  try
  {
    return new SwapChainDepthStencilBuffer (frame_buffer_manager, swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainFrameBufferManager::CreateDepthStencilBuffer");

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
    
    if (!dynamic_cast<IRenderTargetTexture*> (base_texture) && !dynamic_cast<SwapChainDepthStencilBuffer*> (base_texture))
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
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::SwapChainFrameBufferManager::CreateFrameBuffer");

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
