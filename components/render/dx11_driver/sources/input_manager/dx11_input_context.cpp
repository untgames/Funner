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

typedef xtl::com_ptr<InputLayout> InputLayoutPtr;

struct InputManagerContextState::Impl: public DeviceObject
{
  InputLayoutPtr input_layout; //входной лэйаут  
  bool           is_dirty;     //флаг "грязности"

/// Конструктор
  Impl (const DeviceManager& device_manager)
    : DeviceObject (device_manager)
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

InputManagerContextState::InputManagerContextState (const DeviceManager& device_manager)
  : impl (new Impl (device_manager))
{
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

void InputManagerContextState::SetInputLayout (InputLayout* state)
{  
  impl->input_layout = state;

  impl->UpdateNotify ();
}

InputLayout* InputManagerContextState::GetInputLayout () const
{
  return impl->input_layout.get ();
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
