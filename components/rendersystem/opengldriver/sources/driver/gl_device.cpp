#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

Device::Device (Driver* in_driver, ISwapChain* swap_chain, const char*)
  : driver (in_driver),
    context_manager (xtl::bind (&Driver::LogMessage, in_driver, _1)),
    output_stage (context_manager, swap_chain),
    input_stage (context_manager),
    texture_manager (context_manager)
{  
    //установка текущего контекста

  context_manager.MakeContextCurrent ();

    //получение информации об устройстве отрисовки
    
  properties.AddProperty ("vendor",     reinterpret_cast<const char*> (glGetString (GL_VENDOR)));
  properties.AddProperty ("renderer",   reinterpret_cast<const char*> (glGetString (GL_RENDERER)));
  properties.AddProperty ("gl_version", reinterpret_cast<const char*> (glGetString (GL_VERSION)));
  
    //получение списка расширений
    
  const char* swap_chain_extension_string = context_manager.GetSwapChainsExtensionString ();
  
  stl::string extension_string = reinterpret_cast<const char*> (glGetString (GL_EXTENSIONS));
  
  if (*swap_chain_extension_string)
  {
    extension_string += ' ';
    extension_string += swap_chain_extension_string;
  }
  
  properties.AddProperty ("gl_extensions", extension_string.c_str ());

    //проверка состояния контекста OpenGL

  context_manager.CheckErrors ("render::low_level::opengl::Device::Device");
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

ILightingState* Device::CreateLightingState (const LightingDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateLightingState");
  return 0;
}

IViewerState* Device::CreateViewerState (const ViewerDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateViewerState");
  return 0;
}

ITransformState* Device::CreateTransformState (const TransformDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateTransformState");
  return 0;
}

IMaterialState* Device::CreateMaterialState (const MaterialDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateMaterialState");
  return 0;
}

IRasterizerState* Device::CreateRasterizerState (const RasterizerDesc&)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::CreateRasterizerState");
  return 0;
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

IInputLayoutState* Device::CreateInputLayoutState (const InputLayoutDesc& desc)
{
  return input_stage.CreateInputLayoutState (desc);
}

IBuffer* Device::CreateVertexBuffer (const BufferDesc& desc)
{
  return input_stage.CreateVertexBuffer (desc);
}

IBuffer* Device::CreateIndexBuffer (const BufferDesc& desc)
{
  return input_stage.CreateIndexBuffer (desc);
}

void Device::ISSetInputLayout (IInputLayoutState* state)
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

IInputLayoutState* Device::ISGetInputLayout ()
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
    Управление текстурами
*/

ISamplerState* Device::CreateSamplerState (const SamplerDesc& desc)
{
  return texture_manager.CreateSamplerState (desc);
}

ITexture* Device::CreateTexture (const TextureDesc& desc)
{
  if (desc.bind_flags & BindFlag_Texture)
  {
    return texture_manager.CreateTexture (desc);
  }
  else if (desc.bind_flags & (BindFlag_RenderTarget | BindFlag_DepthStencil))
  {
    return output_stage.CreateTexture (desc);
  }
  else
  {
    RaiseNotSupported ("render::low_level::opengl::Device::CreateTexture", "Unsupported bindable flags desc.bind_flags=%s",
      get_name ((BindFlag)desc.bind_flags));

    return 0;
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
    Управление шейдерными уровнями (shader-stage)
*/

void Device::SSSetMode (ShaderMode mode)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetMode");
}

void Device::SSSetViewer (IViewerState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetViewer");
}

void Device::SSSetTransform (ITransformState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetTransform");
}

void Device::SSSetLighting (ILightingState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetLighting");
}

void Device::SSSetMaterial (IMaterialState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSSetMaterial");
}

ShaderMode Device::SSGetMode ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetMode");
  return ShaderMode_Flat;
}

IViewerState* Device::SSGetViewer ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetViewer");
  return 0;
}

ITransformState* Device::SSGetTransform ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetTransform");
  return 0;
}

ILightingState* Device::SSGetLighting ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetLighting");
  return 0;
}

IMaterialState* Device::SSGetMaterial ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSGetMaterial");
  return 0;
}

/*
    Получение информации о доступных режимах шейдинга устройства
*/

bool Device::SSIsSupported (ShaderMode mode)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::SSIsSupported");
  return false;
}

/*
    Управление растеризатором (rasterizer-stage)
*/

void Device::RSSetState (IRasterizerState* state)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSSetState");
}

void Device::RSSetViewport (const Viewport& viewport)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSSetViewport");
}

void Device::RSSetScissor (const Rect& scissor_rect)
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSSetScissor");
}

IRasterizerState* Device::RSGetState ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSGetState");
  return 0;
}

const Viewport& Device::RSGetViewport ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSGetViewport");
  
  static Viewport temp_viewport;
  
  return temp_viewport;
}

const Rect& Device::RSGetScissor ()
{
  RaiseNotImplemented ("render::low_level::opengl::Device::RSGetScissor");
  
  static Rect temp_scissor;
  
  return temp_scissor;
  
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
  input_stage.Bind (base_vertex, base_index);
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
