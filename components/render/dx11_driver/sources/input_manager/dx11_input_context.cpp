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

typedef xtl::com_ptr<InputBuffer> InputBufferPtr;

struct InputManagerContextState::Impl: public DeviceObject
{
  InputLayoutPtr input_layout;                                      //входной лэйаут  
  InputBufferPtr vertex_buffers [DEVICE_VERTEX_BUFFER_SLOTS_COUNT]; //вершинные буферы 
  InputBufferPtr index_buffer;                                      //индексный буфер
  bool           is_dirty;                                          //флаг "грязности"

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

void InputManagerContextState::SetVertexBuffer (size_t vertex_buffer_slot, IBuffer* in_buffer)
{
  try
  {
    InputBuffer* buffer = cast_object<InputBuffer> (*impl, in_buffer, "", "buffer");

    if (vertex_buffer_slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "vertex_buffer_slot", vertex_buffer_slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);

    impl->vertex_buffers [vertex_buffer_slot] = buffer;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContextState::SetVertexBuffer");
    throw;
  }
}

IBuffer* InputManagerContextState::GetVertexBuffer (size_t vertex_buffer_slot) const
{
  try
  {
    if (vertex_buffer_slot >= DEVICE_VERTEX_BUFFER_SLOTS_COUNT)
      throw xtl::make_range_exception ("", "vertex_buffer_slot", vertex_buffer_slot, DEVICE_VERTEX_BUFFER_SLOTS_COUNT);

    return impl->vertex_buffers [vertex_buffer_slot].get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContextState::GetVertexBuffer");
    throw;
  }
}

/*
    Управление индексным буфером
*/

void InputManagerContextState::SetIndexBuffer (IBuffer* in_buffer)
{
  try
  {
    InputBuffer* buffer = cast_object<InputBuffer> (*impl, in_buffer, "", "buffer");

    impl->index_buffer = buffer;

    impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContextState::SetIndexBuffer");
    throw;
  }
}

IBuffer* InputManagerContextState::GetIndexBuffer () const
{
  return impl->index_buffer.get ();
}

/*
    Управление уровнем вывода вершин
*/

void InputManagerContextState::SOSetTargets (size_t buffers_count, IBuffer** buffers, const size_t* offsets)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContextState::SOSetTargets");
    throw;
  }
}

void InputManagerContextState::SOSetTarget (size_t stream_output_slot, IBuffer* buffer, size_t offset)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContextState::SOSetTarget");
    throw;
  }
}

IBuffer* InputManagerContextState::SOGetTarget (size_t stream_output_slot)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContextState::SOGetTarget");
    throw;
  }
}

size_t InputManagerContextState::SOGetTargetOffset (size_t stream_output_slot)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContextState::SOGetTargetOffset");
    throw;
  }
}

/*
    Копирование состояния
*/

void InputManagerContextState::CopyTo (const StateBlockMask& mask, InputManagerContextState& dst_state) const
{
  try
  {
    bool update_notify = false;

    if (mask.is_layout)
    {
      dst_state.impl->input_layout = impl->input_layout;
      update_notify                = true;
    }

    for (size_t i=0; i<DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++)
      if (mask.is_vertex_buffers [i])
      {
        dst_state.impl->vertex_buffers [i] = impl->vertex_buffers [i];
        update_notify                      = true;
      }

    if (mask.is_index_buffer)
    {
      dst_state.impl->index_buffer = impl->index_buffer;
      update_notify                = true;
    }
 
    if (update_notify)
      dst_state.impl->UpdateNotify ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContextState::CopyTo");
    throw;
  }
}

/*
===================================================================================================
    InputManagerContext
===================================================================================================
*/

struct InputManagerContext::Impl: public InputManagerContextState::Impl
{
  DxContextPtr   context;        //контекст
  InputLayoutPtr default_layout; //лэйаут по умолчанию

/// Конструктор
  Impl (const DeviceManager& device_manager, const DxContextPtr& in_context, const DefaultResources& default_resources)
    : InputManagerContextState::Impl (device_manager)
    , context (in_context)
    , default_layout (default_resources.input_layout)
  {
    static const char* METHOD_NAME = "render::low_level::dx11::InputManagerContext::Impl::Impl";

    if (!context)
      throw xtl::make_null_argument_exception (METHOD_NAME, "context");

    if (!default_layout)
      throw xtl::make_null_argument_exception (METHOD_NAME, "default_layout");
  }
};

/*
    Конструктор / деструктор
*/

InputManagerContext::InputManagerContext (const DeviceManager& device_manager, const DxContextPtr& context, const DefaultResources& default_resources)
  : InputManagerContextState (new Impl (device_manager, context, default_resources))
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

namespace
{

const UINT VB_OFFSETS [DEVICE_VERTEX_BUFFER_SLOTS_COUNT] = {0};

}

void InputManagerContext::Bind ()
{
  try
  {
      //проверка флага "грязности"

    Impl& impl = GetImpl ();

    if (!impl.is_dirty)
      return;

      //преобразование контекстной информации

    InputLayout* layout = impl.input_layout ? impl.input_layout.get () : impl.default_layout.get ();

    ID3D11Buffer* buffers [DEVICE_VERTEX_BUFFER_SLOTS_COUNT];

    ID3D11Buffer**  dst_buffer = buffers;
    InputBufferPtr* src_buffer = impl.vertex_buffers;

    for (size_t i=0; i<DEVICE_VERTEX_BUFFER_SLOTS_COUNT; i++, dst_buffer++, src_buffer++)
      *dst_buffer = *src_buffer ? &(*src_buffer)->GetHandle () : (ID3D11Buffer*)0;

    impl.context->IASetVertexBuffers (0, DEVICE_VERTEX_BUFFER_SLOTS_COUNT, buffers, layout->GetStrides (), VB_OFFSETS);
    impl.context->IASetIndexBuffer   (impl.index_buffer ? &impl.index_buffer->GetHandle () : (ID3D11Buffer*)0, layout->GetIndexFormat (), layout->GetIndexBufferOffset ());

      //очистка флага "грязности"

    impl.is_dirty = false;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::InputManagerContext::Bind");
    throw;
  }
}
