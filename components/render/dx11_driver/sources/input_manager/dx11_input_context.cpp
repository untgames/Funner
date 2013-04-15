#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    InputManagerContextState
===================================================================================================
*/

/*
    Описание реализации состояния контекста менеджера входного уровня
*/

struct InputManagerContextState::Impl: public DeviceObject
{
/// Конструктор
  Impl (const DeviceManager& device_manager)
    : DeviceObject (device_manager)
  {
  }

/// Деструктор
  virtual ~Impl () {}
};

/*
    Конструктор / деструктор
*/

InputManagerContextState::InputManagerContextState (const DeviceManager&)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

InputManagerContextState::InputManagerContextState (Impl* in_impl)
  : impl (in_impl)
{
}

InputManagerContextState::~InputManagerContextState ()
{
}

/*
    Возвращение реализации
*/

InputManagerContextState::Impl& InputManagerContextState::GetImpl () const
{
  return *impl;
}

/*
    Управление конфигурацией входных данных
*/

void InputManagerContextState::SetInputLayout (IInputLayout* state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IInputLayout* InputManagerContextState::GetInputLayout () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Управление вершинными буферами
*/

void InputManagerContextState::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBuffer* InputManagerContextState::GetVertexBuffer (size_t vertex_buffer_slot) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Управление индексным буфером
*/

void InputManagerContextState::SetIndexBuffer (IBuffer* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBuffer* InputManagerContextState::GetIndexBuffer () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
===================================================================================================
    InputManagerContext
===================================================================================================
*/

struct InputManagerContext::Impl: public InputManagerContextState::Impl
{
  DxContextPtr context; //контекст

/// Конструктор
  Impl (const DeviceManager& device_manager, const DxContextPtr& in_context)
    : InputManagerContextState::Impl (device_manager)
    , context (in_context)
  {
    if (!context)
      throw xtl::make_null_argument_exception ("render::low_level::dx11::InputManagerContext::Impl::Impl", "context");
  }
};

/*
    Конструктор / деструктор
*/

InputManagerContext::InputManagerContext (const DeviceManager& device_manager, const DxContextPtr& context)
  : InputManagerContextState (new Impl (device_manager, context))
{
}

InputManagerContext::~InputManagerContext ()
{
}

/*
    Установка состояния уровня в контекст
*/

InputManagerContext::Impl& InputManagerContext::GetImpl () const
{
  return static_cast<Impl&> (InputManagerContextState::GetImpl ());
}

void InputManagerContext::Bind ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContext::Bind");
    throw;
  }
}
