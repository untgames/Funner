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
    cached_base_index (~0),
    need_validate_programs (context_manager.NeedValidatePrograms ())
{  
    //инициализация кэша
    
  memset (&cached_indices_layout, 0, sizeof cached_indices_layout);

    //получение информации об устройстве отрисовки

  properties.AddProperty ("init_string",   init_string);
  properties.AddProperty ("gl_vendor",     context_manager.GetVendor ());
  properties.AddProperty ("gl_renderer",   context_manager.GetRenderer ());
  properties.AddProperty ("gl_version",    context_manager.GetVersion ());
  properties.AddProperty ("gl_extensions", context_manager.GetExtensions ());
  
  stl::string prefix = common::format ("GlDevice%02u.", GetId ());
  
  syslib::Application::SetSystemProperty ((prefix + "Vendor").c_str (), context_manager.GetVendor ());
  syslib::Application::SetSystemProperty ((prefix + "Renderer").c_str (), context_manager.GetRenderer ());
  syslib::Application::SetSystemProperty ((prefix + "Version").c_str (), context_manager.GetVersion ());
  syslib::Application::SetSystemProperty ((prefix + "Extensions").c_str (), context_manager.GetExtensions ());
}

Device::~Device ()
{
  try
  {
    stl::string prefix = common::format ("GlDevice%02u.", GetId ());
    
    syslib::Application::RemoveSystemProperty ((prefix + "Vendor").c_str ());
    syslib::Application::RemoveSystemProperty ((prefix + "Renderer").c_str ());
    syslib::Application::RemoveSystemProperty ((prefix + "Version").c_str ());
    syslib::Application::RemoveSystemProperty ((prefix + "Extensions").c_str ());
  }
  catch (...)
  {
  }
}

/*
    Имя устройства
*/

const char* Device::GetName ()
{
  return "render::low_level::opengl::Device";
}

/*
    Получение адаптера
*/

IAdapter* Device::GetAdapter ()
{
  return context_manager.GetAdapter ();
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

const char* Device::GetVertexAttributeSemanticName (VertexAttributeSemantic semantic)
{
  try
  {
    return InputStage::GetVertexAttributeSemanticName (semantic);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Device::GetVertexAttributeSemanticName");
    throw;
  }
}

/*
    Управление запросами
*/

IQuery* Device::CreateQuery (QueryType type)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::CreateQuery");
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

IBuffer* Device::CreateBuffer (const BufferDesc& desc, const void* data)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Device::CreateBuffer";

  try
  {
    static const size_t BAD_FLAGS = BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil;

    if (desc.bind_flags & BAD_FLAGS)
      throw xtl::make_argument_exception ("", "desc.bind_flags", get_name ((BindFlag)desc.bind_flags));

    xtl::com_ptr<IBuffer> buffer;

    switch (desc.bind_flags)
    {
      case BindFlag_VertexBuffer:
        buffer = xtl::com_ptr<IBuffer> (input_stage.CreateVertexBuffer (desc), false);
        break;
      case BindFlag_IndexBuffer:
        buffer = xtl::com_ptr<IBuffer> (input_stage.CreateIndexBuffer (desc), false);
        break;
      case BindFlag_ConstantBuffer:
        buffer = xtl::com_ptr<IBuffer> (input_stage.CreateConstantBuffer (desc), false);
        break;
      default:
        throw xtl::format_not_supported_exception ("", "Incompatible desc.bind_flags=%s", get_name ((BindFlag)desc.bind_flags));
    }

    if (data)
      buffer->SetData (0, desc.size, data);

    buffer->AddRef ();

    return buffer.get ();
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
    Управление уровнем вывода вершин
*/

void Device::SOSetTargets (size_t buffers_count, IBuffer** buffers, const size_t* offsets)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::SOSetTargets");
}

void Device::SOSetTarget (size_t stream_output_slot, IBuffer* buffer, size_t offset)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::SOSetTarget");
}

IBuffer* Device::SOGetTarget (size_t stream_output_slot)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::SOGetTarget");
}

size_t Device::SOGetTargetOffset (size_t stream_output_slot)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::SOGetTargetOffset");
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

ITexture* Device::CreateTexture (const TextureDesc&, IBuffer* buffer, size_t buffer_offset, const size_t* mip_sizes)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::CreateTexture");
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

void Device::RSSetViewport (size_t render_target_slot, const Viewport& viewport)
{
  render_target_manager.SetViewport (render_target_slot, viewport);
}

void Device::RSSetScissor (size_t render_target_slot, const Rect& scissor_rect)
{
  render_target_manager.SetScissor (render_target_slot, scissor_rect);
}

IRasterizerState* Device::RSGetState ()
{
  return output_stage.GetRasterizerState ();
}

const Viewport& Device::RSGetViewport (size_t render_target_slot)
{
  return render_target_manager.GetViewport (render_target_slot);
}

