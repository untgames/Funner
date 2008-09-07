#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
===================================================================================================
    Описание состояния выходного уровня конвейера OpenGL
===================================================================================================    
*/

class OutputStageState: public IStageState
{
  public:  
      //конструкторы
    OutputStageState (OutputStageState* in_main_state)
    {
      Init (in_main_state, 0);
    }

    OutputStageState (ContextObject* in_owner)
    {
      Init (0, in_owner);
    }

      //установка текущего состояния подуровня смешивания цветов
    void SetBlendState (BlendState* state)
    {
      if (state == blend_state)
        return;

      blend_state = state;
      
      UpdateNotify ();
    }

      //получение текущего состояния подуровня смешивания цветов
    BlendState* GetBlendState () const { return blend_state.get (); }
    
      //установка текущего состояния подуровня попиксельного отсечения
    void SetDepthStencilState (DepthStencilState* state)
    {
      if (state == depth_stencil_state)
        return;

      depth_stencil_state = state;

      UpdateNotify ();      
    }

      //получение текущего состояния подуровня попиксельного отсечения
    DepthStencilState* GetDepthStencilState () const { return depth_stencil_state.get (); }
    
      //установка значения ссылки трафарета
    void SetStencilReference (size_t reference)
    {
      stencil_reference = reference;

      UpdateNotify ();
    }

      //получение значения ссылки трафарета
    size_t GetStencilReference () const { return stencil_reference; }

      //установка состояние растеризатора
    void SetRasterizerState (RasterizerState* state)
    {
      if (state == rasterizer_state)
        return;
        
      rasterizer_state = state;
      
      UpdateNotify ();
    }

      //получение состояния растеризатора
    RasterizerState* GetRasterizerState () const { return rasterizer_state.get (); }

      //захват состояния
    void Capture (const StateBlockMask& mask)
    {
      if (main_state)
        Copy (*main_state, mask);
    }
    
      //восстановление состояния
    void Apply (const StateBlockMask& mask)
    {
      if (main_state)
        main_state->Copy (*this, mask);
    }

  private:    
      //копирование состояния
    void Copy (OutputStageState& source, const StateBlockMask& mask)
    {
      if (mask.os_blend_state)
        SetBlendState (source.GetBlendState ());

      if (mask.os_depth_stencil_state)
        SetDepthStencilState (source.GetDepthStencilState ());

      if (mask.rs_state)
        SetRasterizerState (source.GetRasterizerState ());
    }
    
      //оповещение об обновлении состояния уровня
    void UpdateNotify ()
    {
      if (!owner)
        return;

      owner->GetContextManager ().StageRebindNotify (Stage_Output);
    }

  private:
    OutputStageState (const OutputStageState&); //no impl
    
    void Init (OutputStageState* in_main_state, ContextObject* in_owner)
    {
      owner             = in_owner;
      main_state        = in_main_state;
      stencil_reference = 0;
    }

  private:    
    typedef xtl::trackable_ptr<BlendState>         BlendStatePtr;
    typedef xtl::trackable_ptr<DepthStencilState>  DepthStencilStatePtr;
    typedef xtl::trackable_ptr<OutputStageState>   OutputStageStatePtr;
    typedef xtl::trackable_ptr<RasterizerState>    RasterizerStatePtr;

  private:
    ContextObject*       owner;               //владелец состояния уровня
    OutputStageStatePtr  main_state;          //основное состояние уровня
    BlendStatePtr        blend_state;         //текущее состояние подуровня смешивания цветов
    DepthStencilStatePtr depth_stencil_state; //текущее состояние подуровня попиксельного отсечения
    size_t               stencil_reference;   //текущее значение трафарета
    RasterizerStatePtr   rasterizer_state;    //состояние уровня растеризации
};

}

/*
===================================================================================================
    Описание реализации выходного уровня конвейера OpenGL
===================================================================================================
*/

