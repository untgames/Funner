#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

FrameBufferManager::FrameBufferManager (const ContextManager& manager, ISwapChain* in_default_swap_chain)
  : ContextObject (manager),
    default_swap_chain (in_default_swap_chain),
    EXT_framebuffer_object ("GL_EXT_framebuffer_object")
{
}

FrameBufferManager::~FrameBufferManager ()
{
}

/*
    Работа с теневыми буферами
*/

FrameBufferManager::ColorBufferPtr FrameBufferManager::CreateColorBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  xtl::com_ptr<ISwapChain> swap_chain (GetContextManager ().CreateCompatibleSwapChain (depth_stencil_buffer->GetContextId ()), false);

  return ColorBufferPtr (new SwapChainColorBuffer (GetContextManager (), swap_chain.get (), 1), false);
}

FrameBufferManager::DepthStencilBufferPtr FrameBufferManager::CreateDepthStencilBuffer (SwapChainColorBuffer* color_buffer)
{
  return DepthStencilBufferPtr (new SwapChainDepthStencilBuffer (GetContextManager (), color_buffer->GetSwapChain ()), false);
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

FrameBufferManager::ColorBufferPtr FrameBufferManager::GetShadowBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  ContextManager& manager    = GetContextManager ();
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

  ColorBufferPtr color_buffer = CreateColorBuffer (depth_stencil_buffer);
  
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

FrameBufferManager::DepthStencilBufferPtr FrameBufferManager::GetShadowBuffer (SwapChainColorBuffer* color_buffer)
{
  ContextManager& manager    = GetContextManager ();
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
    
  DepthStencilBufferPtr depth_stencil_buffer = CreateDepthStencilBuffer (color_buffer);
  
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

void FrameBufferManager::GetShadowBuffers (ColorBufferPtr& color_buffer, DepthStencilBufferPtr& depth_stencil_buffer)
{
  ContextManager& manager = GetContextManager ();

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

  SwapChainPtr          swap_chain (GetContextManager ().CreateCompatibleSwapChain (GetDefaultSwapChain ()), false);
  ColorBufferPtr        new_color_buffer (new SwapChainColorBuffer (manager, swap_chain.get (), 1), false);  
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

SwapChainFrameBuffer* FrameBufferManager::CreateShadowFrameBuffer ()
{
  try
  {
    ColorBufferPtr        color_buffer;
    DepthStencilBufferPtr depth_stencil_buffer;
    
    GetShadowBuffers (color_buffer, depth_stencil_buffer);
    
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer.get (), depth_stencil_buffer.get ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateShadowFrameBuffer");
    
    throw;
  }
}

/*
    Создание буфера рендеринга
*/

ITexture* FrameBufferManager::CreateSwapChainRenderBuffer (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::CreateSwapChainRenderBuffer";

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

        SwapChainPtr swap_chain (GetContextManager ().CreateCompatibleSwapChain (default_swap_chain, swap_chain_desc), false);

        return new SwapChainColorBuffer (GetContextManager (), swap_chain.get (), 1);
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

        return new SwapChainDepthStencilBuffer (GetContextManager (), default_swap_chain, desc.width, desc.height);
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
      RaiseNotSupported (METHOD_NAME, "Can not create output-stage texture with format=%s", get_name (desc.format));
      return 0;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
      return 0;
  }
}

ITexture* FrameBufferManager::CreateFboRenderBuffer (const TextureDesc& desc)
{
  try
  {
    return new FboRenderBuffer (GetContextManager (), desc.format, desc.width, desc.height);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFboRenderBuffer");

    throw;
  }
}

ITexture* FrameBufferManager::CreateRenderBuffer (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::CreateRenderBuffer";
  
  switch (desc.dimension)
  {
    case TextureDimension_2D:
      break;
    case TextureDimension_1D:
    case TextureDimension_3D:
    case TextureDimension_Cubemap:
      RaiseNotSupported (METHOD_NAME, "Can not create output-stage texture with dimension %s", get_name (desc.dimension));
      return 0;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.dimension", desc.dimension);
      return 0;
  }
  
  if (desc.layers > 1)
    RaiseNotSupported (METHOD_NAME, "Could not create output-stage texture with desc.layers=%u", desc.layers);

  if (desc.generate_mips_enable)
    RaiseNotSupported (METHOD_NAME, "Could not create output-stage texture with automatic mipmap generation");

  try
  {
      //выбор текущего контекста

    if (IsSupported (EXT_framebuffer_object)) return CreateFboRenderBuffer (desc);
    else                                      return CreateSwapChainRenderBuffer (desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch (METHOD_NAME);

    throw;
  }
}

ITexture* FrameBufferManager::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return new SwapChainColorBuffer (GetContextManager (), swap_chain, buffer_index);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateRenderTargetTexture");

    throw;
  }
}

ITexture* FrameBufferManager::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return new SwapChainDepthStencilBuffer (GetContextManager (), swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateDepthStencilTexture");

    throw;
  }
}