const Rect& Device::RSGetScissor (size_t render_target_slot)
{
  return render_target_manager.GetScissor (render_target_slot);
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

void Device::OSSetRenderTargets (size_t count, IView** render_target_views, IView* depth_stencil_view)
{
  try
  {
    if (!render_target_views && count)
      throw xtl::make_null_argument_exception ("", "render_target_views");

    for (size_t i=0; i<count; i++)
      render_target_manager.SetRenderTargetView (i, render_target_views [i]);

    for (size_t i=count; i<DEVICE_RENDER_TARGET_SLOTS_COUNT; i++)
      render_target_manager.SetRenderTargetView (i, 0);

    render_target_manager.SetDepthStencilView (depth_stencil_view);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::opengl::Device::OSSetRenderTargets");
    throw;
  }
}

void Device::OSSetRenderTargetView (size_t render_target_slot, IView* render_target_view)
{
  render_target_manager.SetRenderTargetView (render_target_slot, render_target_view);
}

void Device::OSSetDepthStencilView (IView* depth_stencil_view)
{
  render_target_manager.SetDepthStencilView (depth_stencil_view);
}

IView* Device::OSGetRenderTargetView (size_t render_target_slot)
{
  return render_target_manager.GetRenderTargetView (render_target_slot);
}

IView* Device::OSGetDepthStencilView ()
{
  return render_target_manager.GetDepthStencilView ();
}

void Device::ClearRenderTargetView (size_t view_index, const Color4f& color)
{
  render_target_manager.ClearRenderTargetView (view_index, color);
}

void Device::ClearDepthStencilView (size_t clear_flags, float depth, unsigned char stencil)
{
  render_target_manager.ClearDepthStencilView (clear_flags, depth, stencil);
}

void Device::ClearViews (size_t clear_flags, size_t views_count, const size_t* view_indices, const Color4f* colors, float depth, unsigned char stencil)
{
  render_target_manager.ClearViews (clear_flags, views_count, view_indices, colors, depth, stencil);
}

/*
   Генерация мип-уровней текстуры (необходимо для текстур в которые ведется рендеринг)
*/

void Device::GenerateMips (ITexture* texture)
{
  try
  {
    texture_manager.GenerateMips (texture);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::Device::GenerateMips");
    throw;
  }
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
    Создание контекста
*/

IDeviceContext* Device::CreateDeferredContext ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::CreateDeferredContext");
}

/*
    
*/

ICommandList* Device::FinishCommandList (bool restore_state)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::FinishCommandList");
}

void Device::ExecuteCommandList (ICommandList* list, bool restore_state)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::ExecuteCommandList");
}

/*
    Установка состояния устройства в контекст OpenGL
*/

void Device::Bind (size_t base_vertex, size_t base_index, IndicesLayout* out_indices_layout)
{
  try
  {
    bool is_program_validate_needed = false;
    
      //установка состояния менеджера целевых буферов отрисовки
      
    if (context_manager.NeedStageRebind (Stage_RenderTargetManager))
    {
      render_target_manager.Bind ();

      context_manager.StageRebindNotify (Stage_Output); //сделать внутреннее кэширование состояния has_render_target/has_depth_stencil!!!
      
      is_program_validate_needed = true;
    }   

      //установка состояния менеджера текстур

    if (context_manager.NeedStageRebind (Stage_TextureManager))
    {
      texture_manager.Bind ();
      
      is_program_validate_needed = true;      
    }

      //установка состояния шейдерного уровня

    if (context_manager.NeedStageRebind (Stage_Shading))
    {
      shader_stage.Bind ();            
      
      is_program_validate_needed = true;      
    }

      //установка состояния входного уровня

    if (context_manager.NeedStageRebind (Stage_Input) || base_vertex != cached_base_vertex || (base_index != cached_base_index && out_indices_layout))
    {
      input_stage.Bind (base_vertex, base_index, shader_stage.GetVertexAttributeDictionary (), out_indices_layout);
      
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
      
      is_program_validate_needed = true;      
    }
    else
    {
      if (out_indices_layout)
        *out_indices_layout = cached_indices_layout;
    }

      //установка состояния выходного уровня

    if (context_manager.NeedStageRebind (Stage_Output))
    {
      bool has_render_targets [DEVICE_RENDER_TARGET_SLOTS_COUNT];

      render_target_manager.HasRenderTargetViews (has_render_targets);

      output_stage.Bind (has_render_targets, render_target_manager.GetDepthStencilView () != 0);
      
      is_program_validate_needed = true;      
    }

      //очистка флагов ребиндинга

    context_manager.ResetRebindNotifications ();
    
      //валидация текущей программы
      
    if (need_validate_programs && is_program_validate_needed)
      shader_stage.ValidateBindedProgram ();
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

void Device::DrawInstanced (PrimitiveType primitive_type, size_t vertex_count_per_instance, size_t instance_count, size_t first_vertex, size_t first_instance_location)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::DrawInstanced");
}

void Device::DrawIndexedInstanced (PrimitiveType primitive_type, size_t index_count_per_instance, size_t instance_count, size_t first_index, size_t base_vertex, size_t first_instance_location)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::DrawIndexedInstanced");
}

void Device::DrawAuto (PrimitiveType primitive_type)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::Device::DrawAuto");
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
