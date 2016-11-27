#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    OutputManagerContextState
===================================================================================================
*/

struct OutputManagerContextState::Impl: public DeviceObject
{
  BlendStatePtr        blend_state;         //состояние подуровня смешивания цветов
  DepthStencilStatePtr depth_stencil_state; //состояние подуровня отсечения
  size_t               stencil_ref;         //референсное значение стенсила
  RasterizerStatePtr   rasterizer_state;    //состояние подуровня растеризации
  bool                 is_dirty;            //флаг "грязности"

/// Конструктор
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
    , stencil_ref ()
    , is_dirty (true)
  {
  }

/// Деструктор
  virtual ~Impl () {}

/// Оповещение об изменении
  void UpdateNotify ()
  {
    is_dirty = true;
  }
};

/*
    Конструктор / деструктор
*/

OutputManagerContextState::OutputManagerContextState (const DeviceManager& manager)
  : impl (new Impl (manager))
{
}

OutputManagerContextState::OutputManagerContextState (Impl* in_impl)
  : impl (in_impl)
{
}

OutputManagerContextState::~OutputManagerContextState ()
{
}

/*
    Получение реализации
*/

OutputManagerContextState::Impl& OutputManagerContextState::GetImpl () const
{
  return *impl;
}

/*
    Настройка подуровня растеризации
*/

void OutputManagerContextState::SetRasterizerState (IRasterizerState* in_state)
{
  try
  {
    RasterizerState* state = cast_object<RasterizerState> (*impl, in_state, "", "state");

    if (state == impl->rasterizer_state)
      return;

    impl->rasterizer_state = state;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetRasterizerState");
    throw;
  }
}

IRasterizerState* OutputManagerContextState::GetRasterizerState () const
{
  return impl->rasterizer_state.get ();
}

/*
    Настройка подуровня смешивания цветов
*/

void OutputManagerContextState::SetBlendState (IBlendState* in_state)
{
  try
  {
    BlendState* state = cast_object<BlendState> (*impl, in_state, "", "state");

    if (state == impl->blend_state)
      return;

    impl->blend_state = state;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetBlendState");
    throw;
  }
}

IBlendState* OutputManagerContextState::GetBlendState () const
{
  return impl->blend_state.get ();
}

/*
    Настройка подуровня попиксельного отсечения
*/

void OutputManagerContextState::SetDepthStencilState (IDepthStencilState* in_state)
{
  try
  {
    DepthStencilState* state = cast_object<DepthStencilState> (*impl, in_state, "", "state");

    if (state == impl->depth_stencil_state)
      return;

    impl->depth_stencil_state = state;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetDepthStencilState");
    throw;
  }
}

void OutputManagerContextState::SetStencilReference (size_t reference)
{
  if (impl->stencil_ref == reference)
    return;

  impl->stencil_ref = reference;

  impl->UpdateNotify ();
}

IDepthStencilState* OutputManagerContextState::GetDepthStencilState () const
{
  return impl->depth_stencil_state.get ();
}

size_t OutputManagerContextState::GetStencilReference () const
{
  return impl->stencil_ref;
}

/*
    Копирование состояния
*/

void OutputManagerContextState::CopyTo (const StateBlockMask& mask, OutputManagerContextState& dst_state) const
{
  if (!mask.os_blend_state && !mask.os_depth_stencil_state && !mask.rs_state)
    return;

  bool update_notify = false; 

  if (mask.os_blend_state)
  {
    dst_state.impl->blend_state = impl->blend_state;
    update_notify               = true;
  }

  if (mask.os_depth_stencil_state)
  {
    dst_state.impl->depth_stencil_state = impl->depth_stencil_state;
    dst_state.impl->stencil_ref         = impl->stencil_ref;
    update_notify                       = true;
  }

  if (mask.rs_state)
  {
    dst_state.impl->rasterizer_state = impl->rasterizer_state;
    update_notify                    = true;
  }

  if (update_notify)
    dst_state.impl->UpdateNotify ();
}

/*
===================================================================================================
    OutputManagerContext
===================================================================================================
*/

/*
    Описание реализации контекста
*/

struct OutputManagerContext::Impl: public OutputManagerContextState::Impl
{
  DxContextPtr         context;                     //контекст устройства
  BlendStatePtr        null_blend_state;            //состояние подуровня смешивания с выключенной записью
  DepthStencilStatePtr null_depth_stencil_state;    //состояние подуровня отсечения с вылюченными операциями
  RasterizerStatePtr   null_rasterizer_state;       //состояни подуровня растеризации по умолчанию

/// Конструктор
  Impl (const DeviceManager& manager, const DxContextPtr& in_context, const DefaultResources& default_resources)
    : OutputManagerContextState::Impl (manager)
    , context (in_context)
  {
    static const char* METHOD_NAME = "render::low_level::dx11::OutputManagerContext::Impl::Impl";

    if (!context)
      throw xtl::make_null_argument_exception (METHOD_NAME, "context");

    if (!default_resources.blend_state)
      throw xtl::make_null_argument_exception (METHOD_NAME, "null_blend_state");

    null_blend_state = cast_object<BlendState> (manager, default_resources.blend_state.get (), METHOD_NAME, "null_blend_state");    

    if (!default_resources.depth_stencil_state)
      throw xtl::make_null_argument_exception (METHOD_NAME, "null_depth_stencil_state");

    null_depth_stencil_state = cast_object<DepthStencilState> (manager, default_resources.depth_stencil_state.get (), METHOD_NAME, "null_depth_stencil_state");     

    if (!default_resources.rasterizer_state)
      throw xtl::make_null_argument_exception (METHOD_NAME, "null_rasterizer_state");

    null_rasterizer_state = cast_object<RasterizerState> (manager, default_resources.rasterizer_state.get (), METHOD_NAME, "null_rasterizer_state");
  }
};

/*
    Конструктор / деструктор
*/

OutputManagerContext::OutputManagerContext (const DeviceManager& manager, const DxContextPtr& context, const DefaultResources& default_resources)
  : OutputManagerContextState (new Impl (manager, context, default_resources))
{
}

OutputManagerContext::~OutputManagerContext ()
{
}

/*
    Получение реализации
*/

OutputManagerContext::Impl& OutputManagerContext::GetImpl () const
{
  return static_cast<Impl&> (OutputManagerContextState::GetImpl ());
}

/*
    Установка состояния уровня в контекст
*/

void OutputManagerContext::Bind ()
{
  try
  {
      //проверка флага "грязности"

    Impl& impl = GetImpl ();

    if (!impl.is_dirty)
      return;

      //установка состояния подуровня смешивания цветов

    BlendState* blend_state = impl.blend_state.get ();

    if (!blend_state)
      blend_state = impl.null_blend_state.get ();    

    static const float blend_factors [] = {1.0f, 1.0f, 1.0f, 1.0f};

    impl.context->OMSetBlendState (&blend_state->GetHandle (), blend_factors, 0xffffffff);

      //установка состояния подуровня отсечения

    DepthStencilState* depth_stencil_state = impl.depth_stencil_state.get ();

    if (!depth_stencil_state)
      depth_stencil_state = impl.null_depth_stencil_state.get ();

    impl.context->OMSetDepthStencilState (&depth_stencil_state->GetHandle (), impl.stencil_ref);

      //установка состояния подуровня растеризации

    RasterizerState* rasterizer_state = impl.rasterizer_state.get ();

    if (!rasterizer_state)
      rasterizer_state = impl.null_rasterizer_state.get ();

    impl.context->RSSetState (&rasterizer_state->GetHandle ());

      //очистка флага "грязности"

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContext::Bind");
    throw;
  }
}
