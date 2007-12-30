#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

//добавить в signal multislot

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

  FrameBufferHolder (View* in_render_target_view, View* in_depth_stencil_view, OutputStageResourceFactory* resource_factory) :
    render_target_view (in_render_target_view),
    depth_stencil_view (in_depth_stencil_view),
    frame_buffer       (CreateFrameBuffer (in_render_target_view, in_depth_stencil_view, resource_factory))
  {
  }
  
  private:
    template <class T>
    static FrameBuffer* CreateFrameBufferImpl (const T& render_target, View* depth_stencil_view, OutputStageResourceFactory* resource_factory)
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
          return resource_factory->CreateFrameBuffer (render_target, depth_stencil_view->GetBindableTexture ());
        default:
          RaiseNotSupported (METHOD_NAME, "Unsupported depth-stencil view type '%s'", typeid (depth_stencil_view->GetTexture ()).name ());
          return 0;
      }
    }
  
    static FrameBuffer* CreateFrameBuffer (View* render_target_view, View* depth_stencil_view, OutputStageResourceFactory* resource_factory)
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
          return CreateFrameBufferImpl (render_target_view->GetBindableTexture (), depth_stencil_view, resource_factory);
        default:
          RaiseNotSupported (METHOD_NAME, "Unsupported render-target view type '%s'", typeid (render_target_view->GetTexture ()).name ());
          return 0;
      }
    }
};

/*
    Хранилище для отображения
*/

struct ViewHolder: public xtl::reference_counter
{
  xtl::com_ptr<View>      view;            //отображение
  xtl::auto_slot<void ()> on_destroy_view; //соединение события удаления отображения с соответствующим сигналом
  
  ViewHolder (const xtl::com_ptr<View>& in_view, xtl::slot<void ()>& destroy_handler) :
    view (in_view), on_destroy_view (destroy_handler)
  {
    view->RegisterDestroyHandler (destroy_handler);
  }
};

/*
    Описание состояния выходного уровня конвейера OpenGL
    TODO: (в будущем имеет смысл убрать OutputStageState, поскольку не планируется хранение состояний отдельно от
    OutputStage::Impl)
*/

class OutputStageState
{
  public:  
      //конструктор
    OutputStageState () :
      blend_state (0),
      frame_buffer_holder (0),
      on_destroy_blend_state (xtl::bind (&OutputStageState::SetBlendState, this, (BlendState*)0)),
      on_destroy_frame_buffer_holder (xtl::bind (&OutputStageState::SetFrameBufferHolder, this, (FrameBufferHolder*)0))
       {}
    
      //присваивание
    OutputStageState& operator = (const OutputStageState& state)
    {
      SetBlendState (state.GetBlendState ());
      SetFrameBufferHolder (state.GetFrameBufferHolder ());

      return *this;
    }

      //установка текущего состояния подуровня смешивания цветов
    void SetBlendState (BlendState* state)
    {
      if (state == blend_state)
        return;
      
      blend_state = state;
      
      if (state)
        state->RegisterDestroyHandler (on_destroy_blend_state);
    }

      //получение текущего состояния подуровня смешивания цветов
    BlendState* GetBlendState () const { return blend_state; }
    
      //установка текущего хранилища буфера кадра
    void SetFrameBufferHolder (FrameBufferHolder* in_frame_buffer_holder)
    {
      if (frame_buffer_holder == in_frame_buffer_holder)
        return;        

      frame_buffer_holder = in_frame_buffer_holder;

      if (frame_buffer_holder)
        frame_buffer_holder->RegisterDestroyHandler (on_destroy_frame_buffer_holder);
    }
    
      //получение текущего хранилища буфера кадра
    FrameBufferHolder* GetFrameBufferHolder () const { return frame_buffer_holder; }
    
      //получение текущего буфера кадра
    FrameBuffer* GetFrameBuffer () const { return frame_buffer_holder ? frame_buffer_holder->frame_buffer : 0; }

