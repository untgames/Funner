#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

/*
    Состояние уровня растеризации
*/

class RasterizerStageState
{
  public:  
      //конструктор
    RasterizerStageState () : rasterizer_state (0), need_recalc_hash (true) {}
    
      //установка состояние растеризатора
    void SetRasterizerState (RasterizerState* state)
    {
      if (state == rasterizer_state)
        return;
        
      rasterizer_state = state;
    }

      //получение состояния растеризатора
    RasterizerState* GetRasterizerState () const { return rasterizer_state.get (); }
    
      //установка текущей области вывода
    void SetViewport (const Viewport& viewport)
    {
      viewport_scissor.viewport = viewport;
      need_recalc_hash          = true;
    }
    
      //получение текущей области вывода
    const Viewport& GetViewport () const { return viewport_scissor.viewport; }
    
      //установка текущей области отсечения
    void SetScissor (const Rect& scissor)
    {
      viewport_scissor.scissor = scissor;
      need_recalc_hash         = true;
    }
    
      //получение текущей области отсечения
    const Rect& GetScissor () const { return viewport_scissor.scissor; }
    
      //получение хэша областей вывода и отсечения
    size_t GetViewportScissorHash () const
    {
      if (need_recalc_hash)
      {
        viewport_scissor_hash = crc32 (&viewport_scissor, sizeof viewport_scissor);
        need_recalc_hash      = false;
      }

      return viewport_scissor_hash;
    }
  
  private:
    typedef xtl::trackable_ptr<RasterizerState> RasterizerStatePtr;

    #pragma pack (1)

    struct ViewportScissor
    {
      Viewport viewport;
      Rect     scissor;
      
      ViewportScissor ()
      {
        memset (this, 0, sizeof *this);
      }      
    };

  private:
    RasterizerStatePtr rasterizer_state;      //состояние уровня растеризации
    ViewportScissor    viewport_scissor;      //вьюпорт и отсечение
    mutable size_t     viewport_scissor_hash; //хеш областей вывода и отсечения
    mutable bool       need_recalc_hash;      //флаг необходимости пересчёта хэша областей вывода и отсечения
};

}

/*
    Описание реализации RasterizerStage
*/

struct RasterizerStage::Impl: public ContextObject
{
  private:
    typedef xtl::com_ptr<RasterizerState> RasterizerStatePtr;

  public:
  
    /*
        Конструктор    
    */

    Impl (const ContextManager& context_manager) : ContextObject (context_manager)
    {
        //инициализация состояния растеризатора по умолчанию

      RasterizerDesc desc;

      memset (&desc, 0, sizeof desc);

      desc.fill_mode               = FillMode_Solid;
      desc.cull_mode               = CullMode_None;
      desc.front_counter_clockwise = true;
      desc.depth_bias              = 0;
      desc.scissor_enable          = false;
      desc.multisample_enable      = false;
      desc.antialiased_line_enable = false;

      default_rasterizer_state = RasterizerStatePtr (new RasterizerState (GetContextManager (), desc), false);
      
      state.SetRasterizerState (default_rasterizer_state.get ());
      
        //инициализация областей отсечения и вывода
        
      SwapChainDesc swap_chain_desc;
      ISwapChain*   draw_swap_chain = context_manager.GetDrawSwapChain ();
      
      if (!draw_swap_chain)
        RaiseInvalidOperation ("render::low_level::opengl::RasterizerStage::Impl::Impl", "Null draw swap chain");

      draw_swap_chain->GetDesc (swap_chain_desc);

      Viewport default_viewport;
      Rect     default_scissor;

      memset (&default_viewport, 0, sizeof default_viewport);
      memset (&default_scissor, 0, sizeof default_scissor);

      default_viewport.width     = swap_chain_desc.frame_buffer.width;
      default_viewport.height    = swap_chain_desc.frame_buffer.height;
      default_viewport.max_depth = 1.0f;

      default_scissor.width  = swap_chain_desc.frame_buffer.width;
      default_scissor.height = swap_chain_desc.frame_buffer.height;

      state.SetViewport (default_viewport);
      state.SetScissor  (default_scissor);      
    }
    
