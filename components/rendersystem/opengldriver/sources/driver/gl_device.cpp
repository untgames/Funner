#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

Device::Device (Driver* in_driver, ISwapChain* swap_chain, const char* init_string)
  : driver (in_driver),
    context_manager (xtl::bind (&Driver::LogMessage, in_driver, _1), init_string),
    output_stage (context_manager, swap_chain),
    input_stage (context_manager),
    rasterizer_stage (context_manager),
    texture_manager (context_manager),
    shader_stage (context_manager),
    query_manager (context_manager)
{  
    //получение информации об устройстве отрисовки

  properties.AddProperty ("init_string",   init_string);
  properties.AddProperty ("gl_vendor",     context_manager.GetVendor ());
  properties.AddProperty ("gl_renderer",   context_manager.GetRenderer ());
  properties.AddProperty ("gl_version",    context_manager.GetVersion ());
  properties.AddProperty ("gl_extensions", context_manager.GetExtensions ());
}

Device::~Device ()
{
}

/*
    Имя устройства
*/

const char* Device::GetName ()
{
  return "render::low_level::opengl::Device";
}

/*
    Создание ресурсов
*/

IStatisticsQuery* Device::CreateStatisticsQuery ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateStatisticsQuery");
  return 0;
}

IStateBlock* Device::CreateStateBlock (const StateBlockMask& mask)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateStateBlock");
  return 0;
}

/*
    Управление входным уровнем (input-stage)
*/

IInputLayout* Device::CreateInputLayout (const InputLayoutDesc& desc)
{
  return input_stage.CreateInputLayout (desc);
}

IBuffer* Device::CreateBuffer (const BufferDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Device::CreateBuffer";

  try
  {
    static const size_t BAD_FLAGS = BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil;

    if (desc.bind_flags & BAD_FLAGS)
      RaiseInvalidArgument ("", "desc.bind_flags", get_name ((BindFlag)desc.bind_flags));

    switch (desc.bind_flags)
    {
      case BindFlag_VertexBuffer:   return input_stage.CreateVertexBuffer (desc);
      case BindFlag_IndexBuffer:    return input_stage.CreateIndexBuffer (desc);
      case BindFlag_ConstantBuffer: return input_stage.CreateConstantBuffer (desc);
      default:
        RaiseNotSupported ("", "Incompatible desc.bind_flags=%s", get_name ((BindFlag)desc.bind_flags));
        return 0;
    }
  }
  catch (Exception& e)
  {
    e.Touch (METHOD_NAME);
    throw;
  }
}

void Device::ISSetInputLayout (IInputLayout* state)
{
  input_stage.SetInputLayout (state);
}

void Device::ISSetVertexBuffer (size_t vertex_buffer_slot, IBuffer* buffer)
{
  input_stage.SetVertexBuffer (vertex_buffer_slot, buffer);
}

void Device::ISSetIndexBuffer (IBuffer* buffer)
{
  input_stage.SetIndexBuffer (buffer);
}

IInputLayout* Device::ISGetInputLayout ()
{
  return input_stage.GetInputLayout ();
}

IBuffer* Device::ISGetVertexBuffer (size_t vertex_buffer_slot)
{
  return input_stage.GetVertexBuffer (vertex_buffer_slot);
}

IBuffer* Device::ISGetIndexBuffer ()
{
  return input_stage.GetIndexBuffer ();
}

/*
    Управление шейдерными уровнями (shader-stage)
*/

IProgramParametersLayout* Device::CreateProgramParametersLayout (const ProgramParametersLayoutDesc& desc)
{
  return shader_stage.CreateProgramParametersLayout (desc);
}

IProgram* Device::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  return shader_stage.CreateProgram (shaders_count, shader_descs, error_log);
}

void Device::SSSetProgram (IProgram* program)
{
  shader_stage.SetProgram (program);
}

void Device::SSSetProgramParametersLayout (IProgramParametersLayout* layout)
{
  shader_stage.SetProgramParametersLayout (layout);  
}

void Device::SSSetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  shader_stage.SetConstantBuffer (buffer_slot, buffer);
}

IProgramParametersLayout* Device::SSGetProgramParametersLayout ()
{
  return shader_stage.GetProgramParametersLayout ();
}