      //получение текущего отображения буфера цвета
    View* GetRenderTargetView () const { return frame_buffer_holder ? frame_buffer_holder->render_target_view : 0; }
    
      //получение текущего отображения буфера глубина-трафарет
    View* GetDepthStencilView () const { return frame_buffer_holder ? frame_buffer_holder->depth_stencil_view : 0; }

  private:
    OutputStageState (const OutputStageState&); //no impl

  private:  
    BlendState*             blend_state;                    //текущее состояние подуровня смешивания цветов
    FrameBufferHolder*      frame_buffer_holder;            //текущее хранилище буфера кадра
    xtl::auto_slot<void ()> on_destroy_blend_state;         //обработчик события удаления состояния подуровня смешивания цветов
    xtl::auto_slot<void ()> on_destroy_frame_buffer_holder; //обработчик события удаления хранилища буфера кадра
};

}

/*
    Описание реализации выходного уровня конвейера OpenGL
*/

typedef xtl::intrusive_ptr<ViewHolder>        ViewHolderPtr;
typedef xtl::intrusive_ptr<FrameBufferHolder> FrameBufferHolderPtr;
typedef stl::list<FrameBufferHolderPtr>       FrameBufferHolderList;
typedef stl::list<ViewHolderPtr>              ViewHolderList;

struct OutputStage::Impl: public ContextObject
{
  OutputStageState                           state;                     //состояние уровня
  stl::auto_ptr<OutputStageResourceFactory>  default_resource_factory;  //фабрика ресурсов по умолчанию
  OutputStageResourceFactory*                resource_factory;          //выбранная фабрика ресурсов
  FrameBufferHolderList                      frame_buffers;             //буферы кадра
  ViewHolderList                             views;                     //отображения
  xtl::com_ptr<ISwapChain>                   default_swap_chain;        //цепочка обмена по умолчанию
  xtl::com_ptr<IBlendState>                  default_blend_state;       //состояние подуровня смешивания цветов "по умолчанию"
  size_t                                     context_id;                //TEST!!!

  Impl (ContextManager& context_manager, ISwapChain* swap_chain) :
    ContextObject (context_manager),
    default_resource_factory (OutputStageResourceFactory::CreateDefaultFactory (context_manager)),
    resource_factory (default_resource_factory.get ()),
    default_swap_chain (swap_chain)
  {
      //TEST!!!!
      
    context_id = context_manager.CreateContext (swap_chain);
    
    context_manager.SetContext (context_id, swap_chain, swap_chain);
      
      //!!!!!!!!
        
    BlendDesc blend_desc;
    
    memset (&blend_desc, 0, sizeof (blend_desc));

    blend_desc.blend_enable                     = false;
    blend_desc.blend_color_operation            = BlendOperation_Add;
    blend_desc.blend_color_source_argument      = BlendArgument_One;
    blend_desc.blend_color_destination_argument = BlendArgument_Zero;
    blend_desc.blend_alpha_operation            = BlendOperation_Add;
    blend_desc.blend_alpha_source_argument      = BlendArgument_One;
    blend_desc.blend_alpha_destination_argument = BlendArgument_Zero;
    blend_desc.color_write_mask                 = ColorWriteFlag_All;

    default_blend_state = xtl::com_ptr<IBlendState> (new BlendState (GetContextManager (), blend_desc), false);

      //установка состояния "по умолчанию"

    SetBlendState (&*default_blend_state);
  }    
  
