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
///Разрешение / запрещение буферов
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetBuffersState            (bool color_buffer_state, bool depth_stencil_buffer_state);
    bool GetColorBufferState        () const { return color_buffer_state; }
    bool GetDepthStencilBufferState () const { return depth_stencil_buffer_state; }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка буфера в контекст OpenGL
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind (bool&, bool&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с текстурами
///////////////////////////////////////////////////////////////////////////////////////////////////
    void              SetRenderTargets       (IBindableTexture*, const ViewDesc*, IBindableTexture*, const ViewDesc*);
    IBindableTexture* GetRenderTargetTexture () const { return render_target_texture.get (); }
    IBindableTexture* GetDepthStencilTexture () const { return depth_stencil_texture.get (); }
    const ViewDesc&   GetRenderTargetDesc    () const { return render_target_desc; }
    const ViewDesc&   GetDepthStencilDesc    () const { return depth_stencil_desc; }
    void              UpdateRenderTargets    ();

  private:
    typedef xtl::com_ptr<SwapChainColorBuffer>        ColorBufferPtr;
    typedef xtl::com_ptr<SwapChainDepthStencilBuffer> DepthStencilBufferPtr;
    typedef xtl::com_ptr<IBindableTexture>            TexturePtr;

  private:
    ColorBufferPtr        color_buffer;
    DepthStencilBufferPtr depth_stencil_buffer;
    TexturePtr            render_target_texture;
    TexturePtr            depth_stencil_texture;
    ViewDesc              render_target_desc;
    ViewDesc              depth_stencil_desc;    
    bool                  color_buffer_state;
    bool                  depth_stencil_buffer_state;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация фабрики ресурсов выходного подуровня OpenGL по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
class DefaultFrameBufferFactory: public FrameBufferFactory, public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    DefaultFrameBufferFactory  (const ContextManager& manager, ISwapChain* default_swap_chain);
    ~DefaultFrameBufferFactory ();
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение цепочки обмена по умолчанию
///////////////////////////////////////////////////////////////////////////////////////////////////
    ISwapChain* GetDefaultSwapChain () const { return default_swap_chain.get (); }
    
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
    FrameBuffer* CreateFrameBuffer (NullView, IBindableTexture*, const ViewDesc&);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, NullView);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (SwapChainColorBuffer*, IBindableTexture*, const ViewDesc&);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, const ViewDesc&, NullView);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, const ViewDesc&, SwapChainDepthStencilBuffer*);
    FrameBuffer* CreateFrameBuffer (IBindableTexture*, const ViewDesc&, IBindableTexture*, const ViewDesc&);
    
  private:
    typedef xtl::com_ptr<SwapChainColorBuffer>        ColorBufferPtr;
    typedef xtl::com_ptr<SwapChainDepthStencilBuffer> DepthStencilBufferPtr;
    
  private:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с теневыми буферами
///////////////////////////////////////////////////////////////////////////////////////////////////
    ColorBufferPtr        GetShadowBuffer          (SwapChainDepthStencilBuffer*);
    DepthStencilBufferPtr GetShadowBuffer          (SwapChainColorBuffer*);
    void                  GetShadowBuffers         (ColorBufferPtr&, DepthStencilBufferPtr&);
    ColorBufferPtr        CreateColorBuffer        (SwapChainDepthStencilBuffer*);
    DepthStencilBufferPtr CreateDepthStencilBuffer (SwapChainColorBuffer*);
    SwapChainFrameBuffer* CreateShadowFrameBuffer  ();

  private:
    typedef stl::list<SwapChainColorBuffer*>        ColorBufferList;
    typedef stl::list<SwapChainDepthStencilBuffer*> DepthStencilBufferList;
    typedef xtl::com_ptr<ISwapChain>                SwapChainPtr;

  private:
    SwapChainPtr           default_swap_chain;
    ColorBufferList        shadow_color_buffers;
    DepthStencilBufferList shadow_depth_stencil_buffers;
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
      depth_stencil_buffer (in_depth_stencil_buffer),
      color_buffer_state (true),
      depth_stencil_buffer_state (true)
{
  memset (&render_target_desc, 0, sizeof (render_target_desc));
  memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
}

/*
    Установка буфера в контекст OpenGL
*/

