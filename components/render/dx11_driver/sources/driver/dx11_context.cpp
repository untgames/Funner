#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор
*/

Context::Context (const DxContextPtr& in_context, const DeviceManager& device_manager)
  : DeviceObject (device_manager)
  , context (in_context)
  , render_target_context (context, device_manager)
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
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::ISSetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::ISSetIndexBuffer (IBuffer* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IInputLayout* Context::ISGetInputLayout ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBuffer* Context::ISGetVertexBuffer (size_t vertex_buffer_slot)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBuffer* Context::ISGetIndexBuffer ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
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
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::SSSetProgramParametersLayout (IProgramParametersLayout* layout)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::SSSetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IProgramParametersLayout* Context::SSGetProgramParametersLayout ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IProgram* Context::SSGetProgram ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IBuffer* Context::SSGetConstantBuffer (size_t buffer_slot)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Управление текстурами
*/

void Context::SSSetSampler (size_t sampler_slot, ISamplerState* state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::SSSetTexture (size_t sampler_slot, ITexture* texture)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

ISamplerState* Context::SSGetSampler (size_t sampler_slot)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

ITexture* Context::SSGetTexture (size_t sampler_slot)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
    Управление растеризатором (rasterizer-stage)
*/

void Context::RSSetState (IRasterizerState* state)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::RSSetViewport (size_t render_target_slot, const Viewport& viewport)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::RSSetScissor (size_t render_target_slot, const Rect& scissor_rect)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IRasterizerState* Context::RSGetState ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

const Viewport& Context::RSGetViewport (size_t render_target_slot)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

const Rect& Context::RSGetScissor (size_t render_target_slot)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
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

    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Context::OSSetRenderTargets");
    throw;
  }
}

void Context::OSSetRenderTargetView (size_t render_target_slot, IView* render_target_view)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::OSSetDepthStencilView (IView* depth_stencil_view)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IView* Context::OSGetRenderTargetView (size_t render_target_slot)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

IView* Context::OSGetDepthStencilView ()
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::ClearRenderTargetView (size_t view_index, const Color4f& color)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

void Context::ClearViews (size_t clear_flags, size_t views_count, const size_t* view_indices, const Color4f* colors, float depth, unsigned char stencil)
{
  throw xtl::make_not_implemented_exception (__FUNCTION__);
}

/*
   Генерация мип-уровней текстуры (необходимо для текстур в которые ведется рендеринг)
*/

void Context::GenerateMips (ITexture* texture)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Context::GenerateMips");
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