    //получение буфера кадра по отображениям
  FrameBufferHolder* GetFrameBufferHolder (View* render_target_view, View* depth_stencil_view)
  {
      //поиск буфера в списке уже созданных
    
    for (FrameBufferHolderList::iterator iter=frame_buffers.begin (), end=frame_buffers.end (); iter!=end; ++iter)
      if ((*iter)->render_target_view == render_target_view && (*iter)->depth_stencil_view == depth_stencil_view)
      {
          //оптимизация: перемещение найденного узла в начало списка для ускорения повтороного поиска

        frame_buffers.splice (frame_buffers.begin (), frame_buffers, iter);

        return &**iter;
      }
      
      //создание нового буфера кадра
      
    FrameBufferHolderPtr frame_buffer_holder (new FrameBufferHolder (render_target_view, depth_stencil_view, resource_factory), false);  

      //добавление нового буфера кадра в список созданных

    frame_buffers.push_front (frame_buffer_holder);

    return &*frame_buffers.front ();
  }
  
    //добавление соединения
  void AddView (const xtl::com_ptr<View>& view)
  {
    xtl::slot<void ()> destroy_handler = xtl::bind (&Impl::RemoveView, this, view.get ());
    
    views.push_back (ViewHolderPtr (new ViewHolder (view, destroy_handler), false));
  }
  
    //удаления отображений
  void RemoveView (View* view)
  {
      //удаление отображения из списка отображений
      
    for (ViewHolderList::iterator iter=views.begin (), end=views.end (); iter!=end;)
    {
      if ((*iter)->view == view)
      {
        ViewHolderList::iterator tmp = iter;
        
        ++tmp;

        views.erase (iter);

        iter = tmp;
      }
      else ++iter;
    }
    
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

    //установка текущего состояния подуровня смешивания цветов
  void SetBlendState (IBlendState* in_blend_state)
  {
    if (!in_blend_state)
      RaiseNullArgument ("render::low_level::opengl::OutputStage::SetBlendState", "blend_state");
      
    BlendState* blend_state = cast_object<BlendState> (*this, in_blend_state, "render::low_level::opengl::OutputStage::SetBlendState", "blend_state");
    
    state.SetBlendState (blend_state);
  }
    
    //получение текущего состояния подуровня смешивания цветов
  IBlendState* GetBlendState () { return state.GetBlendState (); }  
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
    xtl::com_ptr<View> view (new View (texture, desc), false);        

    impl->AddView (view);

    view->AddRef ();

    return view.get ();
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

IDepthStencilState* OutputStage::CreateDepthStencilState (const DepthStencilDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateDepthStencilState");
  return 0;
}

/*
    Выбор целевых отображений
*/

void OutputStage::SetRenderTargets (IView* in_render_target_view, IView* in_depth_stencil_view)
{
  static const char* METHOD_NAME = "render::low_level::opengl::OutputStage::SetRenderTargets";

  try
  {
      //приведение типов отображений

    View *render_target_view = cast_object<View> (in_render_target_view, METHOD_NAME, "render_target_view"),
         *depth_stencil_view = cast_object<View> (in_depth_stencil_view, METHOD_NAME, "depth_stencil_view");

      //получение хранилища буфера кадра, связанного с указанными отображениями

    FrameBufferHolder* frame_buffer_holder = impl->GetFrameBufferHolder (render_target_view, depth_stencil_view);
    
      //обновление целевых буферов отрисовки
      
    if (impl->state.GetFrameBuffer ())
      impl->state.GetFrameBuffer ()->UpdateRenderTargets ();

      //установка текущего хранилища буфера кадра

    impl->state.SetFrameBufferHolder (frame_buffer_holder);
  }
  catch (common::Exception& exception)
  {
    exception.Touch (METHOD_NAME);

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
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetDepthStencilState");
}

void OutputStage::SetStencilReference (size_t reference)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetStencilReference");
}

IDepthStencilState* OutputStage::GetDepthStencilState () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetDepthStencilState");
  return 0;
}

size_t OutputStage::GetStencilReference () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetDepthStencilReference");
  return 0;
}

/*
    Очистка буферов отрисовки
*/

void OutputStage::ClearRenderTargetView (const Color4f& color)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::ClearRenderTargetView");
}

void OutputStage::ClearDepthStencilView (float depth, unsigned char stencil)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::ClearDepthStencilView");
}

void OutputStage::ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::ClearViews");
}
