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
    texture_manager (context_manager)
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

IRasterizerState* Device::CreateRasterizerState (const RasterizerDesc& desc)
{
  return rasterizer_stage.CreateRasterizerState (desc);
}

IPredicate* Device::CreatePredicate ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreatePredicate");
  return 0;
}

IStatisticsQuery* Device::CreateStatisticsQuery ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateStatisticsQuery");
  return 0;
}

/*
    Управление входным уровнем (input-stage)
*/

IInputLayout* Device::CreateInputLayout (const InputLayoutDesc& desc)
{
  return input_stage.CreateInputLayoutState (desc);
}

IBuffer* Device::CreateBuffer (const BufferDesc& desc)
{
  switch (desc.bind_flags)
  {
    case BindFlag_VertexBuffer: return input_stage.CreateVertexBuffer (desc);
    case BindFlag_IndexBuffer:  return input_stage.CreateIndexBuffer (desc);
    default: RaiseInvalidArgument ("render::low_level::opengl::Device::CreateBuffer", "desc.bind_flags", desc.bind_flags); return 0;
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

IShaderParametersLayout* Device::CreateShaderParametersLayout (const ShaderParametersLayoutDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateShaderParametersLayout");
  return 0;
}

IShader* Device::CreateShader (const ShaderDesc& desc, const LogFunction& error_log)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateShader");
  return 0;
}

void Device::SSSetShader (IShader* shader)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetShader");
}

void Device::SSSetShaderParametersLayout (IShaderParametersLayout* parameters_layout)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetShaderParametersLayout");
}

void Device::SSSetConstantBuffer (size_t buffer_slot, IBuffer* buffer)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetConstantBuffer");
}

IShaderParametersLayout* Device::SSGetShaderParametersLayout ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetShaderParametersLayout");
  return 0;
}

IShader* Device::SSGetShader ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetShader");
  return 0;
}

IBuffer* Device::SSGetConstantBufer (size_t buffer_slot)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetConstantBufer");
  return 0;
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

void Device::SetPredication (IPredicate* predicate, bool predicate_value)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SetPredication");
}

IPredicate* Device::GetPredicate ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::GetPredicate");
  return 0;
}

bool Device::GetPredicateValue ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::GetPredicateValue");
  return false;
}

/*
    Установка состояния устройства в контекст OpenGL
*/

void Device::Bind (size_t base_vertex, size_t base_index)
{
  output_stage.Bind ();
//  input_stage.Bind (base_vertex, base_index);
  rasterizer_stage.Bind ();
  texture_manager.Bind ();
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
  static const char* METHOD_NAME = "render::low_level::opengl::Device::Draw";

    //TODO: сделать проверку корректности индексов!!!
    
  GLenum mode = get_mode (primitive_type, METHOD_NAME);
  
  try
  {
      //установка состояния устройства в контекст OpenGL

    Bind (0, 0);

      //рисование

    glDrawArrays (mode, first_vertex, vertices_count);

      //проверка ошибок

    context_manager.CheckErrors ("glDrawArrays");
  }
  catch (common::Exception& exception)
  {
    exception.Touch (METHOD_NAME);
    
    throw;
  }
}

void Device::DrawIndexed (PrimitiveType primitive_type, size_t first_index, size_t indices_count, size_t base_vertex)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Device::DrawIndexed";

    //TODO: сделать проверку корректности индексов!!!
    
  GLenum mode       = get_mode (primitive_type, METHOD_NAME),
         index_type = input_stage.GetIndexType ();

  try
  {
      //установка состояния устройства в контекст OpenGL

    Bind (base_vertex, first_index);

      //рисование

    glDrawElements (mode, indices_count, index_type, input_stage.GetIndices ());

      //проверка ошибок

    context_manager.CheckErrors ("glDrawElements");
  }
  catch (common::Exception& exception)
  {
    exception.Touch (METHOD_NAME);

    throw;
  }
}

/*
    Ожидание завершения выполнения буфера команд
*/

void Device::Flush ()
{
  context_manager.MakeContextCurrent ();
  glFinish ();
}

/*
    Список свойств устройства отрисовки
*/

IPropertyList* Device::GetProperties ()
{
  return &properties;
}
