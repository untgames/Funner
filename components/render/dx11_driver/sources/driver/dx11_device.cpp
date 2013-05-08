#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::dx11;

/*
    Конструктор / деструктор
*/

Device::Device (const AdapterPtr& in_adapter, const char* init_string)
{
  try
  {
      //проверка корректности аргументов

    if (!init_string)
      init_string = "";

    if (!in_adapter)
      throw xtl::make_null_argument_exception ("", "adapter");

      //разбор строки инициализации

    common::PropertyMap init_properties = common::parse_init_string (init_string);

    bool is_debug = init_properties.IsPresent ("debug") && init_properties.GetInteger ("debug");

      //сохранение аргументов

    adapter = in_adapter;

      //создание устройства

    ID3D11Device*        dx_device = 0;
    ID3D11DeviceContext* dx_context = 0;
    D3D_FEATURE_LEVEL    feature_level;

    static const D3D_FEATURE_LEVEL feature_levels_requested [] = {
      D3D_FEATURE_LEVEL_11_0,
      D3D_FEATURE_LEVEL_10_1,
      D3D_FEATURE_LEVEL_10_0,
      D3D_FEATURE_LEVEL_9_3,
      D3D_FEATURE_LEVEL_9_2,
      D3D_FEATURE_LEVEL_9_1,
    };

    static const size_t feature_levels_requested_count = sizeof (feature_levels_requested) / sizeof (*feature_levels_requested);

    UINT flags = 0;

    if (is_debug) flags |= D3D11_CREATE_DEVICE_DEBUG;

    check_errors ("::D3D11CreateDevice", D3D11CreateDevice (adapter->GetModule () ? (IDXGIAdapter*)0 : &adapter->GetHandle (), 
      adapter->GetModule () ? D3D_DRIVER_TYPE_SOFTWARE : D3D_DRIVER_TYPE_UNKNOWN,
      adapter->GetModule (), flags, feature_levels_requested, feature_levels_requested_count, D3D11_SDK_VERSION, &dx_device, &feature_level, &dx_context));

    if (!dx_device || !dx_context)
      throw xtl::format_operation_exception ("", "::D3D11CreateDevice failed");

    device = DxDevicePtr (dx_device, false);
    
    DxContextPtr dx_immediate_context (dx_context, false);

    device_manager.reset (new DeviceManager (device));

      //инициализация подуровней

    render_target_manager.reset (new RenderTargetManager (*device_manager));
    texture_manager.reset       (new TextureManager (*device_manager));
    input_manager.reset         (new InputManager (*device_manager));
    shader_manager.reset        (new ShaderManager (*device_manager));
    output_manager.reset        (new OutputManager (*device_manager));

      //инициализация ресурсов по умолчанию

    InitDefaults ();

      //создание контекста

    immediate_context = ContextPtr (new Context (dx_immediate_context, *device_manager, shader_manager->GetShaderLibrary (), default_resources), false);      

      //создание отладочного уровня

    if (is_debug)
      StartDebugLayer ();

      //заполнение свойств

    properties.AddProperty ("init_string", init_string);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::Device");
    throw;
  }
}

Device::~Device ()
{
  StopDebugLayer ();
}

/*
    Инициализация ресурсов по умолчанию
*/