IProgram* Device::SSGetProgram ()
{
  return shader_stage.GetProgram ();
}

IBuffer* Device::SSGetConstantBuffer (size_t buffer_slot)
{
  return shader_stage.GetConstantBuffer (buffer_slot);
}

/*
    Управление текстурами
*/

ISamplerState* Device::CreateSamplerState (const SamplerDesc& desc)
{
  return texture_manager.CreateSamplerState (desc);
}

ITexture* Device::CreateTexture (const TextureDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Device::CreateTexture";

  if (!(desc.bind_flags & (BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil)))
  {
    RaiseNotSupported (METHOD_NAME, "Unsupported bindable flags desc.bind_flags=%s",
      get_name ((BindFlag)desc.bind_flags));

    return 0;
  }
  
  try
  {
    if (desc.bind_flags & BindFlag_Texture)
    {    
      return texture_manager.CreateTexture (desc);
    }
    else if (desc.bind_flags & (BindFlag_RenderTarget | BindFlag_DepthStencil))
    {
      return output_stage.CreateTexture (desc);
    }
    else return 0;
  }
  catch (common::Exception& exception)
  {
    exception.Touch (METHOD_NAME);
    
    throw;
  }
}

void Device::SSSetSampler (size_t sampler_slot, ISamplerState* state)
{
  texture_manager.SetSampler (sampler_slot, state);
}

void Device::SSSetTexture (size_t sampler_slot, ITexture* texture)
{
  texture_manager.SetTexture (sampler_slot, texture);
}

ISamplerState* Device::SSGetSampler (size_t sampler_slot)
{
  return texture_manager.GetSampler (sampler_slot);
}

ITexture* Device::SSGetTexture (size_t sampler_slot)
{
  return texture_manager.GetTexture (sampler_slot);
}

/*
    Управление растеризатором (rasterizer-stage)
*/

IRasterizerState* Device::CreateRasterizerState (const RasterizerDesc& desc)
{
  return rasterizer_stage.CreateRasterizerState (desc);
}

void Device::RSSetState (IRasterizerState* state)
{
  rasterizer_stage.SetState (state);
}

void Device::RSSetViewport (const Viewport& viewport)
{
  rasterizer_stage.SetViewport (viewport);
}

void Device::RSSetScissor (const Rect& scissor_rect)
{
  rasterizer_stage.SetScissor (scissor_rect);
}

IRasterizerState* Device::RSGetState ()
{
  return rasterizer_stage.GetState ();
}

const Viewport& Device::RSGetViewport ()
{
  return rasterizer_stage.GetViewport ();
}

const Rect& Device::RSGetScissor ()
{
  return rasterizer_stage.GetScissor ();  
}

/*
    Управление выходным уровнем (output-stage)
*/

ITexture* Device::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  return output_stage.CreateRenderTargetTexture (swap_chain, buffer_index);
}

ITexture* Device::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  return output_stage.CreateDepthStencilTexture (swap_chain);
}

IBlendState* Device::CreateBlendState (const BlendDesc& desc)
{
  return output_stage.CreateBlendState (desc);
}

IDepthStencilState* Device::CreateDepthStencilState (const DepthStencilDesc& desc)
{
  return output_stage.CreateDepthStencilState (desc);
}

IView* Device::CreateView (ITexture* texture, const ViewDesc& desc)
{
  return output_stage.CreateView (texture, desc);
}

void Device::OSSetBlendState (IBlendState* state)
{
  output_stage.SetBlendState (state);
}

void Device::OSSetDepthStencilState (IDepthStencilState* state)
{
  output_stage.SetDepthStencilState (state);
}

void Device::OSSetStencilReference (size_t reference)
{
  output_stage.SetStencilReference (reference);
}

void Device::OSSetRenderTargets (IView* render_target_view, IView* depth_stencil_view)
{
  output_stage.SetRenderTargets (render_target_view, depth_stencil_view);
}

IBlendState* Device::OSGetBlendState ()
{
  return output_stage.GetBlendState ();
}

IDepthStencilState* Device::OSGetDepthStencilState ()
{
  return output_stage.GetDepthStencilState ();
}

