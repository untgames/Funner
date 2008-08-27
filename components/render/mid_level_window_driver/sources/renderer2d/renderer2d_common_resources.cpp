#include "shared.h"

using namespace render::low_level;
using namespace render::mid_level::renderer2d;
using namespace render::mid_level::window_driver::renderer2d;

namespace
{

/*const char* PIXEL_SHADER =
"uniform sampler2D texture; varying vec4 color; void main (void) {gl_FragColor = vec4 (vec3 (texture2D (texture, vec2 (gl_TexCoord[0]))), color.w);}";
const char* VERTEX_SHADER = 
"uniform mat4 currentViewMatrix; uniform mat4 currentProjMatrix; varying vec4 color; void main (void) {gl_Position = currentViewMatrix * currentProjMatrix * gl_Vertex; gl_TexCoord[0] = gl_MultiTexCoord0; color = gl_Color;}";

    ShaderDesc shader_descs [] = {
      {"p_shader", strlen (PIXEL_SHADER), PIXEL_SHADER, "glsl.ps", ""},
      {"v_shader", strlen (VERTEX_SHADER), VERTEX_SHADER, "glsl.vs", ""}
    };

    program = ProgramPtr (device->CreateProgram (sizeof shader_descs / sizeof *shader_descs, shader_descs, &default_error_log), false);
*/

//текст шейдера программы по умолчанию
const char* DEFAULT_SHADER_SOURCE_CODE =
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

//текст шейдера программы альфа-отсечения
const char* ALPHA_CLAMP_SHADER_SOURCE_CODE =
"Parameters\n"
"{\n"
"  float4x4 currentViewMatrix currentProjMatrix\n"
"  float currentAlphaReference\n"
"}\n"
"ProjectionMatrix currentProjMatrix\n"
"ViewMatrix       currentViewMatrix\n"
"AlphaCompareMode Greater\n"
"AlphaReference   currentAlphaReference\n"
"Texmap0\n"
"{\n"
"  TexcoordU Explicit\n"
"  TexcoordV Explicit\n"
"  Blend     Modulate\n"
"}";

//создание состояния смешивания цветов
BlendStatePtr create_blend_state
 (IDevice&      device,
  bool          blend_enable,
  BlendArgument src_arg,
  BlendArgument dst_color_arg,
  BlendArgument dst_alpha_arg)
{
  BlendDesc blend_desc;

  memset (&blend_desc, 0, sizeof (blend_desc));

  blend_desc.blend_enable                     = blend_enable;
  blend_desc.blend_color_operation            = BlendOperation_Add;
  blend_desc.blend_alpha_operation            = BlendOperation_Add;
  blend_desc.blend_color_source_argument      = src_arg;
  blend_desc.blend_color_destination_argument = dst_color_arg;
  blend_desc.blend_alpha_source_argument      = src_arg;
  blend_desc.blend_alpha_destination_argument = dst_alpha_arg;
  blend_desc.color_write_mask                 = ColorWriteFlag_All;

  return BlendStatePtr (device.CreateBlendState (blend_desc), false);
}

BlendStatePtr create_blend_state (IDevice& device, BlendArgument src_arg, BlendArgument dst_arg)
{
  return create_blend_state (device, true, src_arg, dst_arg, dst_arg);
}

BlendStatePtr create_blend_state (IDevice& device)
{
  return create_blend_state (device, false, BlendArgument_Zero, BlendArgument_Zero, BlendArgument_Zero);
}

//создание состояния буфера попиксельного отсечения
DepthStencilStatePtr create_depth_stencil_state (IDevice& device, bool depth_write)
{
  DepthStencilDesc depth_stencil_desc;

  memset (&depth_stencil_desc, 0, sizeof (depth_stencil_desc));

  depth_stencil_desc.depth_test_enable   = true;
  depth_stencil_desc.depth_write_enable  = depth_write;
  depth_stencil_desc.depth_compare_mode  = CompareMode_Less;

  return DepthStencilStatePtr (device.CreateDepthStencilState (depth_stencil_desc), false);
}

//протоколирование компилятора шейдеров
void shader_error_log (const char* message)
{
  common::LogSystem::Print ("render.mid_level.window_driver.renderer2d.Renderer", message);
}

//создание программы
ProgramPtr create_program (IDevice& device, const char* shader_name, const char* shader_source)
{
  ShaderDesc shader_desc;

  memset (&shader_desc, 0, sizeof (shader_desc));

  shader_desc.name             = shader_name;
  shader_desc.source_code_size = strlen (shader_source);
  shader_desc.source_code      = shader_source;
  shader_desc.profile          = "fpp";
  shader_desc.options          = "";

  return ProgramPtr (device.CreateProgram (1, &shader_desc, &shader_error_log), false);
}

//создание состояния расположения параметров шейдинга
ProgramParametersLayoutPtr create_program_parameters (IDevice& device)
{
  ProgramParameter program_parameters [3];
  
  memset (program_parameters, 0, sizeof (program_parameters));
   
  program_parameters [0].name   = "currentViewMatrix";
  program_parameters [0].type   = ProgramParameterType_Float4x4;
  program_parameters [0].slot   = 0;
  program_parameters [0].count  = 1;
  program_parameters [0].offset = offsetof (CommonProgramParameters, view_matrix);

  program_parameters [1].name   = "currentProjMatrix";
  program_parameters [1].type   = ProgramParameterType_Float4x4;
  program_parameters [1].slot   = 0;
  program_parameters [1].count  = 1;
  program_parameters [1].offset = offsetof (CommonProgramParameters, projection_matrix);
  
  program_parameters [2].name   = "currentAlphaReference";
  program_parameters [2].type   = ProgramParameterType_Float;
  program_parameters [2].slot   = 1;
  program_parameters [2].count  = 1;
  program_parameters [2].offset = offsetof (DynamicProgramParameters, alpha_reference);

  ProgramParametersLayoutDesc program_parameters_layout_desc;

  memset (&program_parameters_layout_desc, 0, sizeof (program_parameters_layout_desc));

  program_parameters_layout_desc.parameters_count = sizeof (program_parameters) / sizeof (*program_parameters);
  program_parameters_layout_desc.parameters       = program_parameters;

  return ProgramParametersLayoutPtr (device.CreateProgramParametersLayout (program_parameters_layout_desc), false);  
}

//создание сэмплера
SamplerStatePtr create_sampler (IDevice& device)
{
  SamplerDesc sampler_desc;

  memset (&sampler_desc, 0, sizeof (sampler_desc));

  sampler_desc.min_filter           = TexMinFilter_LinearMipLinear;
  sampler_desc.mag_filter           = TexMagFilter_Linear;
  sampler_desc.max_anisotropy       = 1;
  sampler_desc.wrap_u               = TexcoordWrap_Repeat;
  sampler_desc.wrap_v               = TexcoordWrap_Repeat;
  sampler_desc.comparision_function = CompareMode_AlwaysPass;
  sampler_desc.min_lod              = 0;
  sampler_desc.max_lod              = FLT_MAX;

  return SamplerStatePtr (device.CreateSamplerState (sampler_desc), false);
}

//создание объекта расположения входной геометрии
InputLayoutPtr create_input_layout (IDevice& device)
{
  VertexAttribute attributes [3];

  memset (attributes, 0, sizeof (attributes));      

  attributes [0].semantic = VertexAttributeSemantic_Position;
  attributes [0].format   = InputDataFormat_Vector3;
  attributes [0].type     = InputDataType_Float;
  attributes [0].slot     = 0;
  attributes [0].offset   = offsetof (RenderableVertex, position);
  attributes [0].stride   = sizeof (RenderableVertex);

  attributes [1].semantic = VertexAttributeSemantic_TexCoord0;
  attributes [1].format   = InputDataFormat_Vector2;
  attributes [1].type     = InputDataType_Float;
  attributes [1].slot     = 0;
  attributes [1].offset   = offsetof (RenderableVertex, texcoord);
  attributes [1].stride   = sizeof (RenderableVertex);

  attributes [2].semantic = VertexAttributeSemantic_Color;
  attributes [2].format   = InputDataFormat_Vector4;
  attributes [2].type     = InputDataType_Float;
  attributes [2].slot     = 0;
  attributes [2].offset   = offsetof (RenderableVertex, color);
  attributes [2].stride   = sizeof (RenderableVertex);

  InputLayoutDesc layout_desc;

  memset (&layout_desc, 0, sizeof layout_desc);

  layout_desc.vertex_attributes_count = sizeof attributes / sizeof *attributes;
  layout_desc.vertex_attributes       = attributes;
  layout_desc.index_type              = InputDataType_UInt;
  layout_desc.index_buffer_offset     = 0;

  return InputLayoutPtr (device.CreateInputLayout (layout_desc), false);
}

}

