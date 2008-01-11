#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    Хранилище для буфера кадра
*/

struct FrameBufferHolder: public Trackable, public xtl::reference_counter
{
  View*        render_target_view; //целевое отображение буфера цвета
  View*        depth_stencil_view; //целевое отображение буфера глубина-трафарет
  FrameBuffer* frame_buffer;       //буфер кадра

  FrameBufferHolder (View* in_render_target_view, View* in_depth_stencil_view, FrameBufferFactory* resource_factory) :
    render_target_view (in_render_target_view),
    depth_stencil_view (in_depth_stencil_view),
    frame_buffer       (CreateFrameBuffer (in_render_target_view, in_depth_stencil_view, resource_factory))
  {
  }
  
  private:
    template <class T>
    static FrameBuffer* CreateFrameBufferImpl
      (const T&                    render_target,
       View*                       depth_stencil_view,
       FrameBufferFactory* resource_factory)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferHolder::CreateFrameBuffer";

      ViewType depth_stencil_type = depth_stencil_view ? depth_stencil_view->GetType () : ViewType_Null;

      switch (depth_stencil_type)
      {
        case ViewType_Null:
          return resource_factory->CreateFrameBuffer (render_target, NullView ());
        case ViewType_SwapChainColorBuffer:
          RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view type 'ViewType_SwapChainColorBuffer'");
          return 0;
        case ViewType_SwapChainDepthStencilBuffer:
          return resource_factory->CreateFrameBuffer (render_target, depth_stencil_view->GetSwapChainDepthStencilBuffer ());
        case ViewType_Texture:
        {
          ViewDesc desc;
          
          depth_stencil_view->GetDesc (desc);
          
          return resource_factory->CreateFrameBuffer (render_target, depth_stencil_view->GetBindableTexture (), desc);
        }
        default:
          RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view type '%s'", typeid (depth_stencil_view->GetTexture ()).name ());
          return 0;
      }
    }  
  
    template <class T>
    static FrameBuffer* CreateFrameBufferImpl
      (const T&                    render_target,
       const ViewDesc&             render_target_desc,
       View*                       depth_stencil_view,
       FrameBufferFactory* resource_factory)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferHolder::CreateFrameBuffer";

      ViewType depth_stencil_type = depth_stencil_view ? depth_stencil_view->GetType () : ViewType_Null;

      switch (depth_stencil_type)
      {
        case ViewType_Null:
          return resource_factory->CreateFrameBuffer (render_target, render_target_desc, NullView ());
        case ViewType_SwapChainColorBuffer:
          RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view type 'ViewType_SwapChainColorBuffer'");
          return 0;
        case ViewType_SwapChainDepthStencilBuffer:
          return resource_factory->CreateFrameBuffer (render_target, render_target_desc, depth_stencil_view->GetSwapChainDepthStencilBuffer ());
        case ViewType_Texture:
        {
          ViewDesc desc;
          
          depth_stencil_view->GetDesc (desc);
          
          return resource_factory->CreateFrameBuffer (render_target, render_target_desc, depth_stencil_view->GetBindableTexture (), desc);
        }
        default:
          RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view type '%s'", typeid (depth_stencil_view->GetTexture ()).name ());
          return 0;
      }
    }
  
    static FrameBuffer* CreateFrameBuffer (View* render_target_view, View* depth_stencil_view, FrameBufferFactory* resource_factory)
    {
      static const char* METHOD_NAME = "render::low_level::opengl::FrameBufferHolder::CreateFrameBuffer";
      
      ViewType render_target_type = render_target_view ? render_target_view->GetType () : ViewType_Null;

      switch (render_target_type)
      {
        case ViewType_Null:
          return CreateFrameBufferImpl (NullView (), depth_stencil_view, resource_factory);
        case ViewType_SwapChainColorBuffer:
          return CreateFrameBufferImpl (render_target_view->GetSwapChainColorBuffer (), depth_stencil_view, resource_factory);
        case ViewType_SwapChainDepthStencilBuffer:
          RaiseNotSupported (METHOD_NAME, "Unsupported render-target view type 'ViewType_SwapChainDepthStencilBuffer'");
          return 0;
        case ViewType_Texture:
        {
          ViewDesc render_target_desc;
          
          render_target_view->GetDesc (render_target_desc);
          
          return CreateFrameBufferImpl (render_target_view->GetBindableTexture (), render_target_desc, depth_stencil_view, resource_factory);
        }
        default:
          RaiseNotSupported (METHOD_NAME, "Unsupported render-target view type '%s'", typeid (render_target_view->GetTexture ()).name ());
          return 0;
      }
    }
};

