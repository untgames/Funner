#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

enum RasterizerStageDataTable
{
  RasterizerStageDataTable_ViewportScissorHash,
  RasterizerStageDataTable_RasterizerStateHash,

  RasterizerStageDataTable_Num
};

struct ViewportScissor
{
  Viewport viewport;
  Rect     scissor_rect;
};

}

/*
    Описание реализации RasterizerStage
*/

struct RasterizerStage::Impl: public ContextObject
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl (const ContextManager& context_manager);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Биндинг состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Bind ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание дескриптора растерайзера
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRasterizerState* CreateRasterizerState (const RasterizerDesc&);    

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SetState    (IRasterizerState* state);
    void SetViewport (const Viewport& viewport);
    void SetScissor  (const Rect& scissor_rect);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение состояния, вьюпорта и отсечения
///////////////////////////////////////////////////////////////////////////////////////////////////
    IRasterizerState* GetState    ();
    const Viewport&   GetViewport ();
    const Rect&       GetScissor  ();

  public:
    RasterizerState state;                  //дескриптор
    ViewportScissor viewport_scissor;       //вьюпорт и отсечение
    size_t          viewport_scissor_hash;  //crc32 хеш вьюпорта и отсечения
    size_t          state_hash;             //crc32 хеш вьюпорта и отсечения
    bool            need_recalc_vs_hash;    //флаг изменения данных вьюпорта и отсечения 
    bool            need_recalc_state_hash; //флаг изменения данных дескриптора
};


/*
   Конструктор
*/

RasterizerStage::Impl::Impl (const ContextManager& context_manager)
  : ContextObject (context_manager), state (context_manager)
{
  RasterizerDesc desc;

  desc.fill_mode               = FillMode_Solid;
  desc.cull_mode               = CullMode_Back;
  desc.front_counter_clockwise = true;
  desc.depth_bias              = 0;
  desc.scissor_enable          = true;
  desc.multisample_enable      = false;
  desc.antialiased_line_enable = true;

  state.SetDesc (desc);
}

/*
   Биндинг состояния, вьюпорта и отсечения
*/

void RasterizerStage::Impl::Bind ()
{
  MakeContextCurrent ();

  if (need_recalc_vs_hash)
    viewport_scissor_hash = crc32 (&viewport_scissor, sizeof viewport_scissor);

  if (need_recalc_state_hash)
    state_hash = crc32 (&state, sizeof state);

  if (GetContextData (ContextDataTable_RasterizerStage, RasterizerStageDataTable_ViewportScissorHash) != viewport_scissor_hash)
  {
    glScissor    (viewport_scissor.scissor_rect.x, viewport_scissor.scissor_rect.y, viewport_scissor.scissor_rect.width, viewport_scissor.scissor_rect.height);
    glDepthRange (viewport_scissor.viewport.min_depth, viewport_scissor.viewport.max_depth);
    glViewport   (viewport_scissor.viewport.x, viewport_scissor.viewport.y, viewport_scissor.viewport.width, viewport_scissor.viewport.height);

    SetContextData (ContextDataTable_RasterizerStage, RasterizerStageDataTable_ViewportScissorHash, viewport_scissor_hash);
  }
    
  if (GetContextData (ContextDataTable_RasterizerStage, RasterizerStageDataTable_RasterizerStateHash) != state_hash)
  {
    state.Bind ();
  
    SetContextData (ContextDataTable_RasterizerStage, RasterizerStageDataTable_RasterizerStateHash, state_hash);
  }

  CheckErrors ("render::low_level::opengl::RasterizerStage::Impl::Bind");
}

/*
   Создание дескриптора растерайзера
*/

IRasterizerState* RasterizerStage::Impl::CreateRasterizerState (const RasterizerDesc& desc)
{
  IRasterizerState* ret_value = new RasterizerState (GetContextManager ());

  ret_value->SetDesc (desc);

  return ret_value;
}
    
/*
   Установка состояния, вьюпорта и отсечения
*/

void RasterizerStage::Impl::SetState (IRasterizerState* in_state)
{
  RasterizerDesc temp_desc;

  in_state->GetDesc (temp_desc);

  state.SetDesc (temp_desc);
}

void RasterizerStage::Impl::SetViewport (const Viewport& in_viewport)
{
  static const char* METHOD_NAME = "render::low_level::opengl::RasterizerStage::Impl::SetViewport";

  viewport_scissor.viewport = in_viewport;
}

void RasterizerStage::Impl::SetScissor (const Rect& in_scissor_rect)
{
  viewport_scissor.scissor_rect = in_scissor_rect;
}

/*
   Получение состояния, вьюпорта и отсечения
*/

IRasterizerState* RasterizerStage::Impl::GetState ()
{
  return &state;
}

const Viewport& RasterizerStage::Impl::GetViewport ()
{
  return viewport_scissor.viewport;
}

const Rect& RasterizerStage::Impl::GetScissor ()
{
  return viewport_scissor.scissor_rect;
}


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
  impl->SetState (state);
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
  return impl->GetState ();
}

const Viewport& RasterizerStage::GetViewport ()
{
  return impl->GetViewport ();
}

const Rect& RasterizerStage::GetScissor ()
{
  return impl->GetScissor ();
}
