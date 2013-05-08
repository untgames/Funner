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
  BlendStatePtr blend_state; //состояние подуровня смешивания цветов
  bool          is_dirty;    //флаг "грязности"

/// Конструктор
  Impl (const DeviceManager& manager)
    : DeviceObject (manager)
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

void OutputManagerContextState::SetRasterizerState (IRasterizerState* state)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetRasterizerState");
    throw;
  }
}

IRasterizerState* OutputManagerContextState::GetRasterizerState () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
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

void OutputManagerContextState::SetDepthStencilState (IDepthStencilState* state)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContextState::SetDepthStencilState");
    throw;
  }
}

void OutputManagerContextState::SetStencilReference (size_t reference)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IDepthStencilState* OutputManagerContextState::GetDepthStencilState () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t OutputManagerContextState::GetStencilReference () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
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
  DxContextPtr  context;             //контекст устройства
  BlendStatePtr default_blend_state; //состояние подуровня смешивания по умолчанию
  BlendStatePtr null_blend_state;    //состояние подуровня смешивания с выключенной записью

/// Конструктор
  Impl (const DeviceManager& manager, const DxContextPtr& in_context, const DefaultResources& default_resources)
    : OutputManagerContextState::Impl (manager)
    , context (in_context)
  {
    static const char* METHOD_NAME = "render::low_level::dx11::OutputManagerContext::Impl::Impl";

    if (!context)
      throw xtl::make_null_argument_exception (METHOD_NAME, "context");

    if (!default_resources.blend_state)
      throw xtl::make_null_argument_exception (METHOD_NAME, "default_blend_state");

    default_blend_state = cast_object<BlendState> (manager, default_resources.blend_state.get (), METHOD_NAME, "default_blend_state");

    if (!default_resources.null_blend_state)
      throw xtl::make_null_argument_exception (METHOD_NAME, "null_blend_state");

    null_blend_state = cast_object<BlendState> (manager, default_resources.null_blend_state.get (), METHOD_NAME, "null_blend_state");    

      //установка значений по умолчанию

    blend_state = default_blend_state;
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

      //установка состояния контекста

    BlendState* blend_state = impl.blend_state.get ();

    if (!blend_state)
      blend_state = impl.null_blend_state.get ();    

    static const float blend_factors [] = {1.0f, 1.0f, 1.0f, 1.0f};

    impl.context->OMSetBlendState (&blend_state->GetHandle (), blend_factors, 0xffffffff);

    throw xtl::make_not_implemented_exception (__FUNCTION__);

      //очистка флага "грязности"

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::OutputManagerContext::Bind");
    throw;
  }
}