/*
    Описание состояния выходного уровня конвейера OpenGL
*/

class OutputStageState
{
  public:  
      //конструктор
    OutputStageState () : stencil_reference (0) {}

      //установка текущего состояния подуровня смешивания цветов
    void SetBlendState (BlendState* state)
    {
      if (state == blend_state)
        return;

      blend_state = state;
    }

      //получение текущего состояния подуровня смешивания цветов
    BlendState* GetBlendState () const { return blend_state.get (); }
    
      //установка текущего состояния подуровня попиксельного отсечения
    void SetDepthStencilState (DepthStencilState* state)
    {
      if (state == depth_stencil_state)
        return;
        
      depth_stencil_state = state;            
    }

      //получение текущего состояния подуровня попиксельного отсечения
    DepthStencilState* GetDepthStencilState () const { return depth_stencil_state.get (); }
    
      //установка значения ссылки трафарета
    void SetStencilReference (size_t reference)
    {
      stencil_reference = reference;
    }

      //получение значения ссылки трафарета
    size_t GetStencilReference () const { return stencil_reference; }

      //установка текущего хранилища буфера кадра
    void SetFrameBufferHolder (FrameBufferHolder* in_frame_buffer_holder)
    {
      if (frame_buffer_holder == in_frame_buffer_holder)
        return;        

      frame_buffer_holder = in_frame_buffer_holder;
    }
    
      //получение текущего хранилища буфера кадра
    FrameBufferHolder* GetFrameBufferHolder () const { return frame_buffer_holder.get (); }
    
      //получение текущего буфера кадра
    FrameBuffer* GetFrameBuffer () const { return frame_buffer_holder ? frame_buffer_holder->frame_buffer : 0; }

      //получение текущего отображения буфера цвета
    View* GetRenderTargetView () const { return frame_buffer_holder ? frame_buffer_holder->render_target_view : 0; }
    
      //получение текущего отображения буфера глубина-трафарет
    View* GetDepthStencilView () const { return frame_buffer_holder ? frame_buffer_holder->depth_stencil_view : 0; }        

  private:
    OutputStageState (const OutputStageState&); //no impl        

  private:    
    typedef xtl::trackable_ptr<FrameBufferHolder> FrameBufferHolderPtr;
    typedef xtl::trackable_ptr<BlendState>        BlendStatePtr;
    typedef xtl::trackable_ptr<DepthStencilState> DepthStencilStatePtr;

  private:
    BlendStatePtr        blend_state;         //текущее состояние подуровня смешивания цветов
    DepthStencilStatePtr depth_stencil_state; //текущее состояние подуровня попиксельного отсечения
    size_t               stencil_reference;   //текущее значение трафарета
    FrameBufferHolderPtr frame_buffer_holder; //текущее хранилище буфера кадра
};

}

/*
    Описание реализации выходного уровня конвейера OpenGL
*/

typedef xtl::intrusive_ptr<FrameBufferHolder> FrameBufferHolderPtr;
typedef xtl::com_ptr<View>                    ViewPtr;
typedef stl::list<FrameBufferHolderPtr>       FrameBufferHolderList;
typedef xtl::com_ptr<ISwapChain>              SwapChainPtr;
typedef xtl::com_ptr<BlendState>              BlendStatePtr;
typedef xtl::com_ptr<DepthStencilState>       DepthStencilStatePtr;

struct OutputStage::Impl: public ContextObject
{
  OutputStageState                   state;                       //состояние уровня
  stl::auto_ptr<FrameBufferFactory>  default_resource_factory;    //фабрика ресурсов по умолчанию
  FrameBufferFactory*                resource_factory;            //выбранная фабрика ресурсов
  FrameBufferHolderList              frame_buffers;               //буферы кадра
  ViewPtr                            default_render_target_view;  //отображение буфера цвета по умолчанию
  ViewPtr                            default_depth_stencil_view;  //отображение буфера глубина-трафарет по умолчанию
  SwapChainPtr                       default_swap_chain;          //цепочка обмена по умолчанию
  BlendStatePtr                      default_blend_state;         //состояние подуровня смешивания цветов по умолчанию
  BlendStatePtr                      null_blend_state;            //состояние подуровня смешивания цветов соотв. отключению подуровня
  DepthStencilStatePtr               default_depth_stencil_state; //состояние подуровня попиксельного отсечения по умолчанию
  DepthStencilStatePtr               null_depth_stencil_state;    //состояние подуровня попиксельного отсечения соотв. отключению подуровня

