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

typedef xtl::com_ptr<Program> ProgramPtr;

struct ShaderManagerContextState::Impl: public DeviceObject
{
  InputLayoutPtr             input_layout;                                 //входной лэйаут
  ProgramPtr                 program;                                      //программа
  ProgramParametersLayoutPtr parameters_layout;                            //лэйаут параметров программы
  SourceConstantBufferPtr    buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //константные буферы
  BindableProgramContext     bindable_program_context;                     //контекст программы
  bool                       is_dirty;                                     //флаг "грязности"

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

void ShaderManagerContextState::SetInputLayout (InputLayout* state)
{
  try
  {
    if (state == impl->input_layout)
      return;

    impl->input_layout = state;

    impl->bindable_program_context.input_layout = DxInputLayoutPtr ();

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::SetInputLayout");
    throw;
  }
}

InputLayout* ShaderManagerContextState::GetInputLayout () const
{
  return impl->input_layout.get ();
}

/*
    Установка состояния
*/

void ShaderManagerContextState::SetProgram (IProgram* in_program)
{
  try
  {
    Program* program = cast_object<Program> (*impl, in_program, "", "program");

    if (program == impl->program)
      return;

    impl->program = program;

    impl->bindable_program_context.Reset ();

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::SetProgram");
    throw;
  }
}

void ShaderManagerContextState::SetProgramParametersLayout (IProgramParametersLayout* parameters_layout)
{
  try
  {
    ProgramParametersLayout* layout = cast_object<ProgramParametersLayout> (parameters_layout, "", "parameters_layout");

    if (layout == impl->parameters_layout)
      return;

    impl->parameters_layout = layout;

    impl->bindable_program_context.Reset ();

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::SetProgramParametersLayout");
    throw;
  }
}

void ShaderManagerContextState::SetConstantBuffer (size_t buffer_slot, IBuffer* in_buffer)
{
  try
  {
    if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "buffer_slot", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

    SourceConstantBuffer* buffer = cast_object<SourceConstantBuffer> (in_buffer, "", "buffer");

    if (impl->buffers [buffer_slot] == buffer)
      return;

    impl->buffers [buffer_slot]                                = buffer;
    impl->bindable_program_context.dirty_buffers [buffer_slot] = true;
    impl->bindable_program_context.has_dirty_buffers           = true;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::SetConstantBuffer");
    throw;
  }
}

/*
    Получение состояния, вьюпорта и отсечения
*/

IProgramParametersLayout* ShaderManagerContextState::GetProgramParametersLayout () const
{
  return impl->parameters_layout.get ();
}

IProgram* ShaderManagerContextState::GetProgram () const
{
  return impl->program.get ();
}

IBuffer* ShaderManagerContextState::GetConstantBuffer (size_t buffer_slot) const
{
  try
  {
    if (buffer_slot >= DEVICE_CONSTANT_BUFFER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "buffer_slot", buffer_slot, DEVICE_CONSTANT_BUFFER_SLOTS_COUNT);

    return impl->buffers [buffer_slot].get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContextState::GetConstantBuffer");
    throw;
  }
}

/*
    Копирование состояния
*/

void ShaderManagerContextState::CopyTo (const StateBlockMask& mask, ShaderManagerContextState& dst_state) const
{
  bool update_notify = false, reset_bindable_program_context = false, has_dirty_buffers = false, dirty_buffers [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT];

  memset (dirty_buffers, 0, sizeof (dirty_buffers));

  if (mask.is_layout && impl->input_layout != dst_state.impl->input_layout)
  {
    dst_state.impl->input_layout                          = impl->input_layout;
    dst_state.impl->bindable_program_context.input_layout = DxInputLayoutPtr ();
    update_notify                                         = true;
  }
  
  if (mask.ss_program && impl->program != dst_state.impl->program)
  {
    dst_state.impl->program        = impl->program;
    update_notify                  = true;
    reset_bindable_program_context = true;
  }

  if (mask.ss_program_parameters_layout && impl->parameters_layout != dst_state.impl->parameters_layout)
  {
    dst_state.impl->parameters_layout = impl->parameters_layout;
    update_notify                     = true;
    reset_bindable_program_context    = true;
  }

  for (size_t i = 0; i < DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++)
    if (mask.ss_constant_buffers [i] && impl->buffers [i] != dst_state.impl->buffers [i])
    {
      dst_state.impl->buffers [i] = impl->buffers [i];          
      update_notify               = true;
      has_dirty_buffers           = true;
      dirty_buffers [i]           = true;
    }

  if (reset_bindable_program_context)
  {
    dst_state.impl->bindable_program_context.Reset ();    
  }
  else if (has_dirty_buffers)
  {
    bool*       dst = &dst_state.impl->bindable_program_context.dirty_buffers [0];
    const bool* src = &dirty_buffers [0];

    for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++, src++, dst++)
      *dst = *dst || *src;

    dst_state.impl->bindable_program_context.has_dirty_buffers = true;
  }

