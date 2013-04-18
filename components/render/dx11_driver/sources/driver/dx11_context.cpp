#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор
*/

Context::Context (const DxContextPtr& in_context, const DeviceManager& device_manager)
  : DeviceObject (device_manager)
  , context (in_context)
  , render_target_context (device_manager, in_context)
  , texture_manager_context (device_manager, in_context)
  , input_manager_context (device_manager, in_context)
  , shader_manager_context (device_manager, in_context)
{
  if (!context)
    throw xtl::make_null_argument_exception ("render::low_level::dx11::Context::Context", "context");
}

Context::~Context ()
{
}

/*
    Получение низкоуровневого дескриптора
*/

ID3D11DeviceContext& Context::GetHandle ()
{
  return *context;
}

/*
    Управление запросами
*/

void Context::Begin (IQuery* async)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::Begin");
    throw;
  }
}

void Context::End (IQuery* async)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::End");
    throw;
  }
}

/*
    Управление входным уровнем (input-stage)
*/

void Context::ISSetInputLayout (IInputLayout* state)
{
  try
  {
    shader_manager_context.SetInputLayout (state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::ISSetInputLayout");
    throw;
  }
}

void Context::ISSetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)
{
  try
  {
    input_manager_context.SetVertexBuffer (vertex_buffer_slot, buffer);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::ISSetVertexBuffer");
    throw;
  }
}

void Context::ISSetIndexBuffer (IBuffer* buffer)
{
  try
  {
    input_manager_context.SetIndexBuffer (buffer);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::ISSetIndexBuffer");
    throw;
  }
}

IInputLayout* Context::ISGetInputLayout ()
{
  try
  {
    return shader_manager_context.GetInputLayout ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::ISGetInputLayout");
    throw;
  }
}

IBuffer* Context::ISGetVertexBuffer (size_t vertex_buffer_slot)
{
  try
  {
    return input_manager_context.GetVertexBuffer (vertex_buffer_slot);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::ISGetVertexBuffer");
    throw;
  }
}

IBuffer* Context::ISGetIndexBuffer ()
{
  try
  {
    return input_manager_context.GetIndexBuffer ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::ISGetIndexBuffer");
    throw;
  }
}

/*
    Управление уровнем вывода вершин
*/

void Context::SOSetTargets (size_t buffers_count, IBuffer** buffers, const size_t* offsets)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::SOSetTarget (size_t stream_output_slot, IBuffer* buffer, size_t offset)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBuffer* Context::SOGetTarget (size_t stream_output_slot)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t Context::SOGetTargetOffset (size_t stream_output_slot)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Управление шейдерными уровнями (shader-stage)
*/

void Context::SSSetProgram (IProgram* program)
{
  try
  {
    shader_manager_context.SetProgram (program);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSSetProgram");
    throw;
  }
}

void Context::SSSetProgramParametersLayout (IProgramParametersLayout* layout)
{
  try
  {
    shader_manager_context.SetProgramParametersLayout (layout);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSSetProgramParametersLayout");
    throw;
  }
}

void Context::SSSetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  try
  {
    shader_manager_context.SetConstantBuffer (buffer_slot, buffer);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSSetConstantBuffer");
    throw;
  }
}

IProgramParametersLayout* Context::SSGetProgramParametersLayout ()
{
  try
  {
    return shader_manager_context.GetProgramParametersLayout ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSGetProgramParametersLayout");
    throw;
  }
}

IProgram* Context::SSGetProgram ()
{
  try
  {
    return shader_manager_context.GetProgram ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSGetProgram");
    throw;
  }
}

IBuffer* Context::SSGetConstantBuffer (size_t buffer_slot)
{
  try
  {
    return shader_manager_context.GetConstantBuffer (buffer_slot);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSGetConstantBuffer");
    throw;
  }
}

/*
    Управление текстурами
*/

void Context::SSSetSampler (size_t sampler_slot, ISamplerState* state)
{
  try
  {
    texture_manager_context.SetSampler (sampler_slot, state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSSetSampler");
    throw;
  }
}

void Context::SSSetTexture (size_t sampler_slot, ITexture* texture)
{
  try
  {
    texture_manager_context.SetTexture (sampler_slot, texture);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSSetTexture");
    throw;
  }
}

ISamplerState* Context::SSGetSampler (size_t sampler_slot)
{
  try
  {
    return texture_manager_context.GetSampler (sampler_slot);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSGetSampler");
    throw;
  }
}

ITexture* Context::SSGetTexture (size_t sampler_slot)
{
  try
  {
    return texture_manager_context.GetTexture (sampler_slot);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::SSGetTexture");
    throw;
  }
}

/*
    Управление растеризатором (rasterizer-stage)
*/

void Context::RSSetState (IRasterizerState* state)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::RSSetState");
    throw;
  }
}