  Impl (ContextManager& context_manager, ISwapChain* swap_chain) :
    ContextObject (context_manager),
    default_resource_factory (FrameBufferFactory::CreateDefaultFactory (context_manager, swap_chain)),
    resource_factory (default_resource_factory.get ()),
    default_swap_chain (swap_chain)
  {
      //инициализация отображений
      
    xtl::com_ptr<ITexture> color_texture (resource_factory->CreateRenderTargetTexture (swap_chain, 1), false),
                           depth_stencil_texture (resource_factory->CreateDepthStencilTexture (swap_chain), false);

    ViewDesc view_desc;

    memset (&view_desc, 0, sizeof (view_desc));

    default_render_target_view = ViewPtr (CreateView (color_texture.get (), view_desc), false);
    default_depth_stencil_view = ViewPtr (CreateView (depth_stencil_texture.get (), view_desc), false);

    SetRenderTargets (default_render_target_view.get (), default_depth_stencil_view.get ());
    
      //установка текущего контекста      
      
    if (state.GetFrameBuffer ())
    {
      bool buffer_state [2];
      
      state.GetFrameBuffer ()->Bind (buffer_state [0], buffer_state [1]);
    }
      
      //инициализация BlendState
        
    BlendDesc blend_desc;
    
    memset (&blend_desc, 0, sizeof (blend_desc));

    blend_desc.blend_enable     = false;
    blend_desc.color_write_mask = ColorWriteFlag_All;

    default_blend_state = BlendStatePtr (new BlendState (GetContextManager (), blend_desc), false);
    
    blend_desc.color_write_mask = 0;
    
    null_blend_state = BlendStatePtr (new BlendState (GetContextManager (), blend_desc), false);
    
      //инициализация DepthStencilState
      
    DepthStencilDesc depth_stencil_desc;
    
    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
    
    depth_stencil_desc.depth_test_enable  = true;
    depth_stencil_desc.depth_write_enable = true;
    depth_stencil_desc.depth_compare_mode = CompareMode_Less;
    
    default_depth_stencil_state = DepthStencilStatePtr (new DepthStencilState (GetContextManager (), depth_stencil_desc), false);
    
    depth_stencil_desc.depth_test_enable  = false;
    depth_stencil_desc.depth_write_enable = false;
    depth_stencil_desc.depth_compare_mode = CompareMode_AlwaysPass;
    
    null_depth_stencil_state = DepthStencilStatePtr (new DepthStencilState (GetContextManager (), depth_stencil_desc), false);
    
      //установка состояния по умолчанию

    SetBlendState (&*default_blend_state);
    SetDepthStencilState (&*default_depth_stencil_state);
  }    
  
    //получение буфера кадра по отображениям
  FrameBufferHolder* GetFrameBufferHolder (View* render_target_view, View* depth_stencil_view)
  {
      //поиск буфера в списке уже созданных
    
    for (FrameBufferHolderList::iterator iter=frame_buffers.begin (), end=frame_buffers.end (); iter!=end; ++iter)
      if ((*iter)->render_target_view == render_target_view && (*iter)->depth_stencil_view == depth_stencil_view)
      {
          //оптимизация: перемещение найденного узла в начало списка для ускорения повторного поиска

        frame_buffers.splice (frame_buffers.begin (), frame_buffers, iter);

        return &**iter;
      }
      
      //создание нового буфера кадра
      
    FrameBufferHolderPtr frame_buffer_holder (new FrameBufferHolder (render_target_view, depth_stencil_view, resource_factory), false);  

      //добавление нового буфера кадра в список созданных

    frame_buffers.push_front (frame_buffer_holder);

    return &*frame_buffers.front ();
  }
  
    //добавление отображения
  void AddView (const xtl::com_ptr<View>& view)
  {
    RegisterDestroyHandler (xtl::bind (&Impl::RemoveView, this, view.get ()), *this);
  }

    //удаления отображения
  void RemoveView (View* view)
  {
      //удаление всех буферов кадра, в которых присутствует указанное отображение
    
    for (FrameBufferHolderList::iterator iter=frame_buffers.begin (), end=frame_buffers.end (); iter!=end;)
    {
      if ((*iter)->render_target_view == view || (*iter)->depth_stencil_view == view)
      {
        FrameBufferHolderList::iterator tmp = iter;
        
        ++tmp;
        
        frame_buffers.erase (iter);
        
        iter = tmp;
      }
      else ++iter;
    }
  }
  
    //создание отображения
  View* CreateView (ITexture* texture, const ViewDesc& desc)
  {
    xtl::com_ptr<View> view (new View (texture, desc), false);        

    AddView (view);

    view->AddRef ();

    return view.get ();
  }
  