void Device::InitDefaults ()
{
  try
  {
      //создание входного лэйаута по умолчанию

    InputLayoutDesc default_layout_desc;
    
    memset (&default_layout_desc, 0 , sizeof default_layout_desc);

    VertexAttribute va;

    memset (&va, 0, sizeof (va));

    va.semantic = "POSITION";
    va.format   = InputDataFormat_Vector3;
    va.type     = InputDataType_Float;
    va.offset   = 0;
    va.stride   = sizeof (float) * 3;

    default_layout_desc.vertex_attributes       = &va;
    default_layout_desc.vertex_attributes_count = 1;
    default_layout_desc.index_type              = InputDataType_UShort;

    default_resources.input_layout = InputLayoutPtr (input_manager->CreateInputLayout (default_layout_desc), false);

      //создание программы по умолчанию

    ShaderDesc shader_descs [2];

    static const char* DEFAULT_VERTEX_SHADER = 
      "struct VS_INPUT   { float4 Position : POSITION; };\n"
      "struct VS_OUTPUT  { float4 Position : POSITION; };\n"

      "VS_OUTPUT main (in VS_INPUT In) {\n"
      "  VS_OUTPUT Out;\n"
      "  Out.Position = In.Position;\n"
      "  return Out;\n"
      "}\n";

    static const char* DEFAULT_PIXEL_SHADER = 
      "struct PS_INPUT {};\n"
      "float4 main (PS_INPUT Input) : SV_TARGET {\n"
      "    return float4 (1.0, 1, 1.0, 1.0);\n"
      "}\n";

    memset (&shader_descs, 0, sizeof (shader_descs));

    shader_descs [0].name             = "Default vertex shader";
    shader_descs [0].source_code_size = ~0;
    shader_descs [0].source_code      = DEFAULT_VERTEX_SHADER;
    shader_descs [0].profile          = "hlsl.vs";
    shader_descs [1].name             = "Default pixel shader";
    shader_descs [1].source_code_size = ~0;
    shader_descs [1].source_code      = DEFAULT_PIXEL_SHADER;
    shader_descs [1].profile          = "hlsl.ps";

    default_resources.program = IProgramPtr (CreateProgram (sizeof (shader_descs) / sizeof (*shader_descs), &shader_descs [0], LogFunction ()), false);    

      //инициализация BlendState
       
    BlendDesc blend_desc;
    
    memset (&blend_desc, 0, sizeof (blend_desc));

    blend_desc.render_target [0].blend_enable     = false;
    blend_desc.render_target [0].color_write_mask = ColorWriteFlag_All;
    blend_desc.independent_blend_enable           = false;

    default_resources.blend_state = IBlendStatePtr (output_manager->CreateBlendState (blend_desc), false);
    
    blend_desc.render_target [0].color_write_mask = 0;
    
    default_resources.null_blend_state = IBlendStatePtr (output_manager->CreateBlendState (blend_desc), false);

      //инициализация DepthStencilState
      
    DepthStencilDesc depth_stencil_desc;
    
    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));
    
    depth_stencil_desc.depth_test_enable  = true;
    depth_stencil_desc.depth_write_enable = true;
    depth_stencil_desc.depth_compare_mode = CompareMode_Less;
    
    default_resources.depth_stencil_state = IDepthStencilStatePtr (output_manager->CreateDepthStencilState (depth_stencil_desc), false);
    
    depth_stencil_desc.depth_test_enable  = false;
    depth_stencil_desc.depth_write_enable = false;
    depth_stencil_desc.depth_compare_mode = CompareMode_AlwaysPass;
    
    default_resources.null_depth_stencil_state = IDepthStencilStatePtr (output_manager->CreateDepthStencilState (depth_stencil_desc), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11:Device::InitDefaults");
    throw;
  }
}

void Device::StartDebugLayer ()
{
  try
  {
    StopDebugLayer ();

    debug_layer.reset (new DeviceDebugLayer (device));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::StartDebugLayer");
    throw;
  }
}

void Device::StopDebugLayer  ()
{
  try
  {
    debug_layer.reset ();
  }
  catch (...)
  {
  }
}

/*
    Получение дескриптора устройства / адаптера
*/

ID3D11Device& Device::GetHandle ()
{
  return *device;
}

AdapterPtr Device::GetAdapter ()
{
  return adapter;
}

/*
    Имя устройства
*/

const char* Device::GetName ()
{
  return "render::low_level::dx11::Device";
}

/*
    Получение возможностей устройства
*/

void Device::GetCaps (DeviceCaps& caps)
{
  try
  {
    memset (&caps, 0, sizeof (caps));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::GetCaps");
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
        return shader_manager->GetShaderProfilesString ();
      default:
        throw xtl::make_argument_exception ("", "cap_string", cap_string);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::GetCapString");
    throw;
  }
}

const char* Device::GetVertexAttributeSemanticName (VertexAttributeSemantic semantic)
{
  try
  {
    return InputManager::GetVertexAttributeSemanticName (semantic);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::GetVertexAttributeSemanticName");
    throw;
  }
}

/*
    Управление запросами
*/

IQuery* Device::CreateQuery (QueryType type)
{
  throw xtl::make_not_implemented_exception ("render::low_level::dx11::Device::CreateQuery");
}

/*
    Управление предикатами отрисовки
*/

IPredicate* Device::CreatePredicate ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreatePredicate");
    throw;
  }
}

/*
    Работа с блоками состояний
*/

