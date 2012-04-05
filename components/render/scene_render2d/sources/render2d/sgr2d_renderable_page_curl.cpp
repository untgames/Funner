#include "shared.h"

using namespace render;
using namespace render::mid_level;
using namespace render::obsolete::render2d;
using namespace scene_graph;

typedef xtl::com_ptr<low_level::IBlendState>              BlendStatePtr;
typedef xtl::com_ptr<low_level::IBuffer>                  BufferPtr;
typedef xtl::com_ptr<low_level::IDepthStencilState>       DepthStencilStatePtr;
typedef xtl::com_ptr<ILowLevelFrame>                      LowLevelFramePtr;
typedef xtl::com_ptr<ILowLevelTexture>                    LowLevelTexturePtr;
typedef xtl::com_ptr<low_level::IInputLayout>             InputLayoutPtr;
typedef xtl::com_ptr<low_level::IProgram>                 ProgramPtr;
typedef xtl::com_ptr<low_level::IProgramParametersLayout> ProgramParametersLayoutPtr;
typedef xtl::com_ptr<low_level::IRasterizerState>         RasterizerStatePtr;
typedef xtl::com_ptr<low_level::ISamplerState>            SamplerStatePtr;
typedef xtl::com_ptr<low_level::IStateBlock>              StateBlockPtr;
typedef xtl::com_ptr<low_level::ITexture>                 DeviceTexturePtr;
typedef xtl::intrusive_ptr<RenderablePageCurlMesh>        RenderablePageCurlMeshPtr;

namespace
{

const char* LOG_NAME = "render.obsolete.render2d.RenderablePageCurl";

const float  EPS                   = 0.001f;
const float  BACK_SHADOW_OFFSET    = 0;
const float  MIN_SHADOW_LOG_VALUE  = 0.4;
const float  PI                    = 3.1415926f;
const size_t SHADOW_TEXTURE_SIZE   = 32;
const size_t SHADOW_VERTICES_COUNT = 16;
const float  STATIC_PAGES_Z_OFFSET = -0.001f;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметры вершины необходимые для визуализации
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderableVertex
{
  math::vec3f  position; //положение вершины в пространстве
  math::vec2f  texcoord; //текстурные координаты
  math::vec4ub color;    //цвет вершины
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметры шейдера
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ProgramParameters
{
  math::mat4f view_matrix;       //матрица вида
  math::mat4f projection_matrix; //матрица проекции
  math::mat4f object_matrix;     //матрица объекта
};

//текст шейдера программы по умолчанию
const char* DEFAULT_SHADER_SOURCE_CODE =
"Parameters\n"
"{\n"
"  float4x4 currentViewMatrix currentProjMatrix objMatrix\n"
"}\n"
"ObjectMatrix     objMatrix\n"
"ProjectionMatrix currentProjMatrix\n"
"ViewMatrix       currentViewMatrix\n"
"AlphaCompareMode AlwaysPass\n"
"Texmap0\n"
"{\n"
"  TexcoordU Explicit\n"
"  TexcoordV Explicit\n"
"  Blend     Modulate\n"
"}";

//протоколирование компилятора шейдеров
void shader_error_log (const char* message)
{
  static common::Log log (LOG_NAME);

  log.Print (message);
}

}

/*
    Описание реализации листания страницы
*/

struct RenderablePageCurl::Impl : public ILowLevelFrame::IDrawCallback
{
  Render&                    render;                            //ссылка на рендер
  scene_graph::PageCurl*     page_curl;                         //исходный узел
  Renderable*                renderable;                        //объект
  DeviceTexturePtr           shadow_texture;                    //текстура тени
  LowLevelTexturePtr         page_textures [scene_graph::PageCurlPageType_Num];                     //текстуры страниц
  SpriteMaterialPtr          page_materials [scene_graph::PageCurlPageType_Num];                    //материалы страниц
  size_t                     current_page_material_name_hashes [scene_graph::PageCurlPageType_Num]; //хэш текущего имени материалов страниц
  LowLevelFramePtr           low_level_frame;                    //фрейм кастомной отрисовки
  BlendStatePtr              none_blend_state;                   //состояния блендинга
  BlendStatePtr              translucent_blend_state;            //состояния блендинга
  BlendStatePtr              mask_blend_state;                   //состояния блендинга
  BlendStatePtr              additive_blend_state;               //состояния блендинга
  InputLayoutPtr             input_layout;                       //состояние устройства отрисовки
  ProgramPtr                 default_program;                    //шейдер
  ProgramParametersLayoutPtr program_parameters_layout;          //расположение параметров шейдера
  SamplerStatePtr            sampler_state;                      //сэмплер
  RasterizerStatePtr         rasterizer_no_cull_state;           //состояние растеризатора без отсечения
  RasterizerStatePtr         rasterizer_cull_back_state;         //состояние растеризатора с отсечением задних сторон треугольников
  RasterizerStatePtr         rasterizer_cull_front_state;        //состояние растеризатора с отсечением передних сторон треугольников
  RasterizerStatePtr         rasterizer_scissor_enabled_state;   //состояние растеризатора с областью отсечением
  DepthStencilStatePtr       depth_stencil_state_write_enabled;  //состояние буфера попиксельного отсечения
  DepthStencilStatePtr       depth_stencil_state_write_disabled; //состояние буфера попиксельного отсечения
  BufferPtr                  constant_buffer;                    //константный буфер
  BufferPtr                  quad_vertex_buffer;                 //вершинный буфер на два треугольника
  BufferPtr                  shadow_vertex_buffer;               //вершинный буфер для отрисовки тени
  BufferPtr                  corner_shadow_index_buffer;         //индексный буфер для отрисовки тени
  BufferPtr                  page_shadow_index_buffer;           //индексный буфер для отрисовки тени
  StateBlockPtr              render_state;                       //сохранение состояния рендера
  math::vec3f                view_point;                         //позиция камеры
  math::mat4f                projection;                         //матрица камеры
  mid_level::Viewport        viewport;                           //область вывода
  RenderablePageCurlMeshPtr  curled_page;                        //сетка изгибаемой страницы
  math::vec2ui               current_page_grid_size;             //текущий размер сетки страницы
  bool                       initialized;                        //инициализированы ли необходимые поля

  ///Конструктор
  Impl (scene_graph::PageCurl* in_page_curl, Render& in_render, Renderable* in_renderable)
    : render (in_render)
    , page_curl (in_page_curl)
    , renderable (in_renderable)
    , initialized (false)
  {
    ILowLevelRenderer* low_level_renderer = dynamic_cast<ILowLevelRenderer*> (render.Renderer ().get ());

    if (!low_level_renderer)
      throw xtl::format_operation_exception ("render::obsolete::render2d::RenderablePageCurl::RenderablePageCurl", "Can't cast render to ILowLevelRenderer");

    memset (current_page_material_name_hashes, 0, sizeof (current_page_material_name_hashes));

    low_level_frame = LowLevelFramePtr (low_level_renderer->CreateFrame (), false);

    low_level_frame->SetCallback (this);
  }