void SwapChainFrameBuffer::Bind (bool& out_color_buffer_state, bool& out_depth_stencil_buffer_state)
{
  size_t      context_id  = depth_stencil_buffer ? depth_stencil_buffer->GetContextId () : 0;
  ISwapChain* swap_chain  = color_buffer->GetSwapChain ();
  GLenum      buffer_type = color_buffer_state ? color_buffer->GetBufferType () : GL_NONE;
  
    //установка активного контекста

  GetContextManager ().SetContext (context_id, swap_chain, swap_chain);
  
    //установка текущего контекста OpenGL

  MakeContextCurrent ();
  
    //установка текущего буфера чтения и отрисовки    

  glReadBuffer (buffer_type);
  glDrawBuffer (buffer_type);
  
    //проверка ошибок

  CheckErrors ("render::low_level::opengl::SwapChainFrameBuffer::Bind");
  
    //установка флагов состояния буферов отрисовки
    
  out_color_buffer_state         = color_buffer_state;
  out_depth_stencil_buffer_state = depth_stencil_buffer_state;
}

/*
    Разрешение / запрещение буферов
*/

void SwapChainFrameBuffer::SetBuffersState (bool in_color_buffer_state, bool in_depth_stencil_buffer_state)
{
  color_buffer_state         = in_color_buffer_state;
  depth_stencil_buffer_state = in_depth_stencil_buffer_state;
}

/*
    Работа с текстурами
*/

void SwapChainFrameBuffer::SetRenderTargets
 (IBindableTexture* in_render_target_texture,
  const ViewDesc*   in_render_target_desc,
  IBindableTexture* in_depth_stencil_texture,
  const ViewDesc*   in_depth_stencil_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SwapChainFrameBuffer::SetRenderTargets";

  if (in_render_target_texture)
  {
    TextureDesc desc;
    
    in_render_target_texture->GetDesc (desc);
    
    switch (desc.format)
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
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_S8:
        RaiseNotSupported (METHOD_NAME, "Can not bind texture with format %s as render-target", get_name (desc.format));
        break;
      default:
        RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
        break;
    }
  }
  
  if (in_depth_stencil_texture)
  {
    TextureDesc desc;
    
    in_depth_stencil_texture->GetDesc (desc);
    
    switch (desc.format)
    {
      case PixelFormat_RGB8:
      case PixelFormat_RGBA8:
      case PixelFormat_L8:
      case PixelFormat_A8:
      case PixelFormat_LA8:
      case PixelFormat_DXT1:
      case PixelFormat_DXT3:
      case PixelFormat_DXT5:
        RaiseNotSupported (METHOD_NAME, "Can not bind texture with format %s as depth-stencil", get_name (desc.format));
        break;
      case PixelFormat_D16:
      case PixelFormat_D24X8:
      case PixelFormat_D24S8:
      case PixelFormat_S8:
        break;
      default:
        RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
        break;
    }
  }  

  render_target_texture = in_render_target_texture;
  depth_stencil_texture = in_depth_stencil_texture;    
  
  if (in_render_target_desc)
  {
    render_target_desc = *in_render_target_desc;
  }
  else
  {
    memset (&render_target_desc, 0, sizeof (render_target_desc));
  }
  
  if (in_depth_stencil_desc)
  {
    depth_stencil_desc = *in_depth_stencil_desc;
  }
  else
  {
    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
  }
}

namespace
{

void copy_image (size_t width, size_t height, const BindableTextureDesc& texture_desc, const ViewDesc& view_desc)
{
  glBindTexture (texture_desc.target, texture_desc.id);  
  
  switch (texture_desc.target)
  {
    case GL_TEXTURE_1D:
      glCopyTexSubImage1D (GL_TEXTURE_1D, view_desc.mip_level, 0, 0, 0, width);
      break;
    case GL_TEXTURE_2D:
      glCopyTexSubImage2D (GL_TEXTURE_2D, view_desc.mip_level, 0, 0, 0, 0, width, height);
      break;
    default:
      break;
  }
}

}

