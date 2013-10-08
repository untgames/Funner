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

const float  EPS                             = 0.001f;
const float  BACK_SHADOW_OFFSET              = 0;
const float  CORNER_SHADOW_GROW_PART         = 0.2f;
const float  CORNER_SHADOW_SHIFT             = 0.2f;
const float  MAX_CORNER_HEIGHT_OFFSET_FACTOR = 0.2f;
const float  PI                              = 3.1415926f;
const size_t SHADOW_TEXTURE_SIZE             = 32;
const float  STATIC_PAGES_Z_OFFSET           = -0.001f;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Параметры вершины необходимые для визуализации
///////////////////////////////////////////////////////////////////////////////////////////////////
struct RenderableVertex
{
  math::vec3f  position; //положение вершины в пространстве
  math::vec2f  texcoord; //текстурные координаты
  math::vec4ub color;    //цвет вершины
};

typedef xtl::uninitialized_storage<RenderableVertex> VerticesBuffer;

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
  math::vec2ui               page_textures_sizes [scene_graph::PageCurlPageType_Num];               //размеры текстур страниц
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
  size_t                     shadow_vertex_buffer_size;          //размер вершинного буфера для отрисовки тени
  VerticesBuffer             shadow_vertices;                    //буфер для рассчета вершин тени
  StateBlockPtr              render_state;                       //сохранение состояния рендера
  math::vec3f                view_point;                         //позиция камеры
  math::mat4f                projection;                         //матрица камеры
  mid_level::Viewport        viewport;                           //область вывода
  RenderablePageCurlMeshPtr  curled_page;                        //сетка изгибаемой страницы
  math::vec2ui               current_page_grid_size;             //текущий размер сетки страницы
  bool                       current_page_is_rigid;              //является ли страница жесткой
  bool                       initialized;                        //инициализированы ли необходимые поля

  ///Конструктор
  Impl (scene_graph::PageCurl* in_page_curl, Render& in_render, Renderable* in_renderable)
    : render (in_render)
    , page_curl (in_page_curl)
    , renderable (in_renderable)
    , shadow_vertex_buffer_size (0)
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

          render::low_level::TextureDesc texture_desc;

          page_textures [i]->GetTexture ()->GetDesc (texture_desc);

          page_textures_sizes [i].x = texture_desc.width;
          page_textures_sizes [i].y = texture_desc.height;
        }
        else
        {
          page_textures [i]       = 0;
          page_materials [i]      = 0;
          page_textures_sizes [i] = 1;
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

    blend_desc.render_target [0].blend_enable                     = blend_enable;
    blend_desc.render_target [0].blend_color_operation            = low_level::BlendOperation_Add;
    blend_desc.render_target [0].blend_alpha_operation            = low_level::BlendOperation_Add;
    blend_desc.render_target [0].blend_color_source_argument      = src_arg;
    blend_desc.render_target [0].blend_color_destination_argument = dst_color_arg;
    blend_desc.render_target [0].blend_alpha_source_argument      = src_arg;
    blend_desc.render_target [0].blend_alpha_destination_argument = dst_alpha_arg;
    blend_desc.render_target [0].color_write_mask                 = low_level::ColorWriteFlag_All;

    return BlendStatePtr (device.CreateBlendState (blend_desc), false);
  }

  //создание объекта расположения входной геометрии
  InputLayoutPtr CreateInputLayout (low_level::IDevice& device)
  {
    low_level::VertexAttribute attributes [3];

    memset (attributes, 0, sizeof (attributes));

    attributes [0].semantic = device.GetVertexAttributeSemanticName (low_level::VertexAttributeSemantic_Position);
    attributes [0].format   = low_level::InputDataFormat_Vector3;
    attributes [0].type     = low_level::InputDataType_Float;
    attributes [0].slot     = 0;
    attributes [0].offset   = offsetof (RenderableVertex, position);
    attributes [0].stride   = sizeof (RenderableVertex);

    attributes [1].semantic = device.GetVertexAttributeSemanticName (low_level::VertexAttributeSemantic_TexCoord0);
    attributes [1].format   = low_level::InputDataFormat_Vector2;
    attributes [1].type     = low_level::InputDataType_Float;
    attributes [1].slot     = 0;
    attributes [1].offset   = offsetof (RenderableVertex, texcoord);
    attributes [1].stride   = sizeof (RenderableVertex);

    attributes [2].semantic = device.GetVertexAttributeSemanticName (low_level::VertexAttributeSemantic_Color);
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
    rasterizer_desc.multisample_enable      = true;
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

  void GetTexRect (SpriteMaterial* material, const math::vec2ui& texture_size, float& min_s, float& max_s, float& min_t, float& max_t)
  {
     if (material->IsTiled ())
     {
       size_t tile_width  = material->TileWidth (),
              tile_height = material->TileHeight ();

       if (!tile_width || !tile_height)
         throw xtl::format_operation_exception ("render::obsolete::render2d::RenderablePageCurl::GetTexRect",
                                                "Bad page material '%s' (tile_width=%u, tile_height=%u)", material->Name (), tile_width, tile_height);

       min_s = material->TileOffsetX () / (float)texture_size.x;
       min_t = (material->TileOffsetY () - tile_height) / (float)texture_size.y;
       max_s = min_s + tile_width / (float)texture_size.x;
       max_t = min_t + tile_height / (float)texture_size.y;
     }
     else
     {
       min_s = 0;
       min_t = 0;
       max_s = 1;
       max_t = 1;
     }
  }

  void GetTexCoords (bool left, SpriteMaterial* material, const math::vec2ui& texture_size, float& min_s, float& max_s, float& min_t, float& max_t)
  {
    GetTexRect (material, texture_size, min_s, max_s, min_t, max_t);

    if (page_curl->Mode () == PageCurlMode_DoublePageSingleMaterial)
    {
      float half_texture_width = (max_s - min_s) * 0.5f;

      if (left)
        max_s -= half_texture_width;
      else
        min_s += half_texture_width;
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
    low_level::IDeviceContext* context = device.GetImmediateContext ();

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
                    shadow_arg_min       = pow (page_curl->ShadowLogBase (), page_curl->ShadowMinLogValue ()),
                    shadow_arg_range     = page_curl->ShadowLogBase () - shadow_arg_min,
                    log_delimiter        = log (page_curl->ShadowLogBase ());

      for (size_t i = 0; i < SHADOW_TEXTURE_SIZE; i++)
        for (size_t j = 0; j < SHADOW_TEXTURE_SIZE; j++)
        {
          float distance = math::length (math::vec2f (i + 0.5f - half_texture_size, j + 0.5f - half_texture_size)) / max_texture_distance;

          *current_texel++ = stl::min (255, stl::max (0, (int)(log (shadow_arg_min + shadow_arg_range * distance) / log_delimiter * 255)));
        }

      shadow_texture->SetData (0, 0, 0, 0, texture_desc.width, texture_desc.height, texture_desc.format, texture_data);

      initialized = true;
    }

    render_state->Capture (context);

    const math::vec2ui& grid_size = page_curl->GridSize ();
    bool                is_rigid  = page_curl->IsRigidPage ();

    if (current_page_grid_size != grid_size || current_page_is_rigid != is_rigid)
    {
      curled_page = RenderablePageCurlMeshPtr (new RenderablePageCurlMesh (device, is_rigid, grid_size), false);

      current_page_grid_size = grid_size;
      current_page_is_rigid  = is_rigid;

      size_t shadow_max_vertices = (grid_size.x + grid_size.y) * 2 * 2 * 3 + 6 * 4; //side points count * sides count * triangles per point * vertices per triangle + vertices for corners

      if (shadow_vertex_buffer_size < shadow_max_vertices)
      {
        low_level::BufferDesc buffer_desc;

        memset (&buffer_desc, 0, sizeof buffer_desc);

        buffer_desc.usage_mode   = low_level::UsageMode_Stream;
        buffer_desc.bind_flags   = low_level::BindFlag_VertexBuffer;
        buffer_desc.access_flags = low_level::AccessFlag_Write;
        buffer_desc.size         = sizeof (RenderableVertex) * shadow_max_vertices;

        shadow_vertex_buffer = BufferPtr (device.CreateBuffer (buffer_desc), false);

        shadow_vertex_buffer_size = shadow_max_vertices;
      }

      if (shadow_vertices.size () < shadow_vertex_buffer_size)
        shadow_vertices.resize (shadow_vertex_buffer_size, false);
    }

    const math::vec2f& page_size     = page_curl->Size ();
          math::vec2f  one_page_size = page_size;

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      one_page_size.x /= 2;

    curled_page->SetSize                       (one_page_size);
    curled_page->SetColor                      (GetPageColor ());
    curled_page->SetRigidPagePerspectiveFactor (page_curl->RigidPagePerspectiveFactor ());

    ProgramParameters program_parameters;

    program_parameters.view_matrix       = math::translate (-view_point);
    program_parameters.projection_matrix = projection;
    program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

    constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);

      //установка общих ресурсов

    context->ISSetInputLayout             (input_layout.get ());
    context->SSSetConstantBuffer          (0, constant_buffer.get ());
    context->SSSetProgramParametersLayout (program_parameters_layout.get ());
    context->SSSetSampler                 (0, sampler_state.get ());
    context->SSSetProgram                 (default_program.get ());
    context->OSSetDepthStencilState       (depth_stencil_state_write_enabled.get ());

    const math::vec2f& curl_point = page_curl->CurlPoint ();

    if (curl_point.x < page_size.x / 2)
    {
      if (curl_point.y < page_size.y / 2)
        DrawLeftBottomCornerFlip (device, *context);
      else
        DrawLeftTopCornerFlip (device, *context);
    }
    else
    {
      if (curl_point.y < page_size.y / 2)
        DrawRightBottomCornerFlip (device, *context);
      else
        DrawRightTopCornerFlip (device, *context);
    }

    render_state->Apply (context);
  }

  //Рисование теней
  void BuildSideShadow (const math::vec3f* side_positions, size_t side_positions_count, float corner_shadow_offset, float curl_radius, RenderableVertex*& current_vertex, size_t& triangles_count)
  {
    math::vec3f prev_offset_point;

    for (size_t i = 0, count = side_positions_count - 1; i < count; i++, side_positions++)
    {
      const math::vec3f* next_side_position = side_positions + 1;
      math::vec2f        side_vec           = *next_side_position - *side_positions;
      math::vec2f        side_normal        = math::normalize (math::vec2f (side_vec.y, -side_vec.x)) * corner_shadow_offset * stl::max (0.f, side_positions->z / curl_radius - 1.f);

      current_vertex [0].position = *side_positions;
      current_vertex [0].texcoord = math::vec2f (0.5, 0.5);
      current_vertex [0].color    = 255;
      current_vertex [1].position = i ? prev_offset_point : *side_positions + side_normal;
      current_vertex [1].texcoord = math::vec2f (0.5, 1);
      current_vertex [1].color    = 255;
      current_vertex [2].position = *next_side_position + side_normal;
      current_vertex [2].texcoord = math::vec2f (0.5, 1);
      current_vertex [2].color    = 255;
      current_vertex [3].position = *side_positions;
      current_vertex [3].texcoord = math::vec2f (0.5, 0.5);
      current_vertex [3].color    = 255;
      current_vertex [4].position = *next_side_position + side_normal;
      current_vertex [4].texcoord = math::vec2f (0.5, 1);
      current_vertex [4].color    = 255;
      current_vertex [5].position = *next_side_position;
      current_vertex [5].texcoord = math::vec2f (0.5, 0.5);
      current_vertex [5].color    = 255;

      prev_offset_point = current_vertex [2].position;

      triangles_count += 2;
      current_vertex  += 6;
    }
  }

  void BuildCornerShadow (const math::vec3f& corner_position, const math::vec2f& side1_vec, const math::vec2f& side2_vec, float corner_shadow_offset, float curl_radius, RenderableVertex*& current_vertex, size_t& triangles_count)
  {
    if (!corner_position.z)
      return;

    math::vec3f side1_offset = math::normalize (math::vec2f (side1_vec.y, -side1_vec.x)) * corner_shadow_offset * stl::max (0.f, corner_position.z / curl_radius - 1.f),
                side2_offset = math::normalize (math::vec2f (side2_vec.y, -side2_vec.x)) * corner_shadow_offset * stl::max (0.f, corner_position.z / curl_radius - 1.f);

    current_vertex [0].position = corner_position;
    current_vertex [0].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [0].color    = 255;
    current_vertex [1].position = corner_position + side2_offset;
    current_vertex [1].texcoord = math::vec2f (0.5, 1);
    current_vertex [1].color    = 255;
    current_vertex [2].position = corner_position + side1_offset + side2_offset;
    current_vertex [2].texcoord = math::vec2f (1, 1);
    current_vertex [2].color    = 255;
    current_vertex [3].position = corner_position;
    current_vertex [3].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [3].color    = 255;
    current_vertex [4].position = corner_position + side1_offset + side2_offset;
    current_vertex [4].texcoord = math::vec2f (1, 1);
    current_vertex [4].color    = 255;
    current_vertex [5].position = corner_position + side1_offset;
    current_vertex [5].texcoord = math::vec2f (1, 0.5);
    current_vertex [5].color    = 255;

    triangles_count += 2;
    current_vertex  += 6;
  }

  void DrawRigidPageShadows (low_level::IDevice& device, low_level::IDeviceContext& context, float x_offset, bool left_side)
  {
    const math::vec2f page_size = page_curl->Size ();

    //отрисовка тени под страницей
    math::vec3f v0 = curled_page->GetCornerPosition (PageCurlCorner_LeftTop);
    math::vec3f v1 = curled_page->GetCornerPosition (PageCurlCorner_RightTop);
    math::vec3f v2 = curled_page->GetCornerPosition (PageCurlCorner_LeftBottom);
    math::vec3f v3 = curled_page->GetCornerPosition (PageCurlCorner_RightBottom);

    v0.x += x_offset;
    v1.x += x_offset;
    v2.x += x_offset;
    v3.x += x_offset;

    const math::vec3f* top_static_vertex;
    const math::vec3f* bottom_static_vertex;
    const math::vec3f* top_dynamic_vertex;
    const math::vec3f* bottom_dynamic_vertex;

    if (left_side)
    {
      top_static_vertex     = &v1;
      bottom_static_vertex  = &v3;
      top_dynamic_vertex    = &v0;
      bottom_dynamic_vertex = &v2;
    }
    else
    {
      top_static_vertex     = &v0;
      bottom_static_vertex  = &v2;
      top_dynamic_vertex    = &v1;
      bottom_dynamic_vertex = &v3;
    }

    float corner_shadow_offset = page_curl->CornerShadowOffset () * (1 - fabs (top_dynamic_vertex->x - page_size.x / 2) / (page_size.x / 2));

    if (corner_shadow_offset < EPS)
      return;

    math::vec3f top_side_normal = math::normalize (*top_static_vertex - *bottom_static_vertex) * corner_shadow_offset;

    RenderableVertex* current_vertex = shadow_vertices.data ();

    current_vertex [0].position = *top_static_vertex;
    current_vertex [0].texcoord = math::vec2f (0.5, 1);
    current_vertex [0].color    = 255;
    current_vertex [1].position = *top_dynamic_vertex + top_side_normal;
    current_vertex [1].texcoord = math::vec2f (0.5, 1);
    current_vertex [1].color    = 255;
    current_vertex [2].position = *top_dynamic_vertex;
    current_vertex [2].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [2].color    = 255;

    current_vertex += 3;

    math::vec3f side_normal = math::normalize (*top_dynamic_vertex - *top_static_vertex) * corner_shadow_offset;

    current_vertex [0].position = *top_dynamic_vertex;
    current_vertex [0].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [0].color    = 255;
    current_vertex [1].position = *top_dynamic_vertex + side_normal;
    current_vertex [1].texcoord = math::vec2f (0.5, 1);
    current_vertex [1].color    = 255;
    current_vertex [2].position = *bottom_dynamic_vertex + side_normal;
    current_vertex [2].texcoord = math::vec2f (0.5, 1);
    current_vertex [2].color    = 255;
    current_vertex [3].position = *top_dynamic_vertex;
    current_vertex [3].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [3].color    = 255;
    current_vertex [4].position = *bottom_dynamic_vertex + side_normal;
    current_vertex [4].texcoord = math::vec2f (0.5, 1);
    current_vertex [4].color    = 255;
    current_vertex [5].position = *bottom_dynamic_vertex;
    current_vertex [5].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [5].color    = 255;

    current_vertex += 6;

    math::vec3f bottom_side_normal = math::normalize (*bottom_static_vertex - *top_static_vertex) * corner_shadow_offset;

    current_vertex [0].position = *bottom_static_vertex;
    current_vertex [0].texcoord = math::vec2f (0.5, 1);
    current_vertex [0].color    = 255;
    current_vertex [1].position = *bottom_dynamic_vertex + bottom_side_normal;
    current_vertex [1].texcoord = math::vec2f (0.5, 1);
    current_vertex [1].color    = 255;
    current_vertex [2].position = *bottom_dynamic_vertex;
    current_vertex [2].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [2].color    = 255;

    current_vertex += 3;

    current_vertex [0].position = *top_dynamic_vertex;
    current_vertex [0].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [0].color    = 255;
    current_vertex [1].position = *top_dynamic_vertex + top_side_normal;
    current_vertex [1].texcoord = math::vec2f (0.5, 1);
    current_vertex [1].color    = 255;
    current_vertex [2].position = *top_dynamic_vertex + top_side_normal + side_normal;
    current_vertex [2].texcoord = math::vec2f (1, 1);
    current_vertex [2].color    = 255;
    current_vertex [3].position = *top_dynamic_vertex;
    current_vertex [3].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [3].color    = 255;
    current_vertex [4].position = *top_dynamic_vertex + top_side_normal + side_normal;
    current_vertex [4].texcoord = math::vec2f (1, 1);
    current_vertex [4].color    = 255;
    current_vertex [5].position = *top_dynamic_vertex + side_normal;
    current_vertex [5].texcoord = math::vec2f (0.5, 1);
    current_vertex [5].color    = 255;

    current_vertex += 6;

    current_vertex [0].position = *bottom_dynamic_vertex;
    current_vertex [0].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [0].color    = 255;
    current_vertex [1].position = *bottom_dynamic_vertex + side_normal;
    current_vertex [1].texcoord = math::vec2f (0.5, 1);
    current_vertex [1].color    = 255;
    current_vertex [2].position = *bottom_dynamic_vertex + bottom_side_normal + side_normal;
    current_vertex [2].texcoord = math::vec2f (1, 1);
    current_vertex [2].color    = 255;
    current_vertex [3].position = *bottom_dynamic_vertex;
    current_vertex [3].texcoord = math::vec2f (0.5, 0.5);
    current_vertex [3].color    = 255;
    current_vertex [4].position = *bottom_dynamic_vertex + bottom_side_normal + side_normal;
    current_vertex [4].texcoord = math::vec2f (1, 1);
    current_vertex [4].color    = 255;
    current_vertex [5].position = *bottom_dynamic_vertex + bottom_side_normal;
    current_vertex [5].texcoord = math::vec2f (0.5, 1);
    current_vertex [5].color    = 255;

    size_t triangles_count = 8;
    size_t vertices_count = triangles_count * 3;

    shadow_vertex_buffer->SetData (0, sizeof (RenderableVertex) * vertices_count, shadow_vertices.data ());

    context.ISSetVertexBuffer (0, shadow_vertex_buffer.get ());

    context.Draw (low_level::PrimitiveType_TriangleList, 0, vertices_count);
  }

  void DrawShadows (low_level::IDevice& device, low_level::IDeviceContext& context, PageCurlCorner corner, float curl_radius, bool left_side)
  {
    if (curl_radius <= EPS)
      return;

    context.OSSetDepthStencilState (depth_stencil_state_write_disabled.get ());
    context.OSSetBlendState        (mask_blend_state.get ());
    context.SSSetTexture           (0, shadow_texture.get ());

    const math::vec2f& total_size = page_curl->Size ();
    math::vec2f        page_size  = total_size;

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      page_size.x /= 2;

    float x_offset             = left_side || page_curl->Mode () == PageCurlMode_SinglePage ? 0 : page_size.x,
          corner_shadow_offset = page_size.x * page_curl->CornerShadowOffset () * curl_radius / page_curl->CurlRadius ();

    const math::vec3f& left_bottom_corner_position  = curled_page->GetCornerPosition (PageCurlCorner_LeftBottom);
    const math::vec3f& left_top_corner_position     = curled_page->GetCornerPosition (PageCurlCorner_LeftTop);
    const math::vec3f& right_bottom_corner_position = curled_page->GetCornerPosition (PageCurlCorner_RightBottom);
    const math::vec3f& right_top_corner_position    = curled_page->GetCornerPosition (PageCurlCorner_RightTop);

    float shadow_width  = page_size.x * page_curl->ShadowWidth () * curl_radius / page_curl->CurlRadius (),
          corner_offset = 0.f;

    switch (corner)
    {
      case PageCurlCorner_LeftTop:
        corner_offset = math::length (math::vec2f (left_top_corner_position.x, page_size.y - left_top_corner_position.y));
        break;
      case PageCurlCorner_LeftBottom:
        corner_offset = math::length (math::vec2f (left_bottom_corner_position.x, left_bottom_corner_position.y));
        break;
      case PageCurlCorner_RightTop:
        corner_offset = math::length (math::vec2f (page_size.x - right_top_corner_position.x,  - right_top_corner_position.y));
        break;
      case PageCurlCorner_RightBottom:
        corner_offset = math::length (math::vec2f (page_size.x - right_bottom_corner_position.x, right_bottom_corner_position.y));
        break;
    }

    corner_shadow_offset *= stl::min (corner_offset / page_size.x, CORNER_SHADOW_GROW_PART) / CORNER_SHADOW_GROW_PART;

    if (current_page_is_rigid)
    {
      DrawRigidPageShadows (device, context, x_offset, left_side);
      return;
    }

      //отрисовка тени под страницей
    {
      size_t triangles_count = 0;

      RenderableVertex*   current_vertex   = shadow_vertices.data ();
      const math::vec3f*  positions        = curled_page->GridVertices ();
      size_t              positions_stride = curled_page->GridVerticesStride ();
      const math::vec2ui& grid_size        = curled_page->GridSize ();

      xtl::uninitialized_storage<math::vec3f> side_positions (stl::max (grid_size.x, grid_size.y));

      math::vec3f* current_side_position = side_positions.data ();

      for (size_t i = 0; i < grid_size.x; i++, current_side_position++)
      {
        *current_side_position = *(const math::vec3f*)((const unsigned char*)positions + i * positions_stride);
        current_side_position->x +=  x_offset;
      }

      BuildSideShadow (side_positions.data (), grid_size.x, corner_shadow_offset, curl_radius, current_vertex, triangles_count);

      current_side_position = side_positions.data ();

      for (size_t i = 0; i < grid_size.y; i++, current_side_position++)
      {
        *current_side_position = *(const math::vec3f*)((const unsigned char*)positions + (i * grid_size.x + grid_size.x - 1) * positions_stride);
        current_side_position->x +=  x_offset;
      }

      BuildSideShadow (side_positions.data (), grid_size.y, corner_shadow_offset, curl_radius, current_vertex, triangles_count);

      current_side_position = side_positions.data ();

      for (int i = grid_size.x - 1; i >= 0; i--, current_side_position++)
      {
        *current_side_position = *(const math::vec3f*)((const unsigned char*)positions + (grid_size.x * (grid_size.y - 1) + i) * positions_stride);
        current_side_position->x +=  x_offset;
      }

      BuildSideShadow (side_positions.data (), grid_size.x, corner_shadow_offset, curl_radius, current_vertex, triangles_count);

      current_side_position = side_positions.data ();

      for (int i = grid_size.y - 1; i >= 0; i--, current_side_position++)
      {
        *current_side_position = *(const math::vec3f*)((const unsigned char*)positions + (i * grid_size.x) * positions_stride);
        current_side_position->x +=  x_offset;
      }

      BuildSideShadow (side_positions.data (), grid_size.y, corner_shadow_offset, curl_radius, current_vertex, triangles_count);

      BuildCornerShadow (left_top_corner_position + math::vec3f (x_offset, 0, 0), left_top_corner_position - *(const math::vec3f*)((const unsigned char*)positions + grid_size.x * positions_stride),
                         *(const math::vec3f*)((const unsigned char*)positions + positions_stride) - left_top_corner_position, corner_shadow_offset, curl_radius, current_vertex, triangles_count);
      BuildCornerShadow (left_bottom_corner_position + math::vec3f (x_offset, 0, 0), left_bottom_corner_position - *(const math::vec3f*)((const unsigned char*)positions + (grid_size.x * (grid_size.y - 1) + 1) * positions_stride),
                         *(const math::vec3f*)((const unsigned char*)positions + (grid_size.x * (grid_size.y - 2)) * positions_stride) - left_bottom_corner_position, corner_shadow_offset, curl_radius, current_vertex, triangles_count);
      BuildCornerShadow (right_top_corner_position + math::vec3f (x_offset, 0, 0), right_top_corner_position - *(const math::vec3f*)((const unsigned char*)positions + (grid_size.x - 2) * positions_stride),
                         *(const math::vec3f*)((const unsigned char*)positions + (grid_size.x + grid_size.x - 1) * positions_stride) - right_top_corner_position, corner_shadow_offset, curl_radius, current_vertex, triangles_count);
      BuildCornerShadow (right_bottom_corner_position + math::vec3f (x_offset, 0, 0), right_bottom_corner_position - *(const math::vec3f*)((const unsigned char*)positions + (grid_size.x * (grid_size.y - 1) - 1) * positions_stride),
                         *(const math::vec3f*)((const unsigned char*)positions + (grid_size.x * grid_size.y - 2) * positions_stride) - right_bottom_corner_position, corner_shadow_offset, curl_radius, current_vertex, triangles_count);

      if (triangles_count)
      {
        size_t vertices_count = triangles_count * 3;

        shadow_vertex_buffer->SetData (0, sizeof (RenderableVertex) * vertices_count, shadow_vertices.data ());

        context.ISSetVertexBuffer (0, shadow_vertex_buffer.get ());

        context.Draw (low_level::PrimitiveType_TriangleList, 0, vertices_count);
      }
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

      math::vec2f shadow_offset = normalize (math::vec2f (-top_detach_position.y + bottom_detach_position.y, -bottom_detach_position.x + top_detach_position.x)),
                  side_vec      = top_detach_position - bottom_detach_position;

      if (!left_side)
        shadow_offset *= -1;

      RenderableVertex vertices [4];

      for (size_t i = 0; i < 4; i++)
      {
        vertices [i].color.x = 255;
        vertices [i].color.y = 255;
        vertices [i].color.z = 255;
      }

      top_detach_position    += side_vec;
      bottom_detach_position -= side_vec;

      vertices [0].position = math::vec3f (top_detach_position.x + x_offset,                                     top_detach_position.y,                                          BACK_SHADOW_OFFSET);
      vertices [1].position = math::vec3f (top_detach_position.x + shadow_offset.x * shadow_width + x_offset,    top_detach_position.y + shadow_offset.y * shadow_width,         BACK_SHADOW_OFFSET);
      vertices [2].position = math::vec3f (bottom_detach_position.x + x_offset,                                  bottom_detach_position.y,                                       BACK_SHADOW_OFFSET);
      vertices [3].position = math::vec3f (bottom_detach_position.x + shadow_offset.x * shadow_width + x_offset, bottom_detach_position.y + shadow_offset.y * shadow_width,      BACK_SHADOW_OFFSET);
      vertices [0].texcoord = math::vec2f (0.5, 0.5);
      vertices [1].texcoord = math::vec2f (0, 0.5);
      vertices [2].texcoord = math::vec2f (0.5, 0.5);
      vertices [3].texcoord = math::vec2f (0, 0.5);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      context.ISSetVertexBuffer (0, quad_vertex_buffer.get ());

      math::vec3f left_bottom_corner_screen = (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0) * page_curl->WorldTM () - view_point) * projection,
                  right_top_corner_screen   = (math::vec3f (page_curl->Size ().x / 2, page_curl->Size ().y / 2, 0) * page_curl->WorldTM () - view_point) * projection;

      const low_level::Rect& device_viewport = context.RSGetViewport (0);
      low_level::Rect scissor_rect;

      scissor_rect.x      = device_viewport.x + (int)ceil((left_bottom_corner_screen.x + 1.f) / 2.f * viewport.width);
      scissor_rect.y      = device_viewport.y + (int)ceil((left_bottom_corner_screen.y + 1.f) / 2.f * viewport.height);
      scissor_rect.width  = (size_t)ceil ((right_top_corner_screen.x - left_bottom_corner_screen.x) / 2 * viewport.width);
      scissor_rect.height = (size_t)ceil ((right_top_corner_screen.y - left_bottom_corner_screen.y) / 2 * viewport.height);

      context.RSSetState (rasterizer_scissor_enabled_state.get ());
      context.RSSetScissor (0, scissor_rect);

      context.Draw (low_level::PrimitiveType_TriangleStrip, 0, 4);
    }
  }

  //Рисование специфичное для каждого угла
  void DrawLeftTopCornerFlip (low_level::IDevice& device, low_level::IDeviceContext& context)
  {
    if (page_curl->Mode () == PageCurlMode_SinglePage)
      throw xtl::format_operation_exception ("render::obsolete::render2d::RenderablePageCurl::DrawLeftTopCornerFlip", "Can't draw flip for left corner in single page mode");

    const math::vec2f& curl_point_position = page_curl->CurlPointPosition ();
    const math::vec2f& curl_point          = page_curl->CurlPoint ();
          math::vec2f  page_size           = page_curl->Size ();

    math::vec2f corner_position (curl_point_position.x + curl_point.x, curl_point_position.y - curl_point.y + page_size.y);

    if (fabs (corner_position.y) < EPS) //workaround for artifacts with zero curl height
      corner_position.y = EPS;

    float max_corner_height_offset = corner_position.x * MAX_CORNER_HEIGHT_OFFSET_FACTOR;

    if (corner_position.y > page_size.y && corner_position.y - page_size.y > max_corner_height_offset)
      corner_position.y = page_size.y + max_corner_height_offset;

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

    float distance_to_opposite_side = math::length (math::vec2f (2 * page_size.x - stl::min (2 * page_size.x, curl_corner_position.x), curl_point.y - curl_point_position.y));

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (distance_to_opposite_side < curl_radius * 2)                  //we need to reduce radius when page should lay on it's place
      curl_radius *= distance_to_opposite_side / (curl_radius * 2);

    curl_radius = stl::max (0.f, curl_radius);

    float flip_width  = (page_size.y - corner_position.y) / tan (fabs (x_flip_angle)) + corner_position.x,
          flip_height = flip_width * tan (fabs (x_flip_angle / 2.f)),
          curl_angle  = -atan2 (flip_width, flip_height);

    if (curl_corner_position.y > 0 || curl_angle > -EPS)
      curl_angle += PI;

    if (current_page_is_rigid)
      curl_corner_position.x = stl::max (0.f, stl::min (curl_point_position.x, page_size.x * 2));

    curled_page->Curl (curl_corner_position, PageCurlCorner_LeftTop, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    context.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, context, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    float min_s, max_s, min_t, max_t;

    GetTexCoords (false, page_materials [curled_right_page_type].get (), page_textures_sizes [curled_right_page_type], min_s, max_s, min_t, max_t);

    curled_page->SetTexCoords (max_s, min_t, min_s, max_t);

    float max_shadow = curl_radius / page_curl->CurlRadius ();

    curled_page->CalculateShadow (true, max_shadow);

    curled_page->Draw (device, context);

    context.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, context, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    curled_page->CalculateShadow (false, max_shadow);

    GetTexCoords (true, page_materials [curled_left_page_type].get (), page_textures_sizes [curled_left_page_type], min_s, max_s, min_t, max_t);

    curled_page->SetTexCoords (min_s, min_t, max_s, max_t);

    curled_page->Draw (device, context);

    context.RSSetState (rasterizer_no_cull_state.get ());

    DrawStaticPages (device, context);

    DrawShadows (device, context, PageCurlCorner_LeftTop, curl_radius, true);
  }

  void DrawLeftBottomCornerFlip (low_level::IDevice& device, low_level::IDeviceContext& context)
  {
    if (page_curl->Mode () == PageCurlMode_SinglePage)
      throw xtl::format_operation_exception ("render::obsolete::render2d::RenderablePageCurl::DrawLeftBottomCornerFlip", "Can't draw flip for left corner in single page mode");

    const math::vec2f& curl_point_position = page_curl->CurlPointPosition ();
    const math::vec2f& curl_point          = page_curl->CurlPoint ();
          math::vec2f  page_size           = page_curl->Size ();

    math::vec2f corner_position (curl_point_position.x + curl_point.x, curl_point_position.y - curl_point.y);

    if (fabs (corner_position.y) < EPS) //workaround for artifacts with zero curl height
      corner_position.y = EPS;

    float max_corner_height_offset = corner_position.x * MAX_CORNER_HEIGHT_OFFSET_FACTOR;

    if (corner_position.y < 0 && -corner_position.y > max_corner_height_offset)
      corner_position.y = -max_corner_height_offset;

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

    float distance_to_opposite_side = math::length (math::vec2f (2 * page_size.x - stl::min (2 * page_size.x, curl_corner_position.x), curl_point.y - curl_point_position.y));

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (distance_to_opposite_side < curl_radius * 2)                  //we need to reduce radius when page should lay on it's place
      curl_radius *= distance_to_opposite_side / (curl_radius * 2);

    curl_radius = stl::max (0.f, curl_radius);

    float flip_width  = corner_position.y / tan (x_flip_angle) + corner_position.x,
          flip_height = flip_width * tan (x_flip_angle / 2.f),
          curl_angle  = atan2 (flip_width, flip_height);

    if (curl_corner_position.y < page_size.y || curl_angle < EPS)
      curl_angle += PI;

    if (current_page_is_rigid)
      curl_corner_position.x = stl::max (0.f, stl::min (curl_point_position.x, page_size.x * 2));

    curled_page->Curl (curl_corner_position, PageCurlCorner_LeftBottom, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    context.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, context, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    float min_s, max_s, min_t, max_t;

    GetTexCoords (false, page_materials [curled_right_page_type].get (), page_textures_sizes [curled_right_page_type], min_s, max_s, min_t, max_t);

    curled_page->SetTexCoords (max_s, min_t, min_s, max_t);

    float max_shadow = curl_radius / page_curl->CurlRadius ();

    curled_page->CalculateShadow (true, max_shadow);

    curled_page->Draw (device, context);

    context.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, context, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    curled_page->CalculateShadow (false, max_shadow);

    GetTexCoords (true, page_materials [curled_left_page_type].get (), page_textures_sizes [curled_left_page_type], min_s, max_s, min_t, max_t);

    curled_page->SetTexCoords (min_s, min_t, max_s, max_t);

    curled_page->Draw (device, context);

    context.RSSetState (rasterizer_no_cull_state.get ());

    DrawStaticPages (device, context);

    DrawShadows (device, context, PageCurlCorner_LeftBottom, curl_radius, true);
  }

  void DrawRightTopCornerFlip (low_level::IDevice& device, low_level::IDeviceContext& context)
  {
    const math::vec2f& total_size          = page_curl->Size ();
    const math::vec2f& curl_point_position = page_curl->CurlPointPosition ();
    const math::vec2f& curl_point          = page_curl->CurlPoint ();
          math::vec2f  page_size           = total_size;

    math::vec2f corner_position (curl_point_position.x + curl_point.x - page_size.x, curl_point_position.y - curl_point.y + page_size.y);

    if (fabs (corner_position.y) < EPS) //workaround for artifacts with zero curl height
      corner_position.y = EPS;

    float max_corner_height_offset = fabs (page_size.x - corner_position.x) * MAX_CORNER_HEIGHT_OFFSET_FACTOR;

    if (corner_position.y > page_size.y && corner_position.y - page_size.y > max_corner_height_offset)
      corner_position.y = page_size.y + max_corner_height_offset;

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

    float distance_to_opposite_side = math::length (math::vec2f (stl::min (0.f, -page_size.x - curl_corner_position.x), curl_point.y - curl_point_position.y));

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (distance_to_opposite_side < curl_radius * 2)                  //we need to reduce radius when page should lay on it's place
      curl_radius *= distance_to_opposite_side / (curl_radius * 2);

    curl_radius = stl::max (0.f, curl_radius);

    float distance_to_right_edge = total_size.x - corner_position.x,
          flip_width             = (page_size.y - corner_position.y) / tan (fabs (x_flip_angle)) + distance_to_right_edge,
          flip_height            = flip_width * tan (fabs (x_flip_angle / 2.f)),
          curl_angle             = atan2 (flip_width, flip_height);

    if (curl_corner_position.y <= 0 && curl_angle > EPS)
      curl_angle += PI;

    if (current_page_is_rigid)
      curl_corner_position.x = stl::max (-page_size.x, stl::min (curl_point_position.x - page_size.x, page_size.x));

    curled_page->Curl (curl_corner_position, PageCurlCorner_RightTop, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (page_size.x, 0, 0)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    context.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, context, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    float min_s, max_s, min_t, max_t;

    GetTexCoords (true, page_materials [curled_left_page_type].get (), page_textures_sizes [curled_left_page_type], min_s, max_s, min_t, max_t);

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      curled_page->SetTexCoords (max_s, min_t, min_s, max_t);
    else
      curled_page->SetTexCoords (min_s, min_t, max_s, max_t);

    float max_shadow = curl_radius / page_curl->CurlRadius ();

    curled_page->CalculateShadow (true, max_shadow);

    curled_page->Draw (device, context);

    context.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, context, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    curled_page->CalculateShadow (false, max_shadow);

    GetTexCoords (false, page_materials [curled_right_page_type].get (), page_textures_sizes [curled_right_page_type], min_s, max_s, min_t, max_t);

    curled_page->SetTexCoords (min_s, min_t, max_s, max_t);

    curled_page->Draw (device, context);

    context.RSSetState (rasterizer_no_cull_state.get ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    DrawStaticPages (device, context);

    DrawShadows (device, context, PageCurlCorner_RightTop, curl_radius, false);
  }

  void DrawRightBottomCornerFlip (low_level::IDevice& device, low_level::IDeviceContext& context)
  {
    const math::vec2f& total_size          = page_curl->Size ();
    const math::vec2f& curl_point_position = page_curl->CurlPointPosition ();
    const math::vec2f& curl_point          = page_curl->CurlPoint ();
          math::vec2f  page_size           = total_size;

    math::vec2f corner_position (curl_point_position.x + curl_point.x - page_size.x, curl_point_position.y - curl_point.y);

    if (fabs (corner_position.y) < EPS) //workaround for artifacts with zero curl height
      corner_position.y = EPS;

    float max_corner_height_offset = fabs (page_size.x - corner_position.x) * MAX_CORNER_HEIGHT_OFFSET_FACTOR;

    if (corner_position.y < 0 && -corner_position.y > max_corner_height_offset)
      corner_position.y = -max_corner_height_offset;

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

    float distance_to_opposite_side = math::length (math::vec2f (stl::min (0.f, -page_size.x - curl_corner_position.x), curl_point.y - curl_point_position.y));

    curl_corner_position.y = page_size.y - curl_corner_position.y;

    if (distance_to_opposite_side < curl_radius * 2)                  //we need to reduce radius when page should lay on it's place
      curl_radius *= distance_to_opposite_side / (curl_radius * 2);

    curl_radius = stl::max (0.f, curl_radius);

    float distance_to_right_edge = total_size.x - corner_position.x,
          flip_width             = corner_position.y / tan (x_flip_angle) + distance_to_right_edge,
          flip_height            = flip_width * tan (x_flip_angle / 2.f),
          curl_angle             = -atan2 (flip_width, flip_height);

    if (curl_corner_position.y >= page_size.y && curl_angle < -EPS)
      curl_angle -= PI;

    if (current_page_is_rigid)
      curl_corner_position.x = stl::max (-page_size.x, stl::min (curl_point_position.x - page_size.x, page_size.x));

    curled_page->Curl (curl_corner_position, PageCurlCorner_RightBottom, curl_radius, curl_angle,
                       page_curl->FindBestCurlSteps (), page_curl->BindingMismatchWeight ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (page_size.x, 0, 0)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    context.RSSetState (rasterizer_cull_back_state.get ());

    scene_graph::PageCurlPageType curled_left_page_type = GetCurledLeftPageType ();

    BindMaterial (device, context, page_materials [curled_left_page_type].get (), page_textures [curled_left_page_type].get ());

    float min_s, max_s, min_t, max_t;

    GetTexCoords (true, page_materials [curled_left_page_type].get (), page_textures_sizes [curled_left_page_type], min_s, max_s, min_t, max_t);

    if (page_curl->Mode () != PageCurlMode_SinglePage)
      curled_page->SetTexCoords (max_s, min_t, min_s, max_t);
    else
      curled_page->SetTexCoords (min_s, min_t, max_s, max_t);

    float max_shadow = curl_radius / page_curl->CurlRadius ();

    curled_page->CalculateShadow (true, max_shadow);

    curled_page->Draw (device, context);

    context.RSSetState (rasterizer_cull_front_state.get ());

    scene_graph::PageCurlPageType curled_right_page_type = GetCurledRightPageType ();

    BindMaterial (device, context, page_materials [curled_right_page_type].get (), page_textures [curled_right_page_type].get ());

    curled_page->CalculateShadow (false, max_shadow);

    GetTexCoords (false, page_materials [curled_right_page_type].get (), page_textures_sizes [curled_right_page_type], min_s, max_s, min_t, max_t);

    curled_page->SetTexCoords (min_s, min_t, max_s, max_t);

    curled_page->Draw (device, context);

    context.RSSetState (rasterizer_no_cull_state.get ());

    if (page_curl->Mode () != PageCurlMode_SinglePage)
    {
      ProgramParameters program_parameters;

      program_parameters.view_matrix       = math::translate (-view_point);
      program_parameters.projection_matrix = projection;
      program_parameters.object_matrix     = page_curl->WorldTM () * math::scale (math::vec3f (1, 1, -1)) * math::translate (math::vec3f (-page_curl->Size ().x / 2, -page_curl->Size ().y / 2, 0));

      constant_buffer->SetData (0, sizeof (program_parameters), &program_parameters);
    }

    DrawStaticPages (device, context);

    DrawShadows (device, context, PageCurlCorner_RightBottom, curl_radius, false);
  }

  //Рисование лежащих страниц
  void DrawStaticPages (low_level::IDevice& device, low_level::IDeviceContext& context)
  {
	const math::vec2f& curl_point          = page_curl->CurlPoint ();
	const math::vec2f& curl_point_position = page_curl->CurlPointPosition ();
	bool               page_in_place       = (fabs (curl_point.x - curl_point_position.x) < EPS) && (fabs (curl_point.y - curl_point_position.y) < EPS);
    const math::vec2f& size                = page_curl->Size ();
    bool               left_corner_curled  = curl_point.x < size.x / 2;

    int   left_page_type   = -1,
          right_page_type  = -1;
    float left_page_width  = size.x * 0.5f,
          right_page_width = left_page_width;

    context.ISSetVertexBuffer (0, quad_vertex_buffer.get ());

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

    if (left_page_type >= 0 && !(left_corner_curled && page_in_place))
    {
      RenderableVertex vertices [4];

      for (size_t i = 0; i < 4; i++)
        vertices [i].color = page_color;

      float min_s, max_s, min_t, max_t;

      GetTexCoords (true, page_materials [left_page_type].get (), page_textures_sizes [left_page_type], min_s, max_s, min_t, max_t);

      vertices [0].position = math::vec3f (0, 0, STATIC_PAGES_Z_OFFSET);
      vertices [1].position = math::vec3f (left_page_width, 0, STATIC_PAGES_Z_OFFSET);
      vertices [2].position = math::vec3f (0, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [3].position = math::vec3f (left_page_width, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [0].texcoord = math::vec2f (min_s, min_t);
      vertices [1].texcoord = math::vec2f (max_s, min_t);
      vertices [2].texcoord = math::vec2f (min_s, max_t);
      vertices [3].texcoord = math::vec2f (max_s, max_t);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      BindMaterial (device, context, page_materials [left_page_type].get (), page_textures [left_page_type].get ());

      context.Draw (low_level::PrimitiveType_TriangleStrip, 0, 4);
    }

    if (right_page_type >= 0 && !(!left_corner_curled && page_in_place))
    {
      RenderableVertex vertices [4];

      for (size_t i = 0; i < 4; i++)
        vertices [i].color = page_color;

      float min_s, max_s, min_t, max_t;

      GetTexCoords (false, page_materials [right_page_type].get (), page_textures_sizes [right_page_type], min_s, max_s, min_t, max_t);

      vertices [0].position = math::vec3f (left_page_width, 0, STATIC_PAGES_Z_OFFSET);
      vertices [1].position = math::vec3f (left_page_width + right_page_width, 0, STATIC_PAGES_Z_OFFSET);
      vertices [2].position = math::vec3f (left_page_width, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [3].position = math::vec3f (left_page_width + right_page_width, size.y, STATIC_PAGES_Z_OFFSET);
      vertices [0].texcoord = math::vec2f (min_s, min_t);
      vertices [1].texcoord = math::vec2f (max_s, min_t);
      vertices [2].texcoord = math::vec2f (min_s, max_t);
      vertices [3].texcoord = math::vec2f (max_s, max_t);

      quad_vertex_buffer->SetData (0, sizeof (vertices), vertices);

      BindMaterial (device, context, page_materials [right_page_type].get (), page_textures [right_page_type].get ());

      context.Draw (low_level::PrimitiveType_TriangleStrip, 0, 4);
    }
  }

  void BindMaterial (low_level::IDevice& device, low_level::IDeviceContext& context, SpriteMaterial* material, ILowLevelTexture* texture)
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

    context.OSSetBlendState (material_blend_state.get ());
    context.SSSetTexture    (0, texture->GetTexture ());
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