  ILowLevelTexture* GetLowLevelTexture (const char* image_name)
  {
    static const char* METHOD_NAME = "render::obsolete::render2d::RenderablePageCurl::GetLowLevelTexture";

    ILowLevelTexture* texture = dynamic_cast<ILowLevelTexture*> (render.GetTexture (image_name, false, renderable));

    if (!texture)
      throw xtl::format_operation_exception (METHOD_NAME, "Material '%s' has unsupported texture type, only ILowLevelTexture supported", image_name);

    return texture;
  }

  //Обновление
  void Update ()
  {
    for (size_t i = 0; i < scene_graph::PageCurlPageType_Num; i++)
    {
      const char* material_name = page_curl->PageMaterial ((scene_graph::PageCurlPageType)i);

      size_t material_name_hash = xtl::xstrlen (material_name) ? common::strhash (material_name) : 0;

      if (material_name_hash != current_page_material_name_hashes [i])
      {
        if (material_name_hash)
        {
          page_materials [i] = render.GetMaterial (material_name);
          page_textures [i]  = GetLowLevelTexture (page_materials [i]->Image ());
        }
        else
        {
          page_textures [i]  = 0;
          page_materials [i] = 0;
        }

        current_page_material_name_hashes [i] = material_name_hash;
      }
    }
  }

  //создание состояния смешивания цветов
  BlendStatePtr CreateBlendState
   (low_level::IDevice&      device,
    bool                     blend_enable,
    low_level::BlendArgument src_arg,
    low_level::BlendArgument dst_color_arg,
    low_level::BlendArgument dst_alpha_arg)
  {
    low_level::BlendDesc blend_desc;

    memset (&blend_desc, 0, sizeof (blend_desc));

    blend_desc.blend_enable                     = blend_enable;
    blend_desc.blend_color_operation            = low_level::BlendOperation_Add;
    blend_desc.blend_alpha_operation            = low_level::BlendOperation_Add;
    blend_desc.blend_color_source_argument      = src_arg;
    blend_desc.blend_color_destination_argument = dst_color_arg;
    blend_desc.blend_alpha_source_argument      = src_arg;
    blend_desc.blend_alpha_destination_argument = dst_alpha_arg;
    blend_desc.color_write_mask                 = low_level::ColorWriteFlag_All;

    return BlendStatePtr (device.CreateBlendState (blend_desc), false);
  }

  //создание объекта расположения входной геометрии
  InputLayoutPtr CreateInputLayout (low_level::IDevice& device)
  {
    low_level::VertexAttribute attributes [3];

    memset (attributes, 0, sizeof (attributes));

    attributes [0].semantic = low_level::VertexAttributeSemantic_Position;
    attributes [0].format   = low_level::InputDataFormat_Vector3;
    attributes [0].type     = low_level::InputDataType_Float;
    attributes [0].slot     = 0;
    attributes [0].offset   = offsetof (RenderableVertex, position);
    attributes [0].stride   = sizeof (RenderableVertex);

    attributes [1].semantic = low_level::VertexAttributeSemantic_TexCoord0;
    attributes [1].format   = low_level::InputDataFormat_Vector2;
    attributes [1].type     = low_level::InputDataType_Float;
    attributes [1].slot     = 0;
    attributes [1].offset   = offsetof (RenderableVertex, texcoord);
    attributes [1].stride   = sizeof (RenderableVertex);

    attributes [2].semantic = low_level::VertexAttributeSemantic_Color;
    attributes [2].format   = low_level::InputDataFormat_Vector4;
    attributes [2].type     = low_level::InputDataType_UByte;
    attributes [2].slot     = 0;
    attributes [2].offset   = offsetof (RenderableVertex, color);
    attributes [2].stride   = sizeof (RenderableVertex);

    low_level::InputLayoutDesc layout_desc;

    memset (&layout_desc, 0, sizeof layout_desc);

    layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
    layout_desc.vertex_attributes       = attributes;
    layout_desc.index_type              = low_level::InputDataType_UShort;
    layout_desc.index_buffer_offset     = 0;

    return InputLayoutPtr (device.CreateInputLayout (layout_desc), false);
  }

  //создание программы
  ProgramPtr CreateProgram (low_level::IDevice& device, const char* shader_name, const char* shader_source)
  {
    low_level::ShaderDesc shader_desc;

    memset (&shader_desc, 0, sizeof (shader_desc));

    shader_desc.name             = shader_name;
    shader_desc.source_code_size = strlen (shader_source);
    shader_desc.source_code      = shader_source;
    shader_desc.profile          = "ffp";
    shader_desc.options          = "";

    return ProgramPtr (device.CreateProgram (1, &shader_desc, &shader_error_log), false);
  }

  //создание сэмплера
  SamplerStatePtr CreateSampler (low_level::IDevice& device)
  {
    low_level::SamplerDesc sampler_desc;

    memset (&sampler_desc, 0, sizeof (sampler_desc));

#ifdef ARM
    sampler_desc.min_filter           = low_level::TexMinFilter_Linear;
#else
    sampler_desc.min_filter           = low_level::TexMinFilter_LinearMipLinear;
#endif    
    sampler_desc.mag_filter           = low_level::TexMagFilter_Linear;
    sampler_desc.max_anisotropy       = 1;
    sampler_desc.wrap_u               = low_level::TexcoordWrap_Clamp;
    sampler_desc.wrap_v               = low_level::TexcoordWrap_Clamp;
    sampler_desc.comparision_function = low_level::CompareMode_AlwaysPass;
    sampler_desc.min_lod              = 0;
    sampler_desc.max_lod              = FLT_MAX;

    return SamplerStatePtr (device.CreateSamplerState (sampler_desc), false);
  }

  //создание состояния расположения параметров шейдинга
  ProgramParametersLayoutPtr CreateProgramParametersLayout (low_level::IDevice& device)
  {
    low_level::ProgramParameter program_parameters [3];

    memset (program_parameters, 0, sizeof (program_parameters));

    program_parameters [0].name   = "currentViewMatrix";
    program_parameters [0].type   = low_level::ProgramParameterType_Float4x4;
    program_parameters [0].slot   = 0;
    program_parameters [0].count  = 1;
    program_parameters [0].offset = offsetof (ProgramParameters, view_matrix);

    program_parameters [1].name   = "currentProjMatrix";
    program_parameters [1].type   = low_level::ProgramParameterType_Float4x4;
    program_parameters [1].slot   = 0;
    program_parameters [1].count  = 1;
    program_parameters [1].offset = offsetof (ProgramParameters, projection_matrix);

    program_parameters [2].name   = "objMatrix";
    program_parameters [2].type   = low_level::ProgramParameterType_Float4x4;
    program_parameters [2].slot   = 0;
    program_parameters [2].count  = 1;
    program_parameters [2].offset = offsetof (ProgramParameters, object_matrix);

    low_level::ProgramParametersLayoutDesc program_parameters_layout_desc;

    memset (&program_parameters_layout_desc, 0, sizeof (program_parameters_layout_desc));

    program_parameters_layout_desc.parameters_count = sizeof (program_parameters) / sizeof (*program_parameters);
    program_parameters_layout_desc.parameters       = program_parameters;

    return ProgramParametersLayoutPtr (device.CreateProgramParametersLayout (program_parameters_layout_desc), false);
  }