  if (update_notify)
    dst_state.impl->UpdateNotify ();
}

/*
===================================================================================================
    ShaderManagerContext
===================================================================================================
*/

/*
    Описание реализации контекста
*/

typedef xtl::trackable_ptr<BindableProgram> BindableProgramWeakPtr;
typedef xtl::com_ptr<ShaderLibrary>         ShaderLibraryPtr;
typedef xtl::com_ptr<Program>               ProgramPtr;

struct ShaderManagerContext::Impl: public ShaderManagerContextState::Impl
{
  ShaderLibraryPtr       shader_library;           //библиотека шейдеров
  InputLayoutPtr         default_input_layout;     //входной лэйаут по умолчанию
  ProgramPtr             default_program;          //программа по умолчанию
  DxContextPtr           context;                  //контекст
  BindableProgramWeakPtr bindable_program;         //программа, устанавливаемая в контекст
  size_t                 buffer_hashes [DEVICE_CONSTANT_BUFFER_SLOTS_COUNT]; //хэши буферов

/// Конструктор
  Impl (ShaderLibrary& library, const DxContextPtr& in_context, const DefaultResources& default_resources)
    : ShaderManagerContextState::Impl (library.GetDeviceManager ())
    , shader_library (&library)
    , default_input_layout (default_resources.input_layout)
    , context (in_context)
  {
    static const char* METHOD_NAME = "render::low_level::dx11::ShaderManagerContext::Impl::Impl";

    if (!context)
      throw xtl::make_null_argument_exception (METHOD_NAME, "context");

    if (!default_input_layout)
      throw xtl::make_null_argument_exception (METHOD_NAME, "default_input_layout");

    if (!default_resources.program)
      throw xtl::make_null_argument_exception (METHOD_NAME, "default_program");

    default_program = cast_object<Program> (library.GetDeviceManager (), default_resources.program.get (), METHOD_NAME, "default_program");

    memset (buffer_hashes, 0, sizeof (buffer_hashes));
  }
};

/*
    Конструктор / деструктор
*/

ShaderManagerContext::ShaderManagerContext (ShaderLibrary& library, const DxContextPtr& context, const DefaultResources& default_resources)
  : ShaderManagerContextState (new Impl (library, context, default_resources))
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
  try
  {
    Impl& impl = GetImpl ();

      //проверка необходимости биндинга

    if (!impl.is_dirty)
    {
      SourceConstantBufferPtr* buffer = impl.buffers;
      size_t*                  hash   = impl.buffer_hashes;

      for (size_t i=0; i<DEVICE_CONSTANT_BUFFER_SLOTS_COUNT; i++, buffer++, hash++)
      {
        size_t new_hash = *buffer ? (*buffer)->GetHash () : 0;

        if (*hash == new_hash)
          continue;

        *hash                                           = new_hash;
        impl.bindable_program_context.has_dirty_buffers = true;
        impl.bindable_program_context.dirty_buffers [i] = true;
      }

      if (!impl.bindable_program_context.has_dirty_buffers)
        return;
    }

      //получение программы, устанавливаемой в контекст

    if (!impl.bindable_program || &impl.bindable_program->GetProgram () != impl.program.get () || impl.bindable_program->GetProgramParametersLayout () != impl.parameters_layout.get ())
    { 
      Program* program = impl.program.get ();

      if (!program)
        program = impl.default_program.get ();

      BindableProgram& bindable_program = impl.shader_library->GetBindableProgram (*program, impl.parameters_layout.get ());

      impl.bindable_program = &bindable_program;     
    }

      //проверка входного лэйаута

    InputLayout* input_layout = impl.input_layout.get ();

    if (!input_layout)
      input_layout = impl.default_input_layout.get ();

      //установка в контекст

    impl.bindable_program->Bind (*impl.context, *impl.shader_library, impl.buffers, *input_layout, impl.bindable_program_context);

      //обновление флагов

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::ShaderManagerContext::Bind");
    throw;
  }
}