    //установка текущих отображений
  void SetRenderTargets (IView* in_render_target_view, IView* in_depth_stencil_view)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::OutputStage::Impl::SetRenderTargets";
    
      //приведение типов отображений

    View *render_target_view = cast_object<View> (in_render_target_view, METHOD_NAME, "render_target_view"),
         *depth_stencil_view = cast_object<View> (in_depth_stencil_view, METHOD_NAME, "depth_stencil_view");

      //получение хранилища буфера кадра, связанного с указанными отображениями

    FrameBufferHolder* frame_buffer_holder = GetFrameBufferHolder (render_target_view, depth_stencil_view);
    
      //обновление целевых буферов отрисовки
      
    if (state.GetFrameBuffer ())
    {
      try
      {
        state.GetFrameBuffer ()->UpdateRenderTargets ();
      }
      catch (common::Exception& exception)
      {
        exception.Touch (METHOD_NAME);
        LogPrintf ("Exception: %s", exception.Message ());
      }
      catch (std::exception& exception)
      {
        LogPrintf ("Exception '%s'", exception.what ());
      }
      catch (...)
      {
        LogPrintf ("Unknown exception at %s", METHOD_NAME);
      }
    }

      //установка текущего хранилища буфера кадра

    state.SetFrameBufferHolder (frame_buffer_holder);
  }

    //установка текущего состояния подуровня смешивания цветов
  void SetBlendState (IBlendState* in_blend_state)
  {
    BlendState* blend_state = cast_object<BlendState> (*this, in_blend_state, "render::low_level::opengl::OutputStage::SetBlendState", "blend_state");
    
    state.SetBlendState (blend_state);
  }
    
    //получение текущего состояния подуровня смешивания цветов
  IBlendState* GetBlendState () { return state.GetBlendState (); }  
  
    //установка текущего состояния подуровня попиксельного отсечения
  void SetDepthStencilState (IDepthStencilState* in_depth_stencil_state)
  {
    DepthStencilState* depth_stencil_state = cast_object<DepthStencilState> (*this, in_depth_stencil_state,
      "render::low_level::opengl::OutputStage::SetDepthStencilState", "depth_stencil_state");

    state.SetDepthStencilState (depth_stencil_state);
  }
  
    //получение текущего состояния подуровня попиксельного отсечения
  IDepthStencilState* GetDepthStencilState () { return state.GetDepthStencilState (); }
  
    //очистка текущего буфера цвета
  void ClearRenderTargetView (const Color4f& color)
  {
    try
    {
      Bind ();
      
      glClearColor (color.red, color.green, color.blue, color.alpha);
      glClear      (GL_COLOR_BUFFER_BIT);      
      CheckErrors  ("glClear");
    }
    catch (common::Exception& exception)
    {
      exception.Touch ("render::low_level::opengl::OutputStage::ClearRenderTargetView");

      throw;
    }
  }

    //очистка буфера глубина-трафарет
  void ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
  {
    try
    {
      Bind ();

      GLbitfield mask = 0;

      if (clear_flags & ClearFlag_Depth)
      {
        glClearDepth (depth);

        mask |= GL_DEPTH_BUFFER_BIT;        
      }

      if (clear_flags & ClearFlag_Stencil)
      {
        glClearStencil (stencil);

        mask |= GL_STENCIL_BUFFER_BIT;        
      }      

      glClear     (mask);
      CheckErrors ("glClear");
    }
    catch (common::Exception& exception)
    {
      exception.Touch ("render::low_level::opengl::OutputStage::ClearDepthStencilView");

      throw;
    }
  }
  
    //совместная очистка буферов цвета и глубина-трафарет
  void ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
  {
    try
    {
      Bind ();
      
      GLbitfield mask = 0;
      
      if (clear_flags & ClearFlag_RenderTarget)
      {
        glClearColor (color.red, color.green, color.blue, color.alpha);
        
        mask |= GL_COLOR_BUFFER_BIT;
      }

      if (clear_flags & ClearFlag_Depth)
      {
        glClearDepth (depth);

        mask |= GL_DEPTH_BUFFER_BIT;        
      }

      if (clear_flags & ClearFlag_Stencil)
      {
        glClearStencil (stencil);

        mask |= GL_STENCIL_BUFFER_BIT;        
      }      

      glClear     (mask);
      CheckErrors ("glClear");
    }
    catch (common::Exception& exception)
    {
      exception.Touch ("render::low_level::opengl::OutputStage::ClearViews");

      throw;
    }
  }
  
    //установка состояния уровня в контекст OpenGL
  void Bind ()
  {
    static const char* METHOD_NAME = "render::low_level::opengl::OutputStage::Bind";
    
    FrameBuffer*       frame_buffer        = state.GetFrameBuffer ();
    BlendState*        blend_state         = state.GetBlendState ();
    DepthStencilState* depth_stencil_state = state.GetDepthStencilState ();
    
    if (!frame_buffer)
      RaiseInvalidOperation (METHOD_NAME, "No frame-buffer selected (use IDevice::OSSetRenderTargets)");
      
    bool color_buffer_state         = true,
         depth_stencil_buffer_state = true;

    frame_buffer->Bind (color_buffer_state, depth_stencil_buffer_state);
    
    if (blend_state && color_buffer_state)
    {
      blend_state->Bind ();      
    }
    else
    {
      null_blend_state->Bind ();
    }
    
    if (depth_stencil_state && depth_stencil_buffer_state)
    {
      depth_stencil_state->Bind (state.GetStencilReference ());
    }
    else
    {
      null_depth_stencil_state->Bind (state.GetStencilReference ());
    }
  }
};