/*
    Диспетчеры создания буферов кадра
*/

template <class T>
FrameBuffer* FrameBufferManager::CreateFrameBufferImpl
 (const T&        render_target,
  const ViewDesc& render_target_desc,
  View*           depth_stencil_view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::CreateFrameBuffer";

  ViewType depth_stencil_type = depth_stencil_view ? depth_stencil_view->GetType () : ViewType_Null;
  
  ViewDesc depth_stencil_desc;
  
  if (depth_stencil_view)
  {
      //получение дескриптора отображения
    
    depth_stencil_view->GetDesc (depth_stencil_desc);
    
      //получение дескриптора текстуры

    TextureDesc texture_desc;
    
    depth_stencil_view->GetTexture ()->GetDesc (texture_desc);
    
      //проверка формата
      
    switch (texture_desc.format)
    {
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_S8:
        break;      
      default:
        RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view format=%s", get_name (texture_desc.format));
        break;
    }    
  }

  switch (depth_stencil_type)
  {
    case ViewType_Null:
    {
      ViewDesc null_view_desc;
      
      memset (&null_view_desc, 0, sizeof (null_view_desc));
      
      return CreateFrameBuffer (render_target, render_target_desc, NullView (), null_view_desc);
    }
    case ViewType_SwapChainColorBuffer:
      RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view type 'ViewType_SwapChainColorBuffer'");
      return 0;
    case ViewType_SwapChainDepthStencilBuffer:
      return CreateFrameBuffer (render_target, render_target_desc, depth_stencil_view->GetSwapChainDepthStencilBuffer (),
                                depth_stencil_desc);
    case ViewType_RenderTargetTexture:
      return CreateFrameBuffer (render_target, render_target_desc, depth_stencil_view->GetRenderTargetTexture (), depth_stencil_desc);
    case ViewType_FboRenderBuffer:
      return CreateFrameBuffer (render_target, render_target_desc, depth_stencil_view->GetFboRenderBuffer (), depth_stencil_desc);
    default:
      RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view type '%s'", typeid (depth_stencil_view->GetTexture ()).name ());
      return 0;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (View* render_target_view, View* depth_stencil_view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferManager::CreateFrameBuffer";

  ViewType render_target_type = render_target_view ? render_target_view->GetType () : ViewType_Null;

  ViewDesc render_target_desc;

  if (render_target_view)
  {
      //получение дескриптора отображения

    render_target_view->GetDesc (render_target_desc);    
    
      //получение дескриптора текстуры

    TextureDesc texture_desc;
    
    render_target_view->GetTexture ()->GetDesc (texture_desc);        
    
      //проверка формата
      
    switch (texture_desc.format)
    {
      case PixelFormat_RGB8:
      case PixelFormat_RGBA8:
      case PixelFormat_L8:
      case PixelFormat_A8:
      case PixelFormat_LA8:
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
        break;      
      default:
        RaiseNotSupported (METHOD_NAME, "Unsupported render-target view format=%s", get_name (texture_desc.format));
        break;
    }
  }

  switch (render_target_type)
  {
    case ViewType_Null:
    {
      ViewDesc null_view_desc;
      
      memset (&null_view_desc, 0, sizeof (null_view_desc));
      
      return CreateFrameBufferImpl (NullView (), null_view_desc, depth_stencil_view);
    }
    case ViewType_SwapChainColorBuffer:
      return CreateFrameBufferImpl (render_target_view->GetSwapChainColorBuffer (), render_target_desc, depth_stencil_view);
    case ViewType_SwapChainDepthStencilBuffer:
      RaiseNotSupported (METHOD_NAME, "Unsupported render-target view type 'ViewType_SwapChainDepthStencilBuffer'");
      return 0;
    case ViewType_RenderTargetTexture:
      return CreateFrameBufferImpl (render_target_view->GetRenderTargetTexture (), render_target_desc, depth_stencil_view);
    case ViewType_FboRenderBuffer:
      return CreateFrameBufferImpl (render_target_view->GetFboRenderBuffer (), render_target_desc, depth_stencil_view);
    default:
      RaiseNotSupported (METHOD_NAME, "Unsupported render-target view type '%s'", typeid (render_target_view->GetTexture ()).name ());
      return 0;
  }
}

/*
    Создание буферов кадра в различных конфигураций
*/

FrameBuffer* FrameBufferManager::CreateFrameBuffer (NullView, const ViewDesc&, NullView, const ViewDesc&)
{
  try
  {
    return new NullFrameBuffer (GetContextManager ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(NullView,NullView)");
    
    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (NullView, const ViewDesc&, SwapChainDepthStencilBuffer* depth_stencil_buffer, const ViewDesc&)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), GetShadowBuffer (depth_stencil_buffer).get (),
                                                                   depth_stencil_buffer);

    frame_buffer->SetBuffersState (false, true);
    
    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(NullView,SwapChainDepthStencilBuffer*)");
    
    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (NullView, const ViewDesc&, IRenderTargetTexture* texture, const ViewDesc& desc)
{
  try
  {
    if (IsSupported (EXT_framebuffer_object))
    {
      return new FboFrameBuffer (GetContextManager (), NullView (), texture, desc);
    }
    else
    {
      SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();

      frame_buffer->SetBuffersState  (false, true);
      frame_buffer->SetRenderTargets (0, 0, texture, &desc);

      return frame_buffer;
    }
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(NullView,IRenderTargetTexture*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (NullView, const ViewDesc&, FboRenderBuffer* render_buffer, const ViewDesc&)
{
  try
  {
    if (!IsSupported (EXT_framebuffer_object))
      RaiseNotSupported ("", "GL_EXT_framebuffer_object not supported");
    
    return new FboFrameBuffer (GetContextManager (), NullView (), render_buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(NullView,FboRenderBuffer*)");
    
    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, const ViewDesc&, NullView, const ViewDesc&)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), color_buffer,
                                                                   GetShadowBuffer (color_buffer).get ());
    
    frame_buffer->SetBuffersState (true, false);
    
    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(SwapChainColorBuffer*,NullView)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, const ViewDesc&, SwapChainDepthStencilBuffer* depth_stencil_buffer, const ViewDesc&)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer, depth_stencil_buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(SwapChainColorBuffer*,SwapChainDepthStencilBuffer*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, const ViewDesc&, IRenderTargetTexture* texture, const ViewDesc& desc)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), color_buffer,
                                                                   GetShadowBuffer (color_buffer).get ());

    frame_buffer->SetRenderTargets (0, 0, texture, &desc);

    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(SwapChainColorBuffer*,IRenderTargetTexture*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (SwapChainColorBuffer*, const ViewDesc&, FboRenderBuffer*, const ViewDesc&)
{
  RaiseNotSupported ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(SwapChainColorBuffer*,FboRenderBuffer*)",
                     "Incompatible configuration SwapChainColorBuffer with FboRenderBuffer");

  return 0;
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (IRenderTargetTexture* texture, const ViewDesc& desc, NullView, const ViewDesc&)
{
  try
  {
    if (IsSupported (EXT_framebuffer_object))
    {
      return new FboFrameBuffer (GetContextManager (), texture, desc, NullView ());
    }
    else
    {
      SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();

      frame_buffer->SetBuffersState  (true, false);
      frame_buffer->SetRenderTargets (texture, &desc, 0, 0);

      return frame_buffer;
    }
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(IRenderTargetTexture*,NullView)");
    
    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (IRenderTargetTexture* texture, const ViewDesc& desc, SwapChainDepthStencilBuffer* depth_stencil_buffer, const ViewDesc&)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = new SwapChainFrameBuffer (GetContextManager (), GetShadowBuffer (depth_stencil_buffer).get (),
                                                                   depth_stencil_buffer);

    frame_buffer->SetRenderTargets (texture, &desc, 0, 0);

    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(IRenderTargetTexture*,SwapChainDepthStencilBuffer*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer
 (IRenderTargetTexture* render_target_texture,
  const ViewDesc&   render_target_desc,
  IRenderTargetTexture* depth_stencil_texture,
  const ViewDesc&   depth_stencil_desc)
{
  try
  {
    if (IsSupported (EXT_framebuffer_object))
    {
      return new FboFrameBuffer (GetContextManager (), render_target_texture, render_target_desc, depth_stencil_texture,
                                 depth_stencil_desc);
    }
    else
    {
      SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();
      
      frame_buffer->SetRenderTargets (render_target_texture, &render_target_desc, depth_stencil_texture, &depth_stencil_desc);
      
      return frame_buffer;
    }
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(IRenderTargetTexture*,IRenderTargetTexture*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer
 (IRenderTargetTexture* render_target_texture,
  const ViewDesc&   render_target_desc,
  FboRenderBuffer*  depth_stencil_buffer,
  const ViewDesc&)
{
  try
  {
    if (!IsSupported (EXT_framebuffer_object))
      RaiseNotSupported ("", "GL_EXT_framebuffer_object not supported");    
    
    return new FboFrameBuffer (GetContextManager (), render_target_texture, render_target_desc, depth_stencil_buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(IRenderTargetTexture*,FboRenderBuffer*)");
    
    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (FboRenderBuffer* color_buffer, const ViewDesc&, NullView, const ViewDesc&)
{
  try
  {
    if (!IsSupported (EXT_framebuffer_object))
      RaiseNotSupported ("", "GL_EXT_framebuffer_object not supported");    
    
    return new FboFrameBuffer (GetContextManager (), color_buffer, NullView ());
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(FboRenderBuffer*,NullView)");
    
    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer (FboRenderBuffer*, const ViewDesc&, SwapChainDepthStencilBuffer*, const ViewDesc&)
{
  RaiseNotSupported ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(FboRenderBuffer*,SwapChainDepthStencilBuffer*)",
                     "Incompatible configuration FboRenderBuffer with SwapChainDepthStencilBuffer");

  return 0;
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer
 (FboRenderBuffer*  color_buffer,
  const ViewDesc&,
  IRenderTargetTexture* depth_stencil_texture,
  const ViewDesc&   depth_stencil_desc)
{
  try
  {
    if (!IsSupported (EXT_framebuffer_object))
      RaiseNotSupported ("", "GL_EXT_framebuffer_object not supported");    
    
    return new FboFrameBuffer (GetContextManager (), color_buffer, depth_stencil_texture, depth_stencil_desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(FboRenderBuffer*,IRenderTargetTexture*)");

    throw;
  }
}

FrameBuffer* FrameBufferManager::CreateFrameBuffer
 (FboRenderBuffer* color_buffer,
  const ViewDesc&,
  FboRenderBuffer* depth_stencil_buffer,
  const ViewDesc&)
{
  try
  {
    if (!IsSupported (EXT_framebuffer_object))
      RaiseNotSupported ("", "GL_EXT_framebuffer_object not supported");    
    
    return new FboFrameBuffer (GetContextManager (), color_buffer, depth_stencil_buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::FrameBufferManager::CreateFrameBuffer(FboRenderBuffer*,FboRenderBuffer*)");
    
    throw;
  }
}