void SwapChainFrameBuffer::UpdateRenderTargets ()
{
    //данный метод вызывается в установленном контексте, потому не требует вызова MakeConextCurrent
    
    //получение информации о цепочке обмена
    
  SwapChainDesc swap_chain_desc;

  color_buffer->GetSwapChain ()->GetDesc (swap_chain_desc);
  
  size_t width  = swap_chain_desc.frame_buffer.width,
         height = swap_chain_desc.frame_buffer.height;

    //копирование текстуры из буфера цвета
    
  if (render_target_texture)
  {
    BindableTextureDesc bindable_desc;
    TextureDesc         texture_desc;

    render_target_texture->GetDesc (bindable_desc);
    render_target_texture->GetDesc (texture_desc);

    copy_image (width < texture_desc.width ? width : texture_desc.width,
                height < texture_desc.height ? height : texture_desc.height, bindable_desc, render_target_desc);
  }
  
    //копирование текстуры из буфера глубины
    
  if (depth_stencil_texture)
  {
    BindableTextureDesc bindable_desc;
    TextureDesc         texture_desc;

    depth_stencil_texture->GetDesc (bindable_desc);
    depth_stencil_texture->GetDesc (texture_desc);

    copy_image (width < texture_desc.width ? width : texture_desc.width,
                height < texture_desc.height ? height : texture_desc.height, bindable_desc, depth_stencil_desc);
  }
  
  CheckErrors ("render::low_level::opengl::SwapChainFrameBuffer::UpdateRenderTargets");
}

/*
===================================================================================================
    DefaultFrameBufferFactory
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

DefaultFrameBufferFactory::DefaultFrameBufferFactory (const ContextManager& manager, ISwapChain* in_default_swap_chain)
  : ContextObject (manager), default_swap_chain (in_default_swap_chain)
{
}

DefaultFrameBufferFactory::~DefaultFrameBufferFactory ()
{
}

/*
    Работа с теневыми буферами
*/

DefaultFrameBufferFactory::ColorBufferPtr DefaultFrameBufferFactory::CreateColorBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  xtl::com_ptr<ISwapChain> swap_chain (GetContextManager ().CreateCompatibleSwapChain (depth_stencil_buffer->GetContextId ()), false);

  return ColorBufferPtr (new SwapChainColorBuffer (GetContextManager (), swap_chain.get (), 1), false);
}

DefaultFrameBufferFactory::DepthStencilBufferPtr DefaultFrameBufferFactory::CreateDepthStencilBuffer (SwapChainColorBuffer* color_buffer)
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

DefaultFrameBufferFactory::ColorBufferPtr DefaultFrameBufferFactory::GetShadowBuffer (SwapChainDepthStencilBuffer* depth_stencil_buffer)
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
    color_buffer->RegisterDestroyHandler (list_remover<ColorBufferList> (shadow_color_buffers, shadow_color_buffers.begin ()), *this);
  }
  catch (...)
  {
    shadow_color_buffers.pop_front ();
    throw;
  }

  return color_buffer;
}

DefaultFrameBufferFactory::DepthStencilBufferPtr DefaultFrameBufferFactory::GetShadowBuffer (SwapChainColorBuffer* color_buffer)
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
      shadow_depth_stencil_buffers.begin ()), *this);
  }
  catch (...)
  {
    shadow_depth_stencil_buffers.pop_front ();
    throw;
  }

  return depth_stencil_buffer;
}

void DefaultFrameBufferFactory::GetShadowBuffers (ColorBufferPtr& color_buffer, DepthStencilBufferPtr& depth_stencil_buffer)
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
    new_color_buffer->RegisterDestroyHandler (list_remover<ColorBufferList> (shadow_color_buffers, shadow_color_buffers.begin ()), *this);
  }
  catch (...)
  {
    shadow_color_buffers.pop_front ();
    throw;
  }

  color_buffer         = new_color_buffer;
  depth_stencil_buffer = new_depth_stencil_buffer;
}

SwapChainFrameBuffer* DefaultFrameBufferFactory::CreateShadowFrameBuffer ()
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
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateShadowFrameBuffer");
    
    throw;
  }
}

/*
    Создание целевого буфера вывода    
*/

ITexture* DefaultFrameBufferFactory::CreateTexture (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::DefaultFrameBufferFactory::CreateTexture";
  
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

  if (desc.generate_mips_enable)
    RaiseNotSupported (METHOD_NAME, "Can not create output-stage texture with automatic mipmap generation");

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

ITexture* DefaultFrameBufferFactory::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return new SwapChainColorBuffer (GetContextManager (), swap_chain, buffer_index);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateRenderTargetTexture");

    throw;
  }
}