  //создание константного буфера
  BufferPtr CreateConstantBuffer (low_level::IDevice& device)
  {
    low_level::BufferDesc constant_buffer_desc;

    memset (&constant_buffer_desc, 0, sizeof (constant_buffer_desc));

    constant_buffer_desc.size         = sizeof (ProgramParameters);
    constant_buffer_desc.usage_mode   = low_level::UsageMode_Dynamic;
    constant_buffer_desc.bind_flags   = low_level::BindFlag_ConstantBuffer;
    constant_buffer_desc.access_flags = low_level::AccessFlag_ReadWrite;

    return BufferPtr (device.CreateBuffer (constant_buffer_desc), false);
  }

  //создание состояния уровня растеризации
  RasterizerStatePtr CreateRasterizerState (low_level::IDevice& device, low_level::CullMode cull_mode, bool scissor_enable = false)
  {
    low_level::RasterizerDesc rasterizer_desc;

    memset (&rasterizer_desc, 0, sizeof (rasterizer_desc));

    rasterizer_desc.fill_mode               = low_level::FillMode_Solid;
    rasterizer_desc.cull_mode               = cull_mode;
    rasterizer_desc.front_counter_clockwise = true;
    rasterizer_desc.depth_bias              = 0;
    rasterizer_desc.scissor_enable          = scissor_enable;
    rasterizer_desc.multisample_enable      = false;
    rasterizer_desc.antialiased_line_enable = false;

    return RasterizerStatePtr (device.CreateRasterizerState (rasterizer_desc), false);
  }

  //создание состояния буфера попиксельного отсечения
  DepthStencilStatePtr CreateDepthStencilState (low_level::IDevice& device, bool write_enabled)
  {
    low_level::DepthStencilDesc depth_stencil_desc;

    memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));

    depth_stencil_desc.depth_test_enable   = true;
    depth_stencil_desc.depth_write_enable  = write_enabled;
    depth_stencil_desc.depth_compare_mode  = low_level::CompareMode_Less;

