#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор / деструктор
*/

Device::Device (ISwapChain* swap_chain, const char* init_string)
  : context_manager (swap_chain, init_string),
    render_target_manager (context_manager, swap_chain),
    output_stage (context_manager),
    input_stage (context_manager),
    texture_manager (context_manager),
    shader_stage (context_manager),
    query_manager (context_manager),
    cached_base_vertex (~0),
    cached_base_index (~0)
{  
    //инициализация кэша
    
  memset (&cached_indices_layout, 0, sizeof cached_indices_layout);

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
    Получение возможностей устройства
*/

void Device::GetCaps (DeviceCaps& caps)
{
  try
  {
    context_manager.GetCaps ().GetDeviceCaps (caps);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::GetCaps");
    throw;
  }
}

const char* Device::GetCapString (DeviceCapString cap_string)
{
  try
  {
    switch (cap_string)
    {
      case DeviceCapString_ShaderProfiles:
        return shader_stage.GetShaderProfilesString ();
      default:
        throw xtl::make_argument_exception ("", "cap_string", cap_string);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::GetCapString");
    throw;
  }
}

/*
    Создание ресурсов
*/

IStatisticsQuery* Device::CreateStatisticsQuery ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::CreateStatisticsQuery");
}

/*
    Работа с блоками состояний
*/

IStateBlock* Device::CreateStateBlock (const StateBlockMask& mask)
{
  try
  {
    return new StateBlock (*this, mask);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateStateBlock");
    throw;
  }
}

/*
    Управление входным уровнем (input-stage)
*/

IInputLayout* Device::CreateInputLayout (const InputLayoutDesc& desc)
{
  try
  {
    return input_stage.CreateInputLayout (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateInputLayout");
    throw;
  }
}

IBuffer* Device::CreateBuffer (const BufferDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Device::CreateBuffer";

  try
  {
    static const size_t BAD_FLAGS = BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil;

    if (desc.bind_flags & BAD_FLAGS)
      throw xtl::make_argument_exception ("", "desc.bind_flags", get_name ((BindFlag)desc.bind_flags));

    switch (desc.bind_flags)
    {
      case BindFlag_VertexBuffer:   return input_stage.CreateVertexBuffer (desc);
      case BindFlag_IndexBuffer:    return input_stage.CreateIndexBuffer (desc);
      case BindFlag_ConstantBuffer: return input_stage.CreateConstantBuffer (desc);
      default:
        throw xtl::format_not_supported_exception ("", "Incompatible desc.bind_flags=%s", get_name ((BindFlag)desc.bind_flags));
    }
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
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
  try
  {
    return shader_stage.CreateProgramParametersLayout (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateProgramParametersLayout");
    throw;
  }
}

IProgram* Device::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  try
  {
    return shader_stage.CreateProgram (shaders_count, shader_descs, error_log);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateProgram");
    throw;
  }
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
  try
  {
    return texture_manager.CreateSamplerState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateSamplerState");
    throw;
  }
}

ITexture* Device::CreateTexture (const TextureDesc& desc)
{
  return CreateTexture (desc, 0);
}

ITexture* Device::CreateTexture (const TextureDesc& desc, const TextureData& data)
{
  return CreateTexture (desc, &data);
}

ITexture* Device::CreateTexture (const TextureDesc& desc, const TextureData* data)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Device::CreateTexture";

  if (!(desc.bind_flags & (BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil)))
  {
    throw xtl::format_not_supported_exception (METHOD_NAME, "Unsupported bindable flags desc.bind_flags=%s",
      get_name ((BindFlag)desc.bind_flags));

    return 0;
  }

  try
  {
    if (desc.bind_flags & BindFlag_Texture)
    {    
      return texture_manager.CreateTexture (desc, data);
    }
    else if (desc.bind_flags & (BindFlag_RenderTarget | BindFlag_DepthStencil))
    {
      return render_target_manager.CreateTexture (desc);
    }
    else return 0;
  }
  catch (xtl::exception& exception)
  {
    exception.touch (METHOD_NAME);

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
  try
  {
    return output_stage.CreateRasterizerState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateRasterizerState");
    throw;
  }
}

void Device::RSSetState (IRasterizerState* state)
{
  output_stage.SetRasterizerState (state);
}

void Device::RSSetViewport (const Viewport& viewport)
{
  render_target_manager.SetViewport (viewport);
}

void Device::RSSetScissor (const Rect& scissor_rect)
{
  render_target_manager.SetScissor (scissor_rect);
}

IRasterizerState* Device::RSGetState ()
{
  return output_stage.GetRasterizerState ();
}

const Viewport& Device::RSGetViewport ()
{
  return render_target_manager.GetViewport ();
}

const Rect& Device::RSGetScissor ()
{
  return render_target_manager.GetScissor ();  
}

/*
    Управление выходным уровнем (output-stage)
*/

IBlendState* Device::CreateBlendState (const BlendDesc& desc)
{
  try
  {
    return output_stage.CreateBlendState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateBlendState");
    throw;
  }
}

IDepthStencilState* Device::CreateDepthStencilState (const DepthStencilDesc& desc)
{
  try
  {
    return output_stage.CreateDepthStencilState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateDepthStencilState");
    throw;
  }
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

/*
    Управление менеджером целевых буферов отрисовки
*/

ITexture* Device::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return render_target_manager.CreateRenderTargetTexture (swap_chain, buffer_index);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateRenderTargetTexture");
    throw;
  }
}

ITexture* Device::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return render_target_manager.CreateDepthStencilTexture (swap_chain);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateDepthStencilTexture");
    throw;
  }
}

