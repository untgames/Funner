#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
===================================================================================================
    ShaderManagerContextState
===================================================================================================
*/

/*
    Описание реализации состояния контекста менеджера шейдеров
*/

struct ShaderManagerContextState::Impl: public DeviceObject
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

ShaderManagerContextState::ShaderManagerContextState (const DeviceManager& device_manager)
  : impl (new Impl (device_manager))
{
}

ShaderManagerContextState::ShaderManagerContextState (Impl* in_impl)
  : impl (in_impl)
{
}

ShaderManagerContextState::~ShaderManagerContextState ()
{
}

/*
    Реализация контекста
*/

ShaderManagerContextState::Impl& ShaderManagerContextState::GetImpl () const
{
  return *impl;
}

/*
    Управление конфигурацией входных данных
*/

void ShaderManagerContextState::SetInputLayout (IInputLayout* state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IInputLayout* ShaderManagerContextState::GetInputLayout () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Установка состояния, вьюпорта и отсечения
*/

void ShaderManagerContextState::SetProgram (IProgram* program)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ShaderManagerContextState::SetProgramParametersLayout (IProgramParametersLayout* parameters_layout)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void ShaderManagerContextState::SetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Получение состояния, вьюпорта и отсечения
*/

IProgramParametersLayout* ShaderManagerContextState::GetProgramParametersLayout () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IProgram* ShaderManagerContextState::GetProgram () const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBuffer* ShaderManagerContextState::GetConstantBuffer (size_t buffer_slot) const
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
===================================================================================================
    ShaderManagerContext
===================================================================================================
*/

/*
    Описание реализации контекста
*/

struct ShaderManagerContext::Impl: public ShaderManagerContextState::Impl
{
  DxContextPtr context; //контекст

/// Конструктор
  Impl (const DeviceManager& device_manager, const DxContextPtr& in_context)
    : ShaderManagerContextState::Impl (device_manager)
    , context (in_context)
  {
    if (!context)
      throw xtl::make_null_argument_exception ("render::low_level::dx11::ShaderManagerContext::Impl::Impl", "context");
  }
};

/*
    Конструктор / деструктор
*/

ShaderManagerContext::ShaderManagerContext (const DeviceManager& device_manager, const DxContextPtr& context)
  : ShaderManagerContextState (new Impl (device_manager, context))
{
}

ShaderManagerContext::~ShaderManagerContext ()
{
}

/*
    Возвращение реализации
*/

ShaderManagerContext::Impl& ShaderManagerContext::GetImpl () const
{
  return static_cast<Impl&> (ShaderManagerContextState::GetImpl ());
}

/*
    Биндинг состояния, вьюпорта и отсечения
*/

void ShaderManagerContext::Bind ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}