ITexture* DefaultFrameBufferFactory::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return new SwapChainDepthStencilBuffer (GetContextManager (), swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateDepthStencilTexture");

    throw;
  }
}

/*
    Создание фрейм буферов
*/

FrameBuffer* DefaultFrameBufferFactory::CreateFrameBuffer (NullView, NullView)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();
    
    frame_buffer->SetBuffersState (false, false);
    
    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateFrameBuffer(NullView,NullView)");
    
    throw;
  }
}

FrameBuffer* DefaultFrameBufferFactory::CreateFrameBuffer (NullView, SwapChainDepthStencilBuffer* depth_stencil_buffer)
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
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateFrameBuffer(NullView,SwapChainDepthStencilBuffer*)");
    
    throw;
  }
}

FrameBuffer* DefaultFrameBufferFactory::CreateFrameBuffer (NullView, IBindableTexture* texture, const ViewDesc& desc)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();

    frame_buffer->SetBuffersState  (false, true);
    frame_buffer->SetRenderTargets (0, 0, texture, &desc);

    return frame_buffer;    
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateFrameBuffer(NullView,IBindableTexture*)");

    throw;
  }
}

FrameBuffer* DefaultFrameBufferFactory::CreateFrameBuffer (SwapChainColorBuffer* color_buffer, NullView)
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
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateFrameBuffer(SwapChainColorBuffer*,NullView)");

    throw;
  }
}

FrameBuffer* DefaultFrameBufferFactory::CreateFrameBuffer
 (SwapChainColorBuffer*        color_buffer,
  SwapChainDepthStencilBuffer* depth_stencil_buffer)
{
  try
  {
    return new SwapChainFrameBuffer (GetContextManager (), color_buffer, depth_stencil_buffer);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateFrameBuffer(SwapChainColorBuffer*,SwapChainDepthStencilBuffer*)");

    throw;
  }
}

FrameBuffer* DefaultFrameBufferFactory::CreateFrameBuffer
 (SwapChainColorBuffer* color_buffer,
  IBindableTexture*     texture,
  const ViewDesc&       desc)
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
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateFrameBuffer(SwapChainColorBuffer*,IBindableTexture*)");

    throw;
  }
}

FrameBuffer* DefaultFrameBufferFactory::CreateFrameBuffer (IBindableTexture* texture, const ViewDesc& desc, NullView)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();

    frame_buffer->SetBuffersState  (true, false);
    frame_buffer->SetRenderTargets (texture, &desc, 0, 0);

    return frame_buffer;    
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateFrameBuffer(IBindableTexture*,NullView)");
    
    throw;
  }
}

FrameBuffer* DefaultFrameBufferFactory::CreateFrameBuffer
 (IBindableTexture*            texture,
  const ViewDesc&              desc,
  SwapChainDepthStencilBuffer* depth_stencil_buffer)
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
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateFrameBuffer(IBindableTexture*,SwapChainDepthStencilBuffer*)");

    throw;
  }
}

FrameBuffer* DefaultFrameBufferFactory::CreateFrameBuffer
 (IBindableTexture* render_target_texture,
  const ViewDesc&   render_target_desc,
  IBindableTexture* depth_stencil_texture,
  const ViewDesc&   depth_stencil_desc)
{
  try
  {
    SwapChainFrameBuffer* frame_buffer = CreateShadowFrameBuffer ();
    
    frame_buffer->SetRenderTargets (render_target_texture, &render_target_desc, depth_stencil_texture, &depth_stencil_desc);
    
    return frame_buffer;
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::DefaultFrameBufferFactory::CreateFrameBuffer(IBindableTexture*,IBindableTexture*)");

    throw;
  }
}

/*
    Создание фабрики ресурсов выходного уровня по умолчанию
*/

FrameBufferFactory* FrameBufferFactory::CreateDefaultFactory (const ContextManager& manager, ISwapChain* default_swap_chain)
{
  return new DefaultFrameBufferFactory (manager, default_swap_chain);
}