    /*
        Создание состояние растеризатора
    */

    IRasterizerState* CreateRasterizerState (const RasterizerDesc& desc)
    {
      return new RasterizerState (GetContextManager (), desc);
    }

    /*
        Установка / получение состояния растеризатора    
    */

    void SetRasterizerState (IRasterizerState* in_rasterizer_state)
    {
      RasterizerState* rasterizer_state = cast_object<RasterizerState> (*this, in_rasterizer_state, "render::low_level::opengl::RasterizerStage::SetState", "rasterizer_state");

      state.SetRasterizerState (rasterizer_state);
    }
    
    IRasterizerState* GetRasterizerState () const
    {
      return state.GetRasterizerState ();
    }
    
    /*
        Установка / получение состояний областей вывода и отсечения
    */

    void            SetViewport (const Viewport& viewport) { state.SetViewport (viewport); }
    void            SetScissor  (const Rect& scissor)      { state.SetScissor (scissor); }
    const Viewport& GetViewport () const                   { return state.GetViewport (); }
    const Rect&     GetScissor  () const                   { return state.GetScissor (); }

    /*
        Установка уровня в контекст OpenGL
    */

    void Bind ()
    {
        //установка текущего контекста

      MakeContextCurrent ();      

        //устаовка состояния растеризатора

      RasterizerState* rasterizer_state = state.GetRasterizerState ();
      
      if (!rasterizer_state)
        rasterizer_state = default_rasterizer_state.get ();

      rasterizer_state->Bind ();
      
        //установка областей вывода и отсечения
        
      size_t& current_viewport_scissor_hash = GetContextDataTable (Stage_Rasterizer)[RasterizerStageCache_ViewportScissorHash];
      
      if (current_viewport_scissor_hash != state.GetViewportScissorHash ())
      {
        const Viewport& viewport = state.GetViewport ();
        const Rect&     scissor  = state.GetScissor ();

        glViewport   (viewport.x, viewport.y, viewport.width, viewport.height);
        glDepthRange (viewport.min_depth, viewport.max_depth);
        glScissor    (scissor.x, scissor.y, scissor.width, scissor.height);

        current_viewport_scissor_hash = state.GetViewportScissorHash ();
      }

        //проверка ошибок

      CheckErrors ("render::low_level::opengl::RasterizerStage::Impl::Bind");
    }    

  private:
    RasterizerStageState state;                    //состояние уровня
    RasterizerStatePtr   default_rasterizer_state; //состояние растеризатора по умолчанию
};

/*
===================================================================================================
    RasterizerStage
===================================================================================================
*/

/*
   Конструктор / деструктор
*/

RasterizerStage::RasterizerStage (const ContextManager& context_manager)
  : impl (new Impl (context_manager))
  {}

RasterizerStage::~RasterizerStage ()
{
}

/*
   Биндинг состояния, вьюпорта и отсечения
*/

void RasterizerStage::Bind ()
{
  impl->Bind ();
}    

/*
   Создание дескриптора растерайзера
*/

IRasterizerState* RasterizerStage::CreateRasterizerState (const RasterizerDesc& desc)
{
  return impl->CreateRasterizerState (desc);
}
    
/*
   Установка состояния, вьюпорта и отсечения
*/

void RasterizerStage::SetState (IRasterizerState* state)
{
  impl->SetRasterizerState (state);
}

void RasterizerStage::SetViewport (const Viewport& viewport)
{
  impl->SetViewport (viewport);
}

void RasterizerStage::SetScissor (const Rect& scissor_rect)
{
  impl->SetScissor (scissor_rect);
}

/*
   Получение состояния, вьюпорта и отсечения
*/

IRasterizerState* RasterizerStage::GetState ()
{
  return impl->GetRasterizerState ();
}

const Viewport& RasterizerStage::GetViewport ()
{
  return impl->GetViewport ();
}

const Rect& RasterizerStage::GetScissor ()
{
  return impl->GetScissor ();
}
