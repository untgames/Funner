#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::low_level_driver;
using namespace render::mid_level::low_level_driver::renderer2d;

namespace
{

render::low_level::PixelFormat get_pixel_format (media::PixelFormat format)
{
  switch (format)
  {
    case media::PixelFormat_RGB8:  return render::low_level::PixelFormat_RGB8;
    case media::PixelFormat_RGBA8: return render::low_level::PixelFormat_RGBA8;
    default:
      throw xtl::format_operation_exception ("get_pixel_format", "Can't convert media format %s to render format.", media::get_format_name (format));
  }
}

void default_error_log (const char* message)
{
  common::LogSystem::Print ("render.mid_level.low_level_driver.renderer2d.Renderer", message);
}

const char* SHADER_SOURCE_CODE =
"Parameters\n"
"{\n"
"  float4x4 currentViewMatrix currentProjMatrix\n"
"}\n"
"ProjectionMatrix currentProjMatrix\n"
"ViewMatrix       currentViewMatrix\n"
"AlphaCompareMode AlwaysPass\n"
"Texmap0\n"
"{\n"
"  TexcoordU Explicit\n"
"  TexcoordV Explicit\n"
"  Blend     Modulate\n"
"}";

/*const char* PIXEL_SHADER =
"uniform sampler2D texture; varying vec4 color; void main (void) {gl_FragColor = vec4 (vec3 (texture2D (texture, vec2 (gl_TexCoord[0]))), color.w);}";
const char* VERTEX_SHADER = 
"uniform mat4 currentViewMatrix; uniform mat4 currentProjMatrix; varying vec4 color; void main (void) {gl_Position = currentViewMatrix * currentProjMatrix * gl_Vertex; gl_TexCoord[0] = gl_MultiTexCoord0; color = gl_Color;}";
*/
}

/*
    Конструктор
*/