struct OutputStage::Impl: public ContextObject, public OutputStageState
{
  public:
///Конструктор
    Impl (const ContextManager& context_manager) :
      ContextObject (context_manager),
      OutputStageState (static_cast<ContextObject*> (this))
    {
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
      
        //инициализация состояния растеризатора по умолчанию

      RasterizerDesc rasterizer_desc;

      memset (&rasterizer_desc, 0, sizeof rasterizer_desc);

      rasterizer_desc.fill_mode               = FillMode_Solid;
      rasterizer_desc.cull_mode               = CullMode_None;
      rasterizer_desc.front_counter_clockwise = true;
      rasterizer_desc.depth_bias              = 0;
      rasterizer_desc.scissor_enable          = false;
      rasterizer_desc.multisample_enable      = false;
      rasterizer_desc.antialiased_line_enable = false;

      default_rasterizer_state = RasterizerStatePtr (new RasterizerState (GetContextManager (), rasterizer_desc), false);      
      
        //установка состояния по умолчанию

      SetBlendState        (&*default_blend_state);
      SetDepthStencilState (&*default_depth_stencil_state);
      SetRasterizerState   (&*default_rasterizer_state);
    }
    
///Установка состояния уровня в контекст OpenGL
    void Bind (bool has_render_target, bool has_depth_stencil)
    {
      try
      {      
          //установка состояния подуровня смешивания цветов        

        BlendState* blend_state = GetBlendState ();        

        if (!blend_state || !has_render_target)
          blend_state = null_blend_state.get ();

        blend_state->Bind ();

          //установка состояния подуровня попиксельного отсечения

        DepthStencilState* depth_stencil_state = GetDepthStencilState ();        

        if (!depth_stencil_state || !has_depth_stencil)
          depth_stencil_state = null_depth_stencil_state.get ();

        depth_stencil_state->Bind (GetStencilReference ());

          //установка состояния растеризатора

        RasterizerState* rasterizer_state = GetRasterizerState ();
      
        if (!rasterizer_state)
          rasterizer_state = default_rasterizer_state.get ();

        rasterizer_state->Bind ();

          //проверка ошибок

        CheckErrors ("");
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::low_level::opengl::OutputStage::Bind");
        throw;
      }
    }
    
  private:
    typedef xtl::com_ptr<BlendState>        BlendStatePtr;
    typedef xtl::com_ptr<DepthStencilState> DepthStencilStatePtr;
    typedef xtl::com_ptr<RasterizerState>   RasterizerStatePtr;

  private:
    BlendStatePtr        default_blend_state;         //состояние подуровня смешивания цветов по умолчанию
    BlendStatePtr        null_blend_state;            //состояние подуровня смешивания цветов соотв. отключению подуровня
    DepthStencilStatePtr default_depth_stencil_state; //состояние подуровня попиксельного отсечения по умолчанию
    DepthStencilStatePtr null_depth_stencil_state;    //состояние подуровня попиксельного отсечения соотв. отключению подуровня
    RasterizerStatePtr   default_rasterizer_state;    //состояние растеризатора по умолчанию
};

/*
    Конструктор / деструктор
*/

OutputStage::OutputStage (const ContextManager& context_manager)
  : impl (new Impl (context_manager))
{
}

OutputStage::~OutputStage ()
{
}

/*
    Создание объекта состояния устройства
*/

IStageState* OutputStage::CreateStageState ()
{
  return new OutputStageState (static_cast<OutputStageState*> (&*impl));
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
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateBlendState");

    throw;
  }
}

IDepthStencilState* OutputStage::CreateDepthStencilState (const DepthStencilDesc& desc)
{
  try
  {
    return new DepthStencilState (impl->GetContextManager (), desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateDepthStencilState");
    
    throw;
  }
}

IRasterizerState* OutputStage::CreateRasterizerState (const RasterizerDesc& desc)
{
  try
  {
    return new RasterizerState (impl->GetContextManager (), desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::OutputStage::CreateRasterizerState");
    throw;
  }
}

/*
    Настройка подуровня смешивания цветов
*/

void OutputStage::SetBlendState (IBlendState* state)
{
  BlendState* blend_state = cast_object<BlendState> (*impl, state, "render::low_level::opengl::OutputStage::SetBlendState", "state");
      
  impl->SetBlendState (blend_state);
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
  DepthStencilState* depth_stencil_state = cast_object<DepthStencilState> (*impl, state, "render::low_level::opengl::OutputStage::SetDepthStencilState", "state");

  impl->SetDepthStencilState (depth_stencil_state);
}

void OutputStage::SetStencilReference (size_t reference)
{
  impl->SetStencilReference (reference);
}

IDepthStencilState* OutputStage::GetDepthStencilState () const
{
  return impl->GetDepthStencilState ();
}

size_t OutputStage::GetStencilReference () const
{
  return impl->GetStencilReference ();
}

/*
    Настройка подуровня растеризации
*/

void OutputStage::SetRasterizerState (IRasterizerState* state)
{
  RasterizerState* rasterizer_state = cast_object<RasterizerState> (*impl, state, "render::low_level::opengl::OutputStage::SetState", "state");

  impl->SetRasterizerState (rasterizer_state);
}

IRasterizerState* OutputStage::GetRasterizerState () const
{
  return impl->GetRasterizerState ();
}

/*
    Установка состояния уровня в контекст OpenGL
*/

void OutputStage::Bind (bool has_render_target, bool has_depth_stencil_target)
{
  impl->Bind (has_render_target, has_depth_stencil_target);
}