void Context::RSSetViewport (size_t render_target_slot, const Viewport& viewport)
{
  try
  {
    return render_target_context.SetViewport (render_target_slot, viewport);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::RSSetViewport");
    throw;
  }
}

void Context::RSSetScissor (size_t render_target_slot, const Rect& scissor_rect)
{
  try
  {
    return render_target_context.SetScissor (render_target_slot, scissor_rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::RSSetScissor");
    throw;
  }
}

IRasterizerState* Context::RSGetState ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::RSGetState");
    throw;
  }
}

const Viewport& Context::RSGetViewport (size_t render_target_slot)
{
  try
  {
    return render_target_context.GetViewport (render_target_slot);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::RSGetViewport");
    throw;
  }
}

const Rect& Context::RSGetScissor (size_t render_target_slot)
{
  try
  {
    return render_target_context.GetScissor (render_target_slot);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::RSGetScissor");
    throw;
  }

}

/*
    Управление выходным уровнем (output-stage)
*/

void Context::OSSetBlendState (IBlendState* state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::OSSetDepthStencilState (IDepthStencilState* state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::OSSetStencilReference (size_t reference)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBlendState* Context::OSGetBlendState ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IDepthStencilState* Context::OSGetDepthStencilState ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

size_t Context::OSGetStencilReference ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Управление менеджером целевых буферов отрисовки
*/

void Context::OSSetRenderTargets (size_t count, IView** render_target_views, IView* depth_stencil_view)
{
  try
  {
    if (!render_target_views && count)
      throw xtl::make_null_argument_exception ("", "render_target_views");

    for (size_t i=0; i<count; i++)
      render_target_context.SetRenderTargetView (i, render_target_views [i]);

    for (size_t i=count; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
      render_target_context.SetRenderTargetView (i, 0);

    render_target_context.SetDepthStencilView (depth_stencil_view);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::OSSetRenderTargets");
    throw;
  }
}

void Context::OSSetRenderTargetView (size_t render_target_slot, IView* render_target_view)
{
  try
  {    
    render_target_context.SetRenderTargetView (render_target_slot, render_target_view);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::OSSetRenderTargetView");
    throw;
  }
}

void Context::OSSetDepthStencilView (IView* depth_stencil_view)
{
  try
  {    
    render_target_context.SetDepthStencilView (depth_stencil_view);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::OSSetDepthStencilView");
    throw;
  }
}

IView* Context::OSGetRenderTargetView (size_t render_target_slot)
{
  try
  {    
    return render_target_context.GetRenderTargetView (render_target_slot);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::OSGetRenderTargetView");
    throw;
  }
}

IView* Context::OSGetDepthStencilView ()
{
  try
  {    
    return render_target_context.GetDepthStencilView ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::OSGetDepthStencilView");
    throw;
  }
}

void Context::ClearRenderTargetView (size_t view_index, const Color4f& color)
{
  try
  {    
    render_target_context.ClearRenderTargetView (view_index, color);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::ClearRenderTargetView");
    throw;
  }
}

void Context::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  try
  {    
    render_target_context.ClearDepthStencilView (clear_flags, depth, stencil);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::ClearDepthStencilView");
    throw;
  }
}

void Context::ClearViews (size_t clear_flags, size_t views_count, const size_t* view_indices, const Color4f* colors, float depth, unsigned char stencil)
{
  try
  {    
    render_target_context.ClearViews (clear_flags, views_count, view_indices, colors, depth, stencil);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::ClearViews");
    throw;
  }
}

/*
   Генерация мип-уровней текстуры (необходимо для текстур в которые ведется рендеринг)
*/

void Context::GenerateMips (ITexture* texture)
{
  try
  {
    texture_manager_context.GenerateMips (texture);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::GenerateMips");
    throw;
  }
}

/*
    Управление предикатами отрисовки
*/

void Context::SetPredication (IPredicate* predicate, bool predicate_value)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IPredicate* Context::GetPredicate ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

bool Context::GetPredicateValue ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Работа со списками команд
*/

ICommandList* Context::FinishCommandList (bool restore_state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::ExecuteCommandList (ICommandList* list, bool restore_state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Рисование примитивов
*/

void Context::Draw (PrimitiveType primitive_type, size_t first_vertex, size_t vertices_count)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Context::Draw");
    throw;
  }
}

void Context::DrawIndexed (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Context::DrawIndexed");
    throw;
  }
}

void Context::DrawInstanced (PrimitiveType primitive_type, size_t vertex_count_per_instance, size_t instance_count, size_t first_vertex, size_t first_instance_location)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::DrawIndexedInstanced (PrimitiveType primitive_type, size_t index_count_per_instance, size_t instance_count, size_t first_index, size_t base_vertex, size_t first_instance_location)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::DrawAuto (PrimitiveType primitive_type)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Ожидание завершения выполнения буфера команд
*/

void Context::Flush ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Context::Flush");
    throw;
  }
}