Renderer::Renderer (render::low_level::IDevice* device, render::low_level::ISwapChain* swap_chain)
  : BasicRenderer (device, swap_chain)
{
  try
  {
    device->OSSetDepthStencilState (0);
  
    ShaderDesc shader_desc;

    memset (&shader_desc, 0, sizeof (shader_desc));

    shader_desc.name             = "render.mid_level.low_level_driver.renderer2d.Renderer.common_shader";
    shader_desc.source_code_size = strlen (SHADER_SOURCE_CODE);
    shader_desc.source_code      = SHADER_SOURCE_CODE;
    shader_desc.profile          = "fpp";
    shader_desc.options          = "";

    program = ProgramPtr (device->CreateProgram (1, &shader_desc, &default_error_log), false);
/*
    ShaderDesc shader_descs [] = {
      {"p_shader", strlen (PIXEL_SHADER), PIXEL_SHADER, "glsl.ps", ""},
      {"v_shader", strlen (VERTEX_SHADER), VERTEX_SHADER, "glsl.vs", ""}
    };

    program = ProgramPtr (device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &default_error_log), false);
*/
    device->SSSetProgram (program.get ());

    common_resources = CommonResourcesPtr (new CommonResources (device), false); //При переносе выше CreateProgram бленд стейты не работают!!!!!!!

    ProgramParameter program_parameters[2];
    
    memset (program_parameters, 0, sizeof (program_parameters));
     
    program_parameters[0].name   = "currentViewMatrix";
    program_parameters[0].type   = ProgramParameterType_Float4x4;
    program_parameters[0].slot   = 0;
    program_parameters[0].count  = 1;
    program_parameters[0].offset = offsetof (ProgramParameters, view_matrix);

    program_parameters[1].name   = "currentProjMatrix";
    program_parameters[1].type   = ProgramParameterType_Float4x4;
    program_parameters[1].slot   = 0;
    program_parameters[1].count  = 1;
    program_parameters[1].offset = offsetof (ProgramParameters, projection_matrix);
  
    ProgramParametersLayoutDesc program_parameters_layout_desc;

    memset (&program_parameters_layout_desc, 0, sizeof (program_parameters_layout_desc));

    program_parameters_layout_desc.parameters_count = sizeof (program_parameters) / sizeof (program_parameters[0]);
    program_parameters_layout_desc.parameters       = program_parameters;

    program_parameters_layout = ProgramParametersLayoutPtr (device->CreateProgramParametersLayout (program_parameters_layout_desc), false);

    device->SSSetProgramParametersLayout (program_parameters_layout.get ());

    SamplerDesc sampler_desc;
    
    memset (&sampler_desc, 0, sizeof (sampler_desc));

    sampler_desc.min_filter           = TexMinFilter_LinearMipLinear;
    sampler_desc.mag_filter           = TexMagFilter_Linear;
    sampler_desc.max_anisotropy       = 1;
    sampler_desc.wrap_u               = TexcoordWrap_Mirror;
    sampler_desc.wrap_v               = TexcoordWrap_Mirror;
    sampler_desc.comparision_function = CompareMode_AlwaysPass;
    sampler_desc.min_lod              = 0;
    sampler_desc.max_lod              = FLT_MAX;

    sampler = SamplerStatePtr (device->CreateSamplerState (sampler_desc), false);

    device->SSSetSampler (0, sampler.get ());

    VertexAttribute attributes [3];

    memset (attributes, 0, sizeof (attributes));      

    attributes[0].semantic = VertexAttributeSemantic_Position;
    attributes[0].format   = InputDataFormat_Vector3;
    attributes[0].type     = InputDataType_Float;
    attributes[0].slot     = 0;
    attributes[0].offset   = offsetof (SpriteVertex, position);
    attributes[0].stride   = sizeof (SpriteVertex);

    attributes[1].semantic = VertexAttributeSemantic_TexCoord0;
    attributes[1].format   = InputDataFormat_Vector2;
    attributes[1].type     = InputDataType_Float;
    attributes[1].slot     = 0;
    attributes[1].offset   = offsetof (SpriteVertex, texcoord);
    attributes[1].stride   = sizeof (SpriteVertex);

    attributes[2].semantic = VertexAttributeSemantic_Color;
    attributes[2].format   = InputDataFormat_Vector4;
    attributes[2].type     = InputDataType_Float;
    attributes[2].slot     = 0;
    attributes[2].offset   = offsetof (SpriteVertex, color);
    attributes[2].stride   = sizeof (SpriteVertex);

    InputLayoutDesc layout_desc;
    
    memset (&layout_desc, 0, sizeof layout_desc);
    
    layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
    layout_desc.vertex_attributes       = attributes;
    layout_desc.index_type              = InputDataType_UInt;
    layout_desc.index_buffer_offset     = 0;            

    input_layout = InputLayoutPtr (device->CreateInputLayout (layout_desc), false);

    device->ISSetInputLayout (input_layout.get ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level:low_level_driver::renderer2d::Renderer::Renderer");
    throw;
  }
}

/*
    Создание ресурсов
*/

render::mid_level::renderer2d::ITexture* Renderer::CreateTexture (const media::Image& image)
{
  TextureDesc texture_desc = {TextureDimension_2D, image.Width (), image.Height (), 1, get_pixel_format (image.Format ()), true, AccessFlag_ReadWrite, BindFlag_Texture, UsageMode_Default};

  xtl::com_ptr<render::low_level::ITexture> new_texture (device->CreateTexture (texture_desc), false);

  new_texture->SetData (0, 0, 0, 0, image.Width (), image.Height (), get_pixel_format (image.Format ()), image.Bitmap ());

  return new ImageTexture (new_texture.get ());
}

render::mid_level::renderer2d::ITexture* Renderer::CreateTexture (size_t width, size_t height, media::PixelFormat pixel_format)
{
  TextureDesc texture_desc = {TextureDimension_2D, width, height, 1, get_pixel_format (pixel_format), false, AccessFlag_ReadWrite, BindFlag_Texture | BindFlag_RenderTarget, UsageMode_Default};
  ViewDesc    view_desc = {0, 0};

  return new RenderTargetTexture (device->CreateView (device->CreateTexture (texture_desc), view_desc));
}

render::mid_level::renderer2d::IPrimitive* Renderer::CreatePrimitive ()
{
  return new Primitive;
}

render::mid_level::renderer2d::IFrame* Renderer::CreateFrame ()
{
  return new Frame (common_resources.get (), device.get ());
}