IStateBlock* Device::CreateStateBlock (const StateBlockMask& mask)
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateStateBlock");
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
    return input_manager->CreateInputLayout (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateInputLayout");
    throw;
  }
}

IBuffer* Device::CreateBuffer (const BufferDesc& desc)
{
  static const char* METHOD_NAME = "render::low_level::dx11::Device::CreateBuffer";

  try
  {
    static const size_t BAD_FLAGS = BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil;

    if (desc.bind_flags & BAD_FLAGS)
      throw xtl::make_argument_exception ("", "desc.bind_flags", get_name ((BindFlag)desc.bind_flags));

    switch (desc.bind_flags)
    {
      case BindFlag_VertexBuffer:
      case BindFlag_IndexBuffer:    return input_manager->CreateBuffer (desc);
      case BindFlag_ConstantBuffer: return shader_manager->CreateConstantBuffer (desc);
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

/*
    Управление шейдерными уровнями (shader-stage)
*/

IProgramParametersLayout* Device::CreateProgramParametersLayout (const ProgramParametersLayoutDesc& desc)
{
  try
  {
    return shader_manager->CreateProgramParametersLayout (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateProgramParametersLayout");
    throw;
  }
}

IProgram* Device::CreateProgram (size_t shaders_count, const ShaderDesc* shader_descs, const LogFunction& error_log)
{
  try
  {
    return shader_manager->CreateProgram (shaders_count, shader_descs, error_log);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateProgram");
    throw;
  }
}

/*
    Управление текстурами
*/

ISamplerState* Device::CreateSamplerState (const SamplerDesc& desc)
{
  try
  {
    return texture_manager->CreateSamplerState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateSamplerState");
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
  try
  {
    if (!(desc.bind_flags & (BindFlag_Texture | BindFlag_RenderTarget | BindFlag_DepthStencil)))
      throw xtl::format_not_supported_exception ("", "Unsupported bindable flags desc.bind_flags=%s", get_name ((BindFlag)desc.bind_flags));

    return texture_manager->CreateTexture (desc, data);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::CreateTexture");
    throw;
  }
}

ITexture* Device::CreateTexture (const TextureDesc&, IBuffer* buffer, size_t buffer_offset, const size_t* mip_sizes)
{
  throw xtl::make_not_implemented_exception ("render::low_level::dx11::Device::CreateTexture");
}

/*
    Управление растеризатором (rasterizer-stage)
*/

IRasterizerState* Device::CreateRasterizerState (const RasterizerDesc& desc)
{
  try
  {
    return output_manager->CreateRasterizerState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateRasterizerState");
    throw;
  }
}

/*
    Управление выходным уровнем (output-stage)
*/

IBlendState* Device::CreateBlendState (const BlendDesc& desc)
{
  try
  {
    return output_manager->CreateBlendState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateBlendState");
    throw;
  }
}

IDepthStencilState* Device::CreateDepthStencilState (const DepthStencilDesc& desc)
{
  try
  {
    return output_manager->CreateDepthStencilState (desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateDepthStencilState");
    throw;
  }
}

/*
    Управление менеджером целевых буферов отрисовки
*/

ITexture* Device::CreateRenderTargetTexture (ISwapChain* swap_chain, size_t buffer_index)
{
  try
  {
    return render_target_manager->CreateRenderTargetTexture (swap_chain, buffer_index);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateRenderTargetTexture");
    throw;
  }
}

ITexture* Device::CreateDepthStencilTexture (ISwapChain* swap_chain)
{
  try
  {
    return render_target_manager->CreateDepthStencilTexture (swap_chain);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateDepthStencilTexture");
    throw;
  }
}

IView* Device::CreateView (ITexture* texture, const ViewDesc& desc)
{
  try
  {
    return render_target_manager->CreateView (texture, desc);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::dx11::Device::CreateView");
    throw;
  }
}

/*
    Создание контекста
*/

IDeviceContext* Device::CreateDeferredContext ()
{
  try
  {
    throw xtl::make_not_implemented_exception (__FUNCTION__); //can't be implemented only via DX deferred contexts (need MT guard)
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::low_level::dx11::Device::CreateDeferredContext");
    throw;
  }
}

/*
    Получение непосредственного контекста выполнения операций
*/

IDeviceContext* Device::GetImmediateContext ()
{
  return &*immediate_context;
}

/*
    Список свойств устройства отрисовки
*/

IPropertyList* Device::GetProperties ()
{
  return &properties;
}