CommonResources::CommonResources (IDevice* device)
{
  if (!device)
    throw xtl::make_null_argument_exception ("render::mid_level::window_driver::renderer2d::CommonResources::CommonResources", "device");

    //создание состояний входного уровня

  input_layout = create_input_layout (*device);
 
    //создание состояний уровня шейдинга

  default_program = create_program (*device, "render.mid_level.window_driver.renderer2d.Renderer.default_program",
                                    DEFAULT_SHADER_SOURCE_CODE);

  alpha_clamp_program = create_program (*device, "render.mid_level.window_driver.renderer2d.Renderer.alpha_clamp_program",
                                        ALPHA_CLAMP_SHADER_SOURCE_CODE);

  program_parameters_layout = create_program_parameters (*device);

  sampler = create_sampler (*device);  

    //создание состояний выходного уровня
    
//При переносе выше CreateProgram бленд стейты не работают!!!!!!!    ?????

  blend_states [BlendMode_None]        = create_blend_state (*device);
  blend_states [BlendMode_Translucent] = create_blend_state (*device, BlendArgument_SourceAlpha, BlendArgument_InverseSourceAlpha);
  blend_states [BlendMode_Mask]        = create_blend_state (*device, true, BlendArgument_Zero, BlendArgument_SourceColor, BlendArgument_SourceAlpha);
  blend_states [BlendMode_Additive]    = create_blend_state (*device, BlendArgument_One, BlendArgument_One);
  blend_states [BlendMode_AlphaClamp]  = blend_states [BlendMode_None];

  depth_stencil_states [0] = create_depth_stencil_state (*device, false);
  depth_stencil_states [1] = create_depth_stencil_state (*device, true);    
}

render::low_level::IDepthStencilState* CommonResources::GetDepthStencilState (bool depth_write_enabled) 
{ 
  if (depth_write_enabled) return depth_stencil_states[1].get (); 
  else                     return depth_stencil_states[0].get (); 
}
