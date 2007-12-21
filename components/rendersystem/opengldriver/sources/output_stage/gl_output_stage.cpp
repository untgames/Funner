#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    Описание состояния выходного уровня конвейера OpenGL
*/

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

class OutputStageState
{
  public:  
      //конструкторы
    OutputStageState () :
      blend_state (0),
      on_destroy_blend_state (xtl::bind (&OutputStageState::SetBlendState, this, (BlendState*)0))
       {}
    
      //присваивание
    OutputStageState& operator = (const OutputStageState& state)
    {
      SetBlendState (state.GetBlendState ());

      return *this;
    }

      //установка текущего состояния подуровня смешивания цветов
    void SetBlendState (BlendState* state)
    {
      blend_state = state;
      
      if (state)
        state->RegisterDestroyHandler (on_destroy_blend_state);
    }

      //получение текущего состояния подуровня смешивания цветов
    BlendState* GetBlendState () const { return blend_state; }

  private:
    OutputStageState (const OutputStageState&); //no impl

  private:  
    BlendState*             blend_state;             //текущее состояние подуровня смешивания цветов
    xtl::auto_slot<void ()> on_destroy_blend_state;  //обработчик события удаления состояния подуровня смешивания цветов
};

}

/*
    Описание реализации выходного уровня конвейера OpenGL
*/

struct OutputStage::Impl: public ContextObject
{
  OutputStageState          state;                //состояние уровня
  xtl::com_ptr<ISwapChain>  default_swap_chain;   //цепочка обмена по умолчанию
  xtl::com_ptr<IBlendState> default_blend_state;  //состояние подуровня смешивания цветов "по умолчанию"
  size_t                    context_id;           //TEST!!!

  Impl (ContextManager& context_manager, ISwapChain* swap_chain) :
    ContextObject (context_manager),
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
    Получение отображение буфера цепочки обмена на текстуру
*/

ITexture* OutputStage::GetBuffer (ISwapChain* swap_chain, size_t buffer_id)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetBuffer");
  return 0;
}

/*
    Создание отображений
*/

IView* OutputStage::CreateView (ITexture* texture, const ViewDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::CreateView");
  return 0;
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

void OutputStage::SetRenderTargets (IView* render_target_view, IView* depth_stencil_view)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetRenderTargets");
}

IView* OutputStage::GetRenderTargetView () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetRenderTargetView");
  return 0;
}

IView* OutputStage::GetDepthStencilView () const
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::GetDepthStencilView");
  return 0;
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

void OutputStage::SetDepthStencil (IDepthStencilState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::OutputStage::SetDepthStencil");
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