IView* Device::CreateView (ITexture* texture, const ViewDesc& desc)
{
  try
  {
    return render_target_manager.CreateView (texture, desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreateView");
    throw;
  }
}

void Device::OSSetRenderTargets (IView* render_target_view, IView* depth_stencil_view)
{
  render_target_manager.SetRenderTargets (render_target_view, depth_stencil_view);
}

IView* Device::OSGetRenderTargetView ()
{
  return render_target_manager.GetRenderTargetView ();
}

IView* Device::OSGetDepthStencilView ()
{
  return render_target_manager.GetDepthStencilView ();
}

void Device::ClearRenderTargetView (const Color4f& color)
{
  render_target_manager.ClearRenderTargetView (color);
}

void Device::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  render_target_manager.ClearDepthStencilView (clear_flags, depth, stencil);
}

void Device::ClearViews (size_t clear_flags, const Color4f& color, float depth, unsigned char stencil)
{
  render_target_manager.ClearViews (clear_flags, color, depth, stencil);
}

/*
    Управление предикатами отрисовки
*/

IPredicate* Device::CreatePredicate ()
{
  try
  {
    return query_manager.CreatePredicate ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::CreatePredicate");
    throw;
  }
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
      //установка состояния менеджера целевых буферов отрисовки
      
    if (context_manager.NeedStageRebind (Stage_RenderTargetManager))
    {
      render_target_manager.Bind ();

      context_manager.StageRebindNotify (Stage_Output); //сделать внутреннее кэширование состояния has_render_target/has_depth_stencil!!!
    }
    
      //установка состояния входного уровня

    if (context_manager.NeedStageRebind (Stage_Input) || base_vertex != cached_base_vertex || (base_index != cached_base_index && out_indices_layout))
    {
      input_stage.Bind (base_vertex, base_index, out_indices_layout);
      
      cached_base_vertex = base_vertex;

      if (out_indices_layout)
      {
        cached_indices_layout = *out_indices_layout;
        cached_base_index     = base_index;
      }
      else
      {
        cached_base_index = ~0;

        memset (&cached_indices_layout, 0, sizeof cached_indices_layout);
      }
    }
    else
    {
      if (out_indices_layout)
        *out_indices_layout = cached_indices_layout;
    }

      //установка состояния менеджера текстур

    if (context_manager.NeedStageRebind (Stage_TextureManager))
      texture_manager.Bind ();

      //установка состояния шейдерного уровня

    if (context_manager.NeedStageRebind (Stage_Shading))
      shader_stage.Bind ();            

      //установка состояния выходного уровня

    if (context_manager.NeedStageRebind (Stage_Output))
      output_stage.Bind (render_target_manager.GetRenderTargetView () != 0, render_target_manager.GetDepthStencilView () != 0);

      //очистка флагов ребиндинга

    context_manager.ResetRebindNotifications ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::Bind");
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
      throw xtl::make_argument_exception (source, "primitive_type", type);
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
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::Draw");

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
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::DrawIndexed");

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

    render_target_manager.UpdateRenderTargets ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::Flush");
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