/*
    Конструктор / деструктор
*/

OutputStage::OutputStage (ContextManager& context_manager, ISwapChain* swap_chain)
  : impl (new Impl (context_manager, swap_chain))
{
}

OutputStage::~OutputStage ()
{
}

/*
    Создание текстур
*/

ITexture* OutputStage::CreateTexture (const TextureDesc& desc)
{
  try
  {
    return impl->resource_factory->CreateTexture (desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateTexture");

    throw;
  }
}

ITexture* OutputStage::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return impl->resource_factory->CreateRenderTargetTexture (swap_chain, buffer_index);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateRenderTargetTexture");
    
    throw;
  }
}

ITexture* OutputStage::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return impl->resource_factory->CreateDepthStencilTexture (swap_chain);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateDepthStencilTexture");
    
    throw;
  }
}

/*
    Создание отображений
*/

IView* OutputStage::CreateView (ITexture* texture, const ViewDesc& desc)
{
  try
  {
    return impl->CreateView (texture, desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateView");

    throw;
  }
}

/*
    Создание состояний выходного уровня
*/

IBlendState* OutputStage::CreateBlendState (const BlendDesc& desc)
{
  try
  {
    return new BlendState (impl->GetContextManager (), desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateBlendState");

    throw;
  }
}

IDepthStencilState* OutputStage::CreateDepthStencilState (const DepthStencilDesc& desc)
{
  try
  {
    return new DepthStencilState (impl->GetContextManager (), desc);
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::CreateDepthStencilState");
    
    throw;
  }
}

/*
    Выбор целевых отображений
*/

void OutputStage::SetRenderTargets (IView* render_target_view, IView* depth_stencil_view)
{
  try
  {
    return impl->SetRenderTargets (render_target_view, depth_stencil_view);          
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::OutputStage::SetRenderTargets");

    throw;
  }
}

IView* OutputStage::GetRenderTargetView () const
{
  return impl->state.GetRenderTargetView ();
}

IView* OutputStage::GetDepthStencilView () const
{
  return impl->state.GetDepthStencilView ();
}

/*
    Настройка подуровня смешивания цветов
*/

void OutputStage::SetBlendState (IBlendState* state)
{
  impl->SetBlendState (state);
}

IBlendState* OutputStage::GetBlendState () const
{
  return impl->GetBlendState ();
}

/*
    Настройка подуровня попиксельного отсечения
*/

void OutputStage::SetDepthStencilState (IDepthStencilState* state)
{
  impl->SetDepthStencilState (state);
}

void OutputStage::SetStencilReference (size_t reference)
{
  impl->state.SetStencilReference (reference);
}

IDepthStencilState* OutputStage::GetDepthStencilState () const
{
  return impl->GetDepthStencilState ();
}

size_t OutputStage::GetStencilReference () const
{
  return impl->state.GetStencilReference ();
}

/*
    Очистка буферов отрисовки
*/

void OutputStage::ClearRenderTargetView (const Color4f& color)
{  
  impl->ClearRenderTargetView (color);
}

void OutputStage::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  impl->ClearDepthStencilView (clear_flags, depth, stencil);
}

void OutputStage::ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  impl->ClearViews (clear_flags, color, depth, stencil);
}

/*
    Установка состояния уровня в контекст OpenGL
*/

void OutputStage::Bind ()
{
  impl->Bind ();
}