size_t Device::OSGetStencilReference ()
{
  return output_stage.GetStencilReference ();
}

IView* Device::OSGetRenderTargetView ()
{
  return output_stage.GetRenderTargetView ();
}

IView* Device::OSGetDepthStencilView ()
{
  return output_stage.GetDepthStencilView ();
}

/*
    Очистка
*/

void Device::ClearRenderTargetView (const Color4f& color)
{
  output_stage.ClearRenderTargetView (color);
}

void Device::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  output_stage.ClearDepthStencilView (clear_flags, depth, stencil);
}

void Device::ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  output_stage.ClearViews (clear_flags, color, depth, stencil);
}

/*
    Управление предикатами отрисовки
*/

IPredicate* Device::CreatePredicate ()
{
  return query_manager.CreatePredicate ();
}

void Device::SetPredication (IPredicate* predicate, bool predicate_value)
{
  query_manager.SetPredication (predicate, predicate_value);
}

IPredicate* Device::GetPredicate ()
{
  return query_manager.GetPredicate ();
}

bool Device::GetPredicateValue ()
{
  return query_manager.GetPredicateValue ();
}

/*
    Установка состояния устройства в контекст OpenGL
*/

void Device::Bind (size_t base_vertex, size_t base_index, IndicesLayout* out_indices_layout)
{
  try
  {
    output_stage.Bind ();
    output_stage.InvalidateRenderTargets (rasterizer_stage.GetViewport ());
    input_stage.Bind (base_vertex, base_index, out_indices_layout);
    rasterizer_stage.Bind ();
    texture_manager.Bind ();
    shader_stage.Bind ();
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Device::Bind");
    throw;
  }
}

/*
    Рисование примитивов
*/

namespace
{

//получение режима рисования примитивов OpenGL
GLenum get_mode (PrimitiveType type, const char* source)
{
  switch (type)
  {
    case PrimitiveType_PointList:     return GL_POINTS;
    case PrimitiveType_LineList:      return GL_LINES;
    case PrimitiveType_LineStrip:     return GL_LINE_STRIP;
    case PrimitiveType_TriangleList:  return GL_TRIANGLES;
    case PrimitiveType_TriangleStrip: return GL_TRIANGLE_STRIP;
    case PrimitiveType_TriangleFan:   return GL_TRIANGLE_FAN;
    default:
      RaiseInvalidArgument (source, "primitive_type", type);
      return 0;
  }
}

}

void Device::Draw (PrimitiveType primitive_type, size_t first_vertex, size_t vertices_count)
{
  try
  {
      //проверка предиката отрисовки

    if (!query_manager.GetPredicateAsyncResult ())
      return;

      //преобразование типа примитивов

    GLenum mode = get_mode (primitive_type, "");
    
      //установка состояния устройства в контекст OpenGL

    Bind (0, 0);

      //рисование      

    glDrawArrays (mode, first_vertex, vertices_count);

      //проверка ошибок

    context_manager.CheckErrors ("glDrawArrays");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Device::Draw");

    throw;
  }
}

void Device::DrawIndexed (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex)
{
  try
  {
      //проверка предиката отрисовки

    if (!query_manager.GetPredicateAsyncResult ())
      return;

      //преобразование типа примитивов

    GLenum mode = get_mode (primitive_type, "");
    
      //установка состояния устройства в контекст OpenGL

    IndicesLayout indices_layout;

    Bind (base_vertex, first_index, &indices_layout);

      //рисование

    glDrawElements (mode, indices_count, indices_layout.type, indices_layout.data);

      //проверка ошибок

    context_manager.CheckErrors ("glDrawElements");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Device::DrawIndexed");

    throw;
  }
}

/*
    Ожидание завершения выполнения буфера команд
*/

void Device::Flush ()
{
  try
  {
      //установка текущего контекста

    context_manager.MakeContextCurrent ();
    
      //сброс очереди команд
    
    glFlush ();  

      //обновление целевых буферов отрисовки

    output_stage.UpdateRenderTargets ();
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Device::Flush");
    throw;
  }
}

/*
    Список свойств устройства отрисовки
*/

IPropertyList* Device::GetProperties ()
{
  return &properties;
}