    return DepthStencilStatePtr (device.CreateDepthStencilState (depth_stencil_desc), false);
  }

  //Получение свойств
  math::vec4ub GetPageColor ()
  {
    const math::vec4f& float_page_color = page_curl->PageColor ();

    return math::vec4ub ((unsigned char)(float_page_color.x * 255), (unsigned char)(float_page_color.y * 255), (unsigned char)(float_page_color.z * 255), (unsigned char)(float_page_color.w * 255));
  }

  void GetTexCoords (bool left, float& min_s, float& max_s)
  {
    switch (page_curl->Mode ())
    {
      case PageCurlMode_SinglePage:
      case PageCurlMode_DoublePageDoubleMaterial:
        min_s = 0.f;
        max_s = 1.f;
        break;
      case PageCurlMode_DoublePageSingleMaterial:
        if (left)
        {
          min_s = 0.f;
          max_s = 0.5f;
        }
        else
        {
          min_s = 0.5f;
          max_s = 1.f;
        }
        break;
    }
  }

  scene_graph::PageCurlPageType GetCurledRightPageType ()
  {
    switch (page_curl->Mode ())
    {
      case PageCurlMode_SinglePage:
      case PageCurlMode_DoublePageSingleMaterial:
        return PageCurlPageType_Back;
      default:
        return PageCurlPageType_BackRight;
    }
  }

  scene_graph::PageCurlPageType GetCurledLeftPageType ()
  {
    switch (page_curl->Mode ())
    {
      case PageCurlMode_SinglePage:
        return PageCurlPageType_Back;
      case PageCurlMode_DoublePageSingleMaterial:
        return PageCurlPageType_Front;
      default:
        return PageCurlPageType_FrontLeft;
    }
  }

  //Отрисовка
  void Draw (low_level::IDevice& device)
  {
    if (!initialized)
    {
      none_blend_state                   = CreateBlendState              (device, false, low_level::BlendArgument_Zero, low_level::BlendArgument_Zero, low_level::BlendArgument_Zero);
      translucent_blend_state            = CreateBlendState              (device, true, low_level::BlendArgument_SourceAlpha, low_level::BlendArgument_InverseSourceAlpha, low_level::BlendArgument_InverseSourceAlpha);
      mask_blend_state                   = CreateBlendState              (device, true, low_level::BlendArgument_Zero, low_level::BlendArgument_SourceColor, low_level::BlendArgument_SourceAlpha);
      additive_blend_state               = CreateBlendState              (device, true, low_level::BlendArgument_SourceAlpha, low_level::BlendArgument_One, low_level::BlendArgument_One);
      input_layout                       = CreateInputLayout             (device);
      default_program                    = CreateProgram                 (device, "render.obsolete.renderer2d.RenderablePageCurl.default_program", DEFAULT_SHADER_SOURCE_CODE);
      program_parameters_layout          = CreateProgramParametersLayout (device);
      constant_buffer                    = CreateConstantBuffer          (device);
      rasterizer_no_cull_state           = CreateRasterizerState         (device, low_level::CullMode_None);
      rasterizer_cull_front_state        = CreateRasterizerState         (device, low_level::CullMode_Front);
      rasterizer_cull_back_state         = CreateRasterizerState         (device, low_level::CullMode_Back);
      rasterizer_scissor_enabled_state   = CreateRasterizerState         (device, low_level::CullMode_None, true);
      depth_stencil_state_write_enabled  = CreateDepthStencilState       (device, true);
      depth_stencil_state_write_disabled = CreateDepthStencilState       (device, false);
      sampler_state                      = CreateSampler                 (device);

      low_level::StateBlockMask state_block_mask;

      state_block_mask.Set ();

      render_state = StateBlockPtr (device.CreateStateBlock (state_block_mask), false);

      low_level::BufferDesc buffer_desc;

      memset (&buffer_desc, 0, sizeof buffer_desc);

      buffer_desc.usage_mode   = low_level::UsageMode_Stream;
      buffer_desc.bind_flags   = low_level::BindFlag_VertexBuffer;
      buffer_desc.access_flags = low_level::AccessFlag_Write;
      buffer_desc.size         = sizeof (RenderableVertex) * 4;

      quad_vertex_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

      memset (&buffer_desc, 0, sizeof buffer_desc);

      buffer_desc.usage_mode   = low_level::UsageMode_Stream;
      buffer_desc.bind_flags   = low_level::BindFlag_VertexBuffer;
      buffer_desc.access_flags = low_level::AccessFlag_Write;
      buffer_desc.size         = sizeof (RenderableVertex) * SHADOW_VERTICES_COUNT;

      shadow_vertex_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

        //создание индексного буфера

      memset (&buffer_desc, 0, sizeof buffer_desc);

      buffer_desc.usage_mode   = low_level::UsageMode_Stream;
      buffer_desc.bind_flags   = low_level::BindFlag_IndexBuffer;
      buffer_desc.access_flags = low_level::AccessFlag_Write;
      buffer_desc.size         = sizeof (unsigned short) * 39;

      corner_shadow_index_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

      unsigned short corner_shadow_indices [39] = { 0,  1,  2,    1,  2,  3,    2,  3,  4,    3,  4,  5,
                                                    3,  5,  6,    3,  6,  7,    6,  7,  8,    7,  8,  9,
                                                    1,  3,  7,    0,  1,  9,    1,  7,  9,    0,  2, 10,
                                                    8,  9, 11 };

      corner_shadow_index_buffer->SetData (0, sizeof (corner_shadow_indices), corner_shadow_indices);

      memset (&buffer_desc, 0, sizeof buffer_desc);

      buffer_desc.usage_mode   = low_level::UsageMode_Stream;
      buffer_desc.bind_flags   = low_level::BindFlag_IndexBuffer;
      buffer_desc.access_flags = low_level::AccessFlag_Write;
      buffer_desc.size         = sizeof (unsigned short) * 54;

      page_shadow_index_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

      unsigned short page_shadow_indices [54] = { 0,  1,  2,    1,  2,  3,    2,  3,  4,    3,  4,  5,
                                                  3,  5,  6,    3,  6,  7,    6,  7,  8,    7,  8,  9,
                                                  8,  9, 10,    7,  9, 11,    9, 11, 12,   11, 12, 13,
                                                  1,  3,  7,    1,  7, 11,    0,  1, 11,    0, 11, 13,
                                                  0,  2, 14,   12, 13, 15 };

      page_shadow_index_buffer->SetData (0, sizeof (page_shadow_indices), page_shadow_indices);

        //создание текстуры тени

      low_level::TextureDesc texture_desc;

      memset (&texture_desc, 0, sizeof (texture_desc));

      texture_desc.dimension            = low_level::TextureDimension_2D;
      texture_desc.width                = SHADOW_TEXTURE_SIZE;
      texture_desc.height               = SHADOW_TEXTURE_SIZE;
      texture_desc.layers               = 1;
      texture_desc.format               = low_level::PixelFormat_L8;
      texture_desc.access_flags         = low_level::AccessFlag_ReadWrite;
      texture_desc.bind_flags           = low_level::BindFlag_Texture;
#ifndef ARM
      texture_desc.generate_mips_enable = true;
#endif

      shadow_texture = DeviceTexturePtr (device.CreateTexture (texture_desc), false);

      unsigned char texture_data [SHADOW_TEXTURE_SIZE * SHADOW_TEXTURE_SIZE];
      unsigned char *current_texel       = texture_data;
      float         half_texture_size    = SHADOW_TEXTURE_SIZE / 2.f,
                    max_texture_distance = half_texture_size - 0.5f,
                    shadow_arg_min       = pow (page_curl->ShadowLogBase (), MIN_SHADOW_LOG_VALUE),
                    shadow_arg_range     = page_curl->ShadowLogBase () - shadow_arg_min,
                    log_delimiter        = log (page_curl->ShadowLogBase ());

      for (size_t i = 0; i < SHADOW_TEXTURE_SIZE; i++)
        for (size_t j = 0; j < SHADOW_TEXTURE_SIZE; j++)
        {
          float distance = math::length (math::vec2f (i + 0.5f - half_texture_size, j + 0.5 - half_texture_size)) / max_texture_distance;

          *current_texel++ = stl::min (255, stl::max (0, (int)(log (shadow_arg_min + shadow_arg_range * distance) / log_delimiter * 255)));
        }

      shadow_texture->SetData (0, 0, 0, 0, texture_desc.width, texture_desc.height, texture_desc.format, texture_data);

      initialized = true;
    }

    render_state->Capture ();

    const math::vec2ui& grid_size = page_curl->GridSize ();

    if (current_page_grid_size != grid_size)
    {
      curled_page = RenderablePageCurlMeshPtr (new RenderablePageCurlMesh (device, grid_size), false);

      current_page_grid_size = grid_size;
    }

    math::vec2f one_page_size = page_curl->Size ();

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      one_page_size.x /= 2;

    curled_page->SetSize  (one_page_size);
    curled_page->SetColor (GetPageColor ());

    ProgramParameters program_parameters;

    program_parameters.view_matrix       = math::translate (-view_point);
    program_parameters.projection_matrix = projection;
    program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

    constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);

      //установка общих ресурсов

    device.ISSetInputLayout             (input_layout.get ());
    device.SSSetConstantBuffer          (0, constant_buffer.get ());
    device.SSSetProgramParametersLayout (program_parameters_layout.get ());
    device.SSSetSampler                 (0, sampler_state.get ());
    device.SSSetProgram                 (default_program.get ());
    device.OSSetDepthStencilState       (depth_stencil_state_write_enabled.get ());

    switch (page_curl->CurlCorner ())
    {
      case PageCurlCorner_LeftTop:
        DrawLeftTopCornerFlip (device);
        break;
      case PageCurlCorner_LeftBottom:
        DrawLeftBottomCornerFlip (device);
        break;
      case PageCurlCorner_RightTop:
        DrawRightTopCornerFlip (device);
        break;
      case PageCurlCorner_RightBottom:
        DrawRightBottomCornerFlip (device);
        break;
    }

    render_state->Apply ();
  }

  //Рисование теней
  void DrawShadows (low_level::IDevice& device, float curl_radius, bool left_side)
  {
    device.OSSetDepthStencilState (depth_stencil_state_write_disabled.get ());
    device.OSSetBlendState        (mask_blend_state.get ());
    device.SSSetTexture           (0, shadow_texture.get ());

    const math::vec2f& total_size = page_curl->Size ();
    math::vec2f        page_size  = total_size;

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      page_size.x /= 2;

    float x_offset             = left_side || page_curl->Mode () == PageCurlMode_SinglePage ? 0 : page_size.x,
          corner_shadow_offset = page_curl->CornerShadowOffset ();

    const math::vec3f& left_bottom_corner_position  = curled_page->GetCornerPosition (PageCurlCorner_LeftBottom);
    const math::vec3f& left_top_corner_position     = curled_page->GetCornerPosition (PageCurlCorner_LeftTop);
    const math::vec3f& right_bottom_corner_position = curled_page->GetCornerPosition (PageCurlCorner_RightBottom);
    const math::vec3f& right_top_corner_position    = curled_page->GetCornerPosition (PageCurlCorner_RightTop);
    const math::vec3f& top_corner_position          = left_side ? left_top_corner_position : right_top_corner_position;
    const math::vec3f& bottom_corner_position       = left_side ? left_bottom_corner_position : right_bottom_corner_position;

    unsigned char light = 255;//(unsigned char)((1 - page_curl->ShadowDensity () * curl_radius / page_curl->CurlRadius ()) * 255);

      //отрисовка тени под страницей
    if (curled_page->HasBottomSideBendPosition () || curled_page->HasTopSideBendPosition ())
    {
      RenderableVertex vertices [SHADOW_VERTICES_COUNT];

      for (size_t i = 0; i < SHADOW_VERTICES_COUNT; i++)
      {
        vertices [i].color.x = light;
        vertices [i].color.y = light;
        vertices [i].color.z = light;
      }

      size_t      triangles_count = 0;
      math::vec2f base_vertices [4];
      math::vec2f mid_points [2];
      bool        has_side_bend_position    = left_side ? curled_page->HasLeftSideBendPosition () : curled_page->HasRightSideBendPosition ();
      math::vec3f bottom_side_bend_position = curled_page->GetBottomSideBendPosition ();
      math::vec3f top_side_bend_position    = curled_page->GetTopSideBendPosition ();
      math::vec3f side_bend_position        = left_side ? curled_page->GetLeftSideBendPosition () : curled_page->GetRightSideBendPosition ();

      if (has_side_bend_position)  //загнут один угол
      {
        base_vertices [0] = side_bend_position;

        if (fabs (top_corner_position.z) > curl_radius)  //загнут верхний угол
        {
          base_vertices [1] = top_corner_position;
          base_vertices [2] = top_side_bend_position;
        }
        else //загнут нижний угол
        {
          base_vertices [1] = bottom_corner_position;
          base_vertices [2] = bottom_side_bend_position;
        }

        math::vec2f side_vec = base_vertices [2] - base_vertices [0];
        float       angle    = atan2 (-side_vec.y, side_vec.x);

        base_vertices [0] += side_vec * 0.2 * sin (angle);
        base_vertices [2] -= side_vec * 0.2 * cos (angle);

        triangles_count += 13;
      }
      else //загнуты два угла
      {
        base_vertices [0] = top_side_bend_position;
        base_vertices [1] = top_corner_position;
        base_vertices [2] = bottom_corner_position;
        base_vertices [3] = bottom_side_bend_position;

        triangles_count += 18;
      }

      for (size_t i = 0; i < 4; i++)
        base_vertices [i].x += x_offset;

      math::vec2f first_side_vec         = base_vertices [1] - base_vertices [0],
                  first_side_dir         = math::normalize (first_side_vec),
                  second_side_vec        = base_vertices [2] - base_vertices [1],
                  second_side_dir        = math::normalize (second_side_vec);
      float       first_side_length      = math::length (first_side_vec),
                  first_mid_point_offset = stl::min (corner_shadow_offset, first_side_length / 2.f);

      mid_points [0] = base_vertices [0] + first_side_dir * first_mid_point_offset;

      vertices [0].position = math::vec3f (base_vertices [0].x, base_vertices [0].y, curl_radius);
      vertices [0].texcoord = math::vec2f (0, 0.5);
      vertices [1].position = math::vec3f (mid_points [0].x, mid_points [0].y, curl_radius);
      vertices [1].texcoord = math::vec2f (0.5, 0.5);
      vertices [2].position = math::vec3f (mid_points [0].x - second_side_dir.x * corner_shadow_offset, mid_points [0].y - second_side_dir.y * corner_shadow_offset, curl_radius);
      vertices [2].texcoord = math::vec2f (0.5, 1);
      vertices [3].position = math::vec3f (base_vertices [1].x, base_vertices [1].y, curl_radius);
      vertices [3].texcoord = math::vec2f (0.5, 0.5);
      vertices [4].position = math::vec3f (base_vertices [1].x - second_side_dir.x * corner_shadow_offset, base_vertices [1].y - second_side_dir.y * corner_shadow_offset, curl_radius);
      vertices [4].texcoord = math::vec2f (0.5, 1);
      vertices [5].position = math::vec3f (base_vertices [1].x + (-second_side_dir.x + first_side_dir.x) * corner_shadow_offset, base_vertices [1].y + (-second_side_dir.y + first_side_dir.y) * corner_shadow_offset, curl_radius);
      vertices [5].texcoord = math::vec2f (1, 1);
      vertices [6].position = math::vec3f (base_vertices [1].x + first_side_dir.x * corner_shadow_offset, base_vertices [1].y + first_side_dir.y * corner_shadow_offset, curl_radius);
      vertices [6].texcoord = math::vec2f (1, 0.5);

      if (has_side_bend_position)
      {
        float second_side_length      = math::length (second_side_vec),
              second_mid_point_offset = stl::min (corner_shadow_offset, second_side_length / 2);

        mid_points [1] = base_vertices [2] - second_side_dir * second_mid_point_offset;

        vertices [7].position = math::vec3f (mid_points [1].x, mid_points [1].y, curl_radius);
        vertices [7].texcoord = math::vec2f (0.5, 0.5);
        vertices [8].position = math::vec3f (mid_points [1].x + first_side_dir.x * corner_shadow_offset, mid_points [1].y + first_side_dir.y * corner_shadow_offset, curl_radius);
        vertices [8].texcoord = math::vec2f (1, 0.5);
        vertices [9].position = math::vec3f (base_vertices [2].x, base_vertices [2].y, curl_radius);
        vertices [9].texcoord = math::vec2f (0.5, 0);
        vertices [10].position = math::vec3f (base_vertices [0].x - second_side_dir.x * corner_shadow_offset, base_vertices [0].y - second_side_dir.y * corner_shadow_offset, curl_radius);
        vertices [10].texcoord = math::vec2f (0, 1);
        vertices [11].position = math::vec3f (base_vertices [2].x + first_side_dir.x * corner_shadow_offset, base_vertices [2].y + first_side_dir.y * corner_shadow_offset, curl_radius);
        vertices [11].texcoord = math::vec2f (1, 0);

        device.ISSetIndexBuffer (corner_shadow_index_buffer.get ());
      }
      else
      {
        math::vec2f third_side_vec          = base_vertices [3] - base_vertices [2],
                    third_side_dir          = math::normalize (third_side_vec);
        float       third_side_length       = math::length (third_side_vec),
                    second_mid_point_offset = stl::min (corner_shadow_offset, third_side_length / 2.f);

        mid_points [1] = base_vertices [3] - third_side_dir * second_mid_point_offset;

        vertices [7].position  = math::vec3f (base_vertices [2].x, base_vertices [2].y, curl_radius);
        vertices [7].texcoord  = math::vec2f (0.5, 0.5);
        vertices [8].position  = math::vec3f (base_vertices [2].x - third_side_dir.x * corner_shadow_offset, base_vertices [2].y - third_side_dir.y * corner_shadow_offset, curl_radius);
        vertices [8].texcoord  = math::vec2f (1, 0.5);
        vertices [9].position  = math::vec3f (base_vertices [2].x + second_side_dir.x * corner_shadow_offset, base_vertices [2].y + second_side_dir.y * corner_shadow_offset, curl_radius);
        vertices [9].texcoord  = math::vec2f (0.5, 0);
        vertices [10].position = math::vec3f (base_vertices [2].x + (-third_side_dir.x + second_side_dir.x) * corner_shadow_offset, base_vertices [2].y + (-third_side_dir.y + second_side_dir.y) * corner_shadow_offset, curl_radius);
        vertices [10].texcoord = math::vec2f (1, 0);
        vertices [11].position = math::vec3f (mid_points [1].x, mid_points [1].y, curl_radius);
        vertices [11].texcoord = math::vec2f (0.5, 0.5);
        vertices [12].position = math::vec3f (mid_points [1].x + second_side_dir.x * corner_shadow_offset, mid_points [1].y + second_side_dir.y * corner_shadow_offset, curl_radius);
        vertices [12].texcoord = math::vec2f (0.5, 0);
        vertices [13].position = math::vec3f (base_vertices [3].x, base_vertices [3].y, curl_radius);
        vertices [13].texcoord = math::vec2f (0, 0.5);
        vertices [14].position = math::vec3f (base_vertices [0].x - second_side_dir.x * corner_shadow_offset, base_vertices [0].y - second_side_dir.y * corner_shadow_offset, curl_radius);
        vertices [14].texcoord = math::vec2f (0, 1);
        vertices [15].position = math::vec3f (base_vertices [3].x + second_side_dir.x * corner_shadow_offset, base_vertices [3].y + second_side_dir.y * corner_shadow_offset, curl_radius);
        vertices [15].texcoord = math::vec2f (0, 0);

        device.ISSetIndexBuffer (page_shadow_index_buffer.get ());
      }

      shadow_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      device.ISSetVertexBuffer (0, shadow_vertex_buffer.get ());

      device.DrawIndexed (low_level::PrimitiveType_TriangleList, 0, triangles_count * 3, 0);
    }

      //отрисовка тени за страницей
    if (fabs (left_top_corner_position.z) > EPS || fabs (right_top_corner_position.z) > EPS || fabs (left_bottom_corner_position.z) > EPS || fabs (right_bottom_corner_position.z) > EPS)
    {
      bool               has_side_detach_position        = left_side ? curled_page->HasLeftSideDetachPosition () : curled_page->HasRightSideDetachPosition ();
      bool               has_bottom_side_detach_position = curled_page->HasBottomSideDetachPosition ();
      bool               has_top_side_detach_position    = curled_page->HasTopSideDetachPosition ();
      const math::vec3f& side_detach_position            = left_side ? curled_page->GetLeftSideDetachPosition () : curled_page->GetRightSideDetachPosition ();
      math::vec3f        bottom_detach_position;
      math::vec3f        top_detach_position;

      if (has_bottom_side_detach_position)
      {
        bottom_detach_position = curled_page->GetBottomSideDetachPosition ();
      }
      else
      {
        if (has_side_detach_position)
          bottom_detach_position = side_detach_position;
        else
        {
          if (fabs (left_bottom_corner_position.z) > fabs (right_bottom_corner_position.z))
            bottom_detach_position = right_bottom_corner_position;
          else
            bottom_detach_position = left_bottom_corner_position;
        }
      }

      if (has_top_side_detach_position)
      {
        top_detach_position = curled_page->GetTopSideDetachPosition ();
      }
      else
      {
        if (has_side_detach_position)
          top_detach_position = side_detach_position;
        else
        {
          if (fabs (left_top_corner_position.z) > fabs (right_top_corner_position.z))
            top_detach_position = right_top_corner_position;
          else
            top_detach_position = left_top_corner_position;
        }
      }

      if (!has_bottom_side_detach_position && !has_top_side_detach_position)
      {
        if (fabs (left_top_corner_position.z) > 0 && fabs (right_top_corner_position.z) > 0)
        {
          if (fabs (left_top_corner_position.z) > fabs (right_top_corner_position.z))
            top_detach_position = right_top_corner_position;
          else
            top_detach_position = left_top_corner_position;
        }
        if (fabs (left_bottom_corner_position.z) > 0 && fabs (right_bottom_corner_position.z) > 0)
        {
          if (fabs (left_bottom_corner_position.z) > fabs (right_bottom_corner_position.z))
            bottom_detach_position = right_bottom_corner_position;
          else
            bottom_detach_position = left_bottom_corner_position;
        }
      }

      math::vec2f shadow_offset = normalize (math::vec2f (-top_detach_position.y + bottom_detach_position.y, -bottom_detach_position.x + top_detach_position.x));

      if (!left_side)
        shadow_offset *= -1;

      float shadow_width = page_size.x * page_curl->ShadowWidth () * curl_radius / page_curl->CurlRadius ();

      RenderableVertex vertices [4];

      for (size_t i = 0; i < 4; i++)
      {
        vertices [i].color.x = light;
        vertices [i].color.y = light;
        vertices [i].color.z = light;
      }

      vertices [0].position = math::vec3f (top_detach_position.x + x_offset,                                     top_detach_position.y,                                          BACK_SHADOW_OFFSET);
      vertices [1].position = math::vec3f (top_detach_position.x + shadow_offset.x * shadow_width + x_offset,    top_detach_position.y + shadow_offset.y * shadow_width,         BACK_SHADOW_OFFSET);
      vertices [2].position = math::vec3f (bottom_detach_position.x + x_offset,                                  bottom_detach_position.y,                                       BACK_SHADOW_OFFSET);
      vertices [3].position = math::vec3f (bottom_detach_position.x + shadow_offset.x * shadow_width + x_offset, bottom_detach_position.y + shadow_offset.y * shadow_width,      BACK_SHADOW_OFFSET);
      vertices [0].texcoord = math::vec2f (0.5, 0.5);
      vertices [1].texcoord = math::vec2f (0, 0.5);
      vertices [2].texcoord = math::vec2f (0.5, 0.5);
      vertices [3].texcoord = math::vec2f (0, 0.5);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      device.ISSetVertexBuffer (0, quad_vertex_buffer.get ());

      math::vec3f left_bottom_corner_screen = (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0) * page_curl->WorldTM () - view_point) * projection,
                  right_top_corner_screen   = (math::vec3f (page_curl->Size ().x / 2, page_curl->Size ().y / 2, 0) * page_curl->WorldTM () - view_point) * projection;

      low_level::Rect scissor_rect;

      scissor_rect.x = viewport.x + (left_bottom_corner_screen.x + 1) / 2 * viewport.width;
      scissor_rect.y = viewport.y + (left_bottom_corner_screen.y + 1) / 2 * viewport.height;
      scissor_rect.width  = ceil ((right_top_corner_screen.x - left_bottom_corner_screen.x) / 2 * viewport.width);
      scissor_rect.height = ceil ((right_top_corner_screen.y - left_bottom_corner_screen.y) / 2 * viewport.height);

      device.RSSetState (rasterizer_scissor_enabled_state.get ());
      device.RSSetScissor (scissor_rect);

      device.Draw (low_level::PrimitiveType_TriangleStrip, 0, 4);
    }
  }

  //Рисование специфичное для каждого угла
  void DrawLeftTopCornerFlip (low_level::IDevice& device)
  {
    if (page_curl->Mode () == PageCurlMode_SinglePage)
      throw xtl::format_operation_exception ("render::obsolete::render2d::RenderablePageCurl::DrawLeftBottomCornerFlip", "Can't draw flip for left bottom corner in single page mode");

    math::vec2f page_size       = page_curl->Size ();
    math::vec2f corner_position = page_curl->CornerPosition ();

    page_size.x /= 2;

    float x_flip_angle = -stl::max (PI - 2 * atan2 (page_size.y - corner_position.y, corner_position.x),
                                    atan2 (page_size.y - corner_position.y, page_size.x - corner_position.x));

    math::vec2f flip_vec ((page_size.y - corner_position.y) / tan (fabs (x_flip_angle)), corner_position.y - page_size.y);

    float flip_vec_length = math::length (flip_vec),
          curl_radius     = page_curl->CurlRadius ();

    if (flip_vec_length > page_size.x)
    {
      corner_position.x += (flip_vec_length - page_size.x) * flip_vec.x / flip_vec_length;
      corner_position.y -= (flip_vec_length - page_size.x) * flip_vec.y / flip_vec_length;
    }

    math::vec2f curl_corner_position = corner_position;

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (curl_corner_position.x > 2 * page_size.x - curl_radius * 2)
      curl_radius *= (2 * page_size.x - curl_corner_position.x) / (curl_radius * 2);

    curl_radius = stl::max (EPS, curl_radius);

    float flip_width  = (page_size.y - corner_position.y) / tan (fabs (x_flip_angle)) + corner_position.x,
          flip_height = flip_width * tan (fabs (x_flip_angle / 2.f)),
          curl_angle  = -atan2 (flip_width, flip_height);

    if (curl_corner_position.y > 0 || curl_angle > -EPS)
      curl_angle += PI;

    float curl_x = (page_size.x - flip_width) * cos (curl_angle);

    curled_page->Curl (curl_corner_position, page_curl->CurlCorner (), curl_x, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    device.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    float min_s, max_s;

    GetTexCoords (false, min_s, max_s);

    curled_page->SetTexCoords (max_s, 0, min_s, 1);

    curled_page->CalculateShadow (true);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    curled_page->CalculateShadow (false);

    GetTexCoords (true, min_s, max_s);

    curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_no_cull_state.get ());

    DrawStaticPages (device);

    DrawShadows (device, curl_radius, true);
  }

  void DrawLeftBottomCornerFlip (low_level::IDevice& device)
  {
    if (page_curl->Mode () == PageCurlMode_SinglePage)
      throw xtl::format_operation_exception ("render::obsolete::render2d::RenderablePageCurl::DrawLeftBottomCornerFlip", "Can't draw flip for left bottom corner in single page mode");

    math::vec2f page_size       = page_curl->Size ();
    math::vec2f corner_position = page_curl->CornerPosition ();

    page_size.x /= 2;

    float x_flip_angle = stl::max (PI - 2 * atan2 (corner_position.y, corner_position.x),
                                   atan2 (corner_position.y, page_size.x - corner_position.x));

    math::vec2f flip_vec (corner_position.y / tan (x_flip_angle), corner_position.y);

    float flip_vec_length = math::length (flip_vec),
          curl_radius     = page_curl->CurlRadius ();

    if (flip_vec_length > page_size.x)
    {
      corner_position.x += (flip_vec_length - page_size.x) * flip_vec.x / flip_vec_length;
      corner_position.y -= (flip_vec_length - page_size.x) * flip_vec.y / flip_vec_length;
    }

    math::vec2f curl_corner_position = corner_position;

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (curl_corner_position.x > 2 * page_size.x - curl_radius * 2)
      curl_radius *= (2 * page_size.x - curl_corner_position.x) / (curl_radius * 2);

    curl_radius = stl::max (EPS, curl_radius);

    float flip_width  = corner_position.y / tan (x_flip_angle) + corner_position.x,
          flip_height = flip_width * tan (x_flip_angle / 2.f),
          curl_angle  = atan2 (flip_width, flip_height);

    if (curl_corner_position.y < page_size.y || curl_angle < EPS)
      curl_angle += PI;

    float curl_x = (page_size.x - flip_width) * cos (curl_angle);

    curled_page->Curl (curl_corner_position, page_curl->CurlCorner (), curl_x, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    device.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    float min_s, max_s;

    GetTexCoords (false, min_s, max_s);

    curled_page->SetTexCoords (max_s, 0, min_s, 1);

    curled_page->CalculateShadow (true);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    curled_page->CalculateShadow (false);

    GetTexCoords (true, min_s, max_s);

    curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_no_cull_state.get ());

    DrawStaticPages (device);

    DrawShadows (device, curl_radius, true);
  }

  void DrawRightTopCornerFlip (low_level::IDevice& device)
  {
    const math::vec2f& total_size      = page_curl->Size ();
    math::vec2f        page_size       = total_size;
    math::vec2f        corner_position = page_curl->CornerPosition ();

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      page_size.x /= 2;

    float left_page_width = page_curl->Mode () == PageCurlMode_SinglePage ? 0 : page_size.x;

    float x_flip_angle = -stl::max (PI - 2 * atan2 (page_size.y - corner_position.y, total_size.x - corner_position.x),
                                    atan2 (page_size.y - corner_position.y, corner_position.x - left_page_width));

    math::vec2f flip_vec ((page_size.y - corner_position.y) / tan (fabs (x_flip_angle)), corner_position.y - page_size.y);

    float flip_vec_length = math::length (flip_vec),
          curl_radius     = page_curl->CurlRadius ();

    if (flip_vec_length > page_size.x)
    {
      corner_position.x -= (flip_vec_length - page_size.x) * flip_vec.x / flip_vec_length;
      corner_position.y -= (flip_vec_length - page_size.x) * flip_vec.y / flip_vec_length;
    }

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      corner_position.x -= page_size.x;

    math::vec2f curl_corner_position = corner_position;

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (curl_corner_position.x < -page_size.x + curl_radius * 2)
      curl_radius *= (-page_size.x - curl_corner_position.x) / -(curl_radius * 2);

    curl_radius = stl::max (EPS, curl_radius);

    float distance_to_right_edge = total_size.x - corner_position.x,
          flip_width             = (page_size.y - corner_position.y) / tan (fabs (x_flip_angle)) + distance_to_right_edge,
          flip_height            = flip_width * tan (fabs (x_flip_angle / 2.f)),
          curl_angle             = atan2 (flip_width, flip_height);

    if (curl_corner_position.y <= 0 && curl_angle > EPS)
      curl_angle += PI;

    float curl_x = (page_size.x - flip_width) * cos (curl_angle);

    curled_page->Curl (curl_corner_position, page_curl->CurlCorner (), curl_x, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (page_size.x, 0, 0)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    device.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    float min_s, max_s;

    GetTexCoords (true, min_s, max_s);

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      curled_page->SetTexCoords (max_s, 0, min_s, 1);
    else
      curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->CalculateShadow (true);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    curled_page->CalculateShadow (false);

    GetTexCoords (false, min_s, max_s);

    curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_no_cull_state.get ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    DrawStaticPages (device);

    DrawShadows (device, curl_radius, false);
  }

  void DrawRightBottomCornerFlip (low_level::IDevice& device)
  {
    const math::vec2f& total_size      = page_curl->Size ();
    math::vec2f        page_size       = total_size;
    math::vec2f        corner_position = page_curl->CornerPosition ();

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      page_size.x /= 2;

    float left_page_width = page_curl->Mode () == PageCurlMode_SinglePage ? 0 : page_size.x;

    float x_flip_angle = stl::max (PI - 2 * atan2 (corner_position.y, total_size.x - corner_position.x),
                                    atan2 (corner_position.y, corner_position.x - left_page_width));

    math::vec2f flip_vec (corner_position.y / tan (x_flip_angle), corner_position.y);

    float flip_vec_length = math::length (flip_vec),
          curl_radius     = page_curl->CurlRadius ();

    if (flip_vec_length > page_size.x)
    {
      corner_position.x -= (flip_vec_length - page_size.x) * flip_vec.x / flip_vec_length;
      corner_position.y -= (flip_vec_length - page_size.x) * flip_vec.y / flip_vec_length;
    }

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      corner_position.x -= page_size.x;

    math::vec2f curl_corner_position = corner_position;

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (curl_corner_position.x < -page_size.x + curl_radius * 2)
      curl_radius *= (-page_size.x - curl_corner_position.x) / -(curl_radius * 2);

    curl_radius = stl::max (EPS, curl_radius);

    float distance_to_right_edge = total_size.x - corner_position.x,
          flip_width             = corner_position.y / tan (x_flip_angle) + distance_to_right_edge,
          flip_height            = flip_width * tan (x_flip_angle / 2.f),
          curl_angle             = -atan2 (flip_width, flip_height);

    if (curl_corner_position.y >= page_size.y && curl_angle < -EPS)
      curl_angle -= PI;

    float curl_x = (page_size.x - flip_width) * cos (curl_angle);

    curled_page->Curl (curl_corner_position, page_curl->CurlCorner (), curl_x, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (page_size.x, 0, 0)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    device.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    float min_s, max_s;

    GetTexCoords (true, min_s, max_s);

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      curled_page->SetTexCoords (max_s, 0, min_s, 1);
    else
      curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->CalculateShadow (true);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    curled_page->CalculateShadow (false);

    GetTexCoords (false, min_s, max_s);

    curled_page->SetTexCoords (min_s, 0, max_s, 1);

    curled_page->Draw (device);

    device.RSSetState (rasterizer_no_cull_state.get ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    DrawStaticPages (device);

    DrawShadows (device, curl_radius, false);
  }

  //Рисование лежащих страниц
  void DrawStaticPages (low_level::IDevice& device)
  {
    const math::vec2f& size = page_curl->Size ();

    int   left_page_type   = -1,
          right_page_type  = -1;
    float left_page_width  = size.x * 0.5f,
          right_page_width = left_page_width;

    device.ISSetVertexBuffer (0, quad_vertex_buffer.get ());

    switch (page_curl->Mode ())
    {
      case PageCurlMode_SinglePage:
        right_page_type  = PageCurlPageType_Front;
        left_page_width  = 0.f;
        right_page_width = size.x;
        break;
      case PageCurlMode_DoublePageSingleMaterial:
        left_page_type  = PageCurlPageType_Back;
        right_page_type = PageCurlPageType_Front;
        break;
      case PageCurlMode_DoublePageDoubleMaterial:
        left_page_type  = PageCurlPageType_BackLeft;
        right_page_type = PageCurlPageType_FrontRight;
        break;
    }

    math::vec4ub page_color (GetPageColor ());

    if (left_page_type >= 0)
    {
      RenderableVertex vertices [4];

      for (size_t i = 0; i < 4; i++)
        vertices [i].color = page_color;

      float min_s, max_s;

      GetTexCoords (true, min_s, max_s);

      vertices [0].position = math::vec3f (0, 0, STATIC_PAGES_Z_OFFSET);
      vertices [1].position = math::vec3f (left_page_width, 0, STATIC_PAGES_Z_OFFSET);
      vertices [2].position = math::vec3f (0, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [3].position = math::vec3f (left_page_width, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [0].texcoord = math::vec2f (min_s, 0);
      vertices [1].texcoord = math::vec2f (max_s, 0);
      vertices [2].texcoord = math::vec2f (min_s, 1.f);
      vertices [3].texcoord = math::vec2f (max_s, 1.f);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      BindMaterial (device, page_materials [left_page_type].get (), page_textures [left_page_type].get ());

      device.Draw (low_level::PrimitiveType_TriangleStrip, 0, 4);
    }

    if (right_page_type >= 0)
    {
      RenderableVertex vertices [4];

      for (size_t i = 0; i < 4; i++)
        vertices [i].color = page_color;

      float min_s, max_s;

      GetTexCoords (false, min_s, max_s);

      vertices [0].position = math::vec3f (left_page_width, 0, STATIC_PAGES_Z_OFFSET);
      vertices [1].position = math::vec3f (left_page_width + right_page_width, 0, STATIC_PAGES_Z_OFFSET);
      vertices [2].position = math::vec3f (left_page_width, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [3].position = math::vec3f (left_page_width + right_page_width, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [0].texcoord = math::vec2f (min_s, 0);
      vertices [1].texcoord = math::vec2f (max_s, 0);
      vertices [2].texcoord = math::vec2f (min_s, 1.f);
      vertices [3].texcoord = math::vec2f (max_s, 1.f);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      BindMaterial (device, page_materials [right_page_type].get (), page_textures [right_page_type].get ());

      device.Draw (low_level::PrimitiveType_TriangleStrip, 0, 4);
    }
  }

  void BindMaterial (low_level::IDevice& device, SpriteMaterial* material, ILowLevelTexture* texture)
  {
    static const char* METHOD_NAME = "render::obsolete::render2d::RenderablePageCurl::BindMaterial";

    if (!material)
      throw xtl::make_null_argument_exception (METHOD_NAME, "material");

    if (!texture)
      throw xtl::make_null_argument_exception (METHOD_NAME, "texture");

      //получение материала из кэша

    BlendStatePtr material_blend_state;

    switch (material->BlendMode ())
    {
      case media::rfx::obsolete::SpriteBlendMode_None:
        material_blend_state = none_blend_state;
        break;
      case media::rfx::obsolete::SpriteBlendMode_Translucent:
        material_blend_state = translucent_blend_state;
        break;
      case media::rfx::obsolete::SpriteBlendMode_Mask:
        material_blend_state = mask_blend_state;
        break;
      case media::rfx::obsolete::SpriteBlendMode_Additive:
        material_blend_state = additive_blend_state;
        break;
      default:
        throw xtl::format_operation_exception (METHOD_NAME, "Unsupported material '%s' blend mode, only 'none', 'translucent', 'mask' and 'additive' page material blend mode supported", material->Name ());
    }

    device.OSSetBlendState (material_blend_state.get ());
    device.SSSetTexture    (0, texture->GetTexture ());
  }
};

/*
    Конструктор
*/

RenderablePageCurl::RenderablePageCurl (scene_graph::PageCurl* page_curl, Render& render)
  : Renderable (page_curl),
    impl (new Impl (page_curl, render, this))
{
}

RenderablePageCurl::~RenderablePageCurl ()
{
}

/*
   Обновление
*/

void RenderablePageCurl::Update ()
{
  impl->Update ();
}

/*
    Рисование
*/

void RenderablePageCurl::DrawCore (render::obsolete::render2d::IFrame& frame)
{
  frame.GetViewPoint  (impl->view_point);
  frame.GetProjection (impl->projection);
  frame.GetViewport   (impl->viewport);

  impl->low_level_frame->SetRenderTargets (frame.GetRenderTarget (), frame.GetDepthStencilTarget ());
  impl->low_level_frame->SetViewport (impl->viewport);

  impl->render.Renderer ()->AddFrame (impl->low_level_frame.get ());
}
