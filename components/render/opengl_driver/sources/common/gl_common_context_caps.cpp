#include <shared/context_caps.h>

using namespace render::low_level::opengl;

namespace
{

/*
    Версии OpenGL
*/

enum GlVersion
{
  GlVersion_1_2,
  GlVersion_1_3,
  GlVersion_1_4,
  GlVersion_1_5,
  GlVersion_2_0,
  GlVersion_2_1,
  
  GlVersion_Num
};

/*
    Таблица множеств расширений, вошедших в версии стандарта OpenGL
*/

class StandartExtensionSet
{
  public:
    ExtensionSet versions [GlVersion_Num];

    static const StandartExtensionSet& Instance ()
    {
      return common::Singleton<StandartExtensionSet>::Instance ();
    }

    StandartExtensionSet ()
    {
      versions [GlVersion_1_2].SetGroup ("GL_EXT_texture3D GL_EXT_bgra GL_EXT_packed_pixels GL_EXT_rescale_normal GL_EXT_separate_specular_color "
                       "GL_SGIS_texture_edge_clamp GL_SGIS_texture_lod GL_EXT_draw_range_elements GL_EXT_color_table "
                       "GL_EXT_color_subtable GL_EXT_convolution GL_HP_convolution_border_modes GL_SGI_color_matrix "
                       "GL_EXT_histogram GL_EXT_blend_color GL_EXT_blend_minmax GL_EXT_blend_subtract", true);

      versions [GlVersion_1_3] |= versions [GlVersion_1_2];

      versions [GlVersion_1_3].SetGroup ("GL_ARB_texture_compression GL_ARB_texture_cube_map GL_ARB_multisample GL_ARB_multitexture "
                       "GL_ARB_texture_env_add GL_ARB_texture_env_combine GL_ARB_texture_env_dot3 GL_ARB_texture_border_clamp "
                       "GL_ARB_transpose_matrix", true);

      versions [GlVersion_1_4] |= versions [GlVersion_1_3];

      versions [GlVersion_1_4].SetGroup ("GL_SGIS_generate_mipmap GL_NV_blend_square GL_ARB_depth_texture GL_ARB_shadow GL_EXT_fog_coord "
                       "GL_EXT_multi_draw_arrays GL_ARB_point_parameters GL_EXT_secondary_color GL_EXT_blend_func_separate "
                       "GL_EXT_stencil_wrap GL_ARB_texture_env_crossbar GL_EXT_texture_lod_bias GL_ARB_texture_mirrored_repeat "
                       "GL_ARB_window_pos GL_ARB_vertex_program", true);

      versions [GlVersion_1_5] |= versions [GlVersion_1_4];

      versions [GlVersion_1_5].SetGroup ("GL_ARB_vertex_buffer_object GL_ARB_occlusion_query GL_EXT_shadow_funcs GL_ARB_fragment_shader "
                       "GL_ARB_fragment_program GL_ARB_shader_objects GL_ARB_vertex_shader", true);

      versions [GlVersion_2_0] |= versions [GlVersion_1_5];

      versions [GlVersion_2_0].SetGroup ("GL_ARB_shading_language_100 GL_ARB_draw_buffers GL_ARB_texture_non_power_of_two GL_ARB_point_sprite "
                       "GL_EXT_blend_equation_separate GL_EXT_stencil_two_side GL_ATI_separate_stencil", true);

      versions [GlVersion_2_1] |= versions [GlVersion_2_0];

      versions [GlVersion_2_1].SetGroup ("GL_ARB_pixel_buffer_object GL_EXT_texture_sRGB", true);      
    }
};

}

/*
    Реализация ContextCaps
*/

ContextCaps::ContextCaps ()
{
  memset (this, 0, sizeof *this);
}

void ContextCaps::Init (const ExtensionSet& available_extension_set, const ExtensionSet& enabled_extension_set)
{
    //маркеры исползуемых расширений

  static Extension ARB_depth_texture              = "GL_ARB_depth_texture",
                   ARB_fragment_shader            = "GL_ARB_fragment_shader",
                   ARB_multisample                = "GL_ARB_multisample",
                   ARB_multitexture               = "GL_ARB_multitexture",
                   ARB_occlusion_query            = "GL_ARB_occlusion_query",
                   ARB_shading_language_100       = "GL_ARB_shading_language_100",
                   ARB_shadow                     = "GL_ARB_shadow",
                   ARB_texture_border_clamp       = "GL_ARB_texture_border_clamp",
                   ARB_texture_compression        = "GL_ARB_texture_compression",
                   ARB_texture_cube_map           = "GL_ARB_texture_cube_map",
                   ARB_texture_mirrored_repeat    = "GL_ARB_texture_mirrored_repeat",                   
                   ARB_texture_non_power_of_two   = "GL_ARB_texture_non_power_of_two",
                   ARB_texture_rectangle          = "GL_ARB_texture_rectangle",                   
                   ARB_transpose_matrix           = "GL_ARB_transpose_matrix",
                   ARB_vertex_buffer_object       = "GL_ARB_vertex_buffer_object",
                   ARB_vertex_shader              = "GL_ARB_vertex_shader",
                   ARB_window_pos                 = "GL_ARB_window_pos",
                   ATI_separate_stencil           = "GL_ATI_separate_stencil",
                   EXT_blend_equation_separate    = "GL_EXT_blend_equation_separate",
                   EXT_blend_func_separate        = "GL_EXT_blend_func_separate",
                   EXT_blend_minmax               = "GL_EXT_blend_minmax",
                   EXT_blend_subtract             = "GL_EXT_blend_subtract",
                   EXT_framebuffer_object         = "GL_EXT_framebuffer_object",
                   EXT_packed_depth_stencil       = "GL_EXT_packed_depth_stencil",
                   EXT_shadow_funcs               = "GL_EXT_shadow_funcs",
                   EXT_stencil_two_side           = "GL_EXT_stencil_two_side",
                   EXT_texture_compression_s3tc   = "GL_EXT_texture_compression_s3tc",
                   EXT_texture_cube_map           = "GL_EXT_texture_cube_map",
                   EXT_texture_filter_anisotropic = "GL_EXT_texture_filter_anisotropic",
                   EXT_texture_lod_bias           = "GL_EXT_texture_lod_bias",
                   EXT_texture3D                  = "GL_EXT_texture3D",
                   NV_texture_rectangle           = "GL_NV_texture_rectangle",
                   SGIS_generate_mipmap           = "GL_SGIS_generate_mipmap",
                   SGIS_texture_lod               = "GL_SGIS_texture_lod";

  static Extension versions [GlVersion_Num] = {"GL_VERSION_1_2", "GL_VERSION_1_3", "GL_VERSION_1_4", "GL_VERSION_1_5",
                                               "GL_VERSION_2_0", "GL_VERSION_2_1"};

    //получение маски доступных расширений
    
  ExtensionSet ext = available_extension_set;  

  const StandartExtensionSet& std_extension_set = StandartExtensionSet::Instance ();

  for (size_t version_id = GlVersion_1_2; version_id != GlVersion_Num; version_id++)
  {
    if (ext.Get (versions [version_id]))
      (ext |= std_extension_set.versions [version_id]) &= enabled_extension_set;
  }

     //инициализация таблицы возможностей контекста

  has_arb_depth_texture              = ext.Get (ARB_depth_texture);
  has_arb_fragment_shader            = ext.Get (ARB_fragment_shader);
  has_arb_multisample                = ext.Get (ARB_multisample);
  has_arb_multitexture               = ext.Get (ARB_multitexture);
  has_arb_occlusion_query            = ext.Get (ARB_occlusion_query);
  has_arb_shading_language_100       = ext.Get (ARB_shading_language_100);
  has_arb_shadow                     = ext.Get (ARB_shadow);
  has_arb_texture_border_clamp       = ext.Get (ARB_texture_border_clamp);
  has_arb_texture_cube_map           = ext.Get (ARB_texture_cube_map) || ext.Get (EXT_texture_cube_map);
  has_arb_texture_mirrored_repeat    = ext.Get (ARB_texture_mirrored_repeat);
  has_arb_texture_non_power_of_two   = ext.Get (ARB_texture_non_power_of_two);
  has_arb_texture_rectangle          = ext.Get (ARB_texture_rectangle) || ext.Get (NV_texture_rectangle);    
  has_arb_transpose_matrix           = ext.Get (ARB_transpose_matrix);
  has_arb_vertex_buffer_object       = ext.Get (ARB_vertex_buffer_object);
  has_arb_vertex_shader              = ext.Get (ARB_vertex_shader);
  has_arb_window_pos                 = ext.Get (ARB_window_pos);
  has_ati_separate_stencil           = ext.Get (ATI_separate_stencil);
  has_ext_blend_equation_separate    = ext.Get (EXT_blend_equation_separate);
  has_ext_blend_func_separate        = ext.Get (EXT_blend_func_separate);
  has_ext_blend_minmax               = ext.Get (EXT_blend_minmax);
  has_ext_blend_subtract             = ext.Get (EXT_blend_subtract);
  has_ext_framebuffer_object         = ext.Get (EXT_framebuffer_object);
  has_ext_packed_depth_stencil       = ext.Get (EXT_packed_depth_stencil);
  has_ext_shadow_funcs               = ext.Get (EXT_shadow_funcs);
  has_ext_stencil_two_side           = ext.Get (EXT_stencil_two_side);
  has_ext_texture_compression_s3tc   = ext.Get (ARB_texture_compression) && ext.Get (EXT_texture_compression_s3tc);
  has_ext_texture_filter_anisotropic = ext.Get (EXT_texture_filter_anisotropic);
  has_ext_texture_lod_bias           = ext.Get (EXT_texture_lod_bias);
  has_ext_texture3d                  = ext.Get (EXT_texture3D);
  has_sgis_generate_mipmap           = ext.Get (SGIS_generate_mipmap);  
  has_sgis_texture_lod               = ext.Get (SGIS_texture_lod);

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, (GLint*)&max_texture_size);

  if (has_arb_texture_rectangle) glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, (GLint*)&max_rectangle_texture_size);
  if (has_arb_texture_cube_map)  glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, (GLint*)&max_cube_map_texture_size);
  if (has_ext_texture3d)         glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE_EXT, (GLint*)&max_3d_texture_size);

  if (has_ext_texture_filter_anisotropic) glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint*)&max_anisotropy);
  else                                    max_anisotropy = 1;
  
  if (has_arb_multitexture) glGetIntegerv (GL_MAX_TEXTURE_UNITS, (GLint*)&texture_units_count);
  else                      texture_units_count = 1;

  glActiveTexture_fn           = glActiveTexture ? glActiveTexture : glActiveTextureARB;
  glBindBuffer_fn              = glBindBuffer ? glBindBuffer : glBindBufferARB;
  glBufferData_fn              = glBufferData ? glBufferData : glBufferDataARB;
  glBufferSubData_fn           = glBufferSubData ? glBufferSubData : glBufferSubDataARB;
  glClientActiveTexture_fn     = glClientActiveTexture ? glClientActiveTexture : glClientActiveTextureARB;
  glCompressedTexSubImage2D_fn = glCompressedTexSubImage2D ? glCompressedTexSubImage2D : glCompressedTexSubImage2DARB;
  glDeleteBuffers_fn           = glDeleteBuffers ? glDeleteBuffers : glDeleteBuffersARB;
  glGenBuffers_fn              = glGenBuffers ? glGenBuffers : glGenBuffersARB;
  glGetBufferSubData_fn        = glGetBufferSubData ? glGetBufferSubData : glGetBufferSubDataARB;
  glGetCompressedTexImage_fn   = glGetCompressedTexImage ? glGetCompressedTexImage : glGetCompressedTexImageARB;
  glGetUniformLocation_fn      = glGetUniformLocation ? glGetUniformLocation : (PFNGLGETUNIFORMLOCATIONPROC)glGetUniformLocationARB;
  glLoadTransposeMatrixd_fn    = glLoadTransposeMatrixd ? glLoadTransposeMatrixd : (PFNGLLOADTRANSPOSEMATRIXDPROC)glLoadTransposeMatrixdARB;
  glLoadTransposeMatrixf_fn    = glLoadTransposeMatrixf ? glLoadTransposeMatrixf : (PFNGLLOADTRANSPOSEMATRIXFPROC)glLoadTransposeMatrixfARB;
  glMultTransposeMatrixd_fn    = glMultTransposeMatrixd ? glMultTransposeMatrixd : (PFNGLMULTTRANSPOSEMATRIXDPROC)glMultTransposeMatrixdARB;
  glMultTransposeMatrixf_fn    = glMultTransposeMatrixf ? glMultTransposeMatrixf : (PFNGLMULTTRANSPOSEMATRIXFPROC)glMultTransposeMatrixfARB;
  glUniform1fv_fn              = glUniform1fv ? glUniform1fv : glUniform1fvARB;
  glUniform1iv_fn              = glUniform1iv ? glUniform1iv : glUniform1ivARB;
  glUniform2fv_fn              = glUniform2fv ? glUniform2fv : glUniform2fvARB;
  glUniform2iv_fn              = glUniform2iv ? glUniform2iv : glUniform2ivARB;
  glUniform3fv_fn              = glUniform3fv ? glUniform3fv : glUniform3fvARB;
  glUniform3iv_fn              = glUniform3iv ? glUniform3iv : glUniform3ivARB;
  glUniform4fv_fn              = glUniform4fv ? glUniform4fv : glUniform4fvARB;
  glUniform4iv_fn              = glUniform4iv ? glUniform4iv : glUniform4ivARB;
  glUniformMatrix2fv_fn        = glUniformMatrix2fv ? glUniformMatrix2fv : glUniformMatrix2fvARB;
  glUniformMatrix3fv_fn        = glUniformMatrix3fv ? glUniformMatrix3fv : glUniformMatrix3fvARB;
  glUniformMatrix4fv_fn        = glUniformMatrix4fv ? glUniformMatrix4fv : glUniformMatrix4fvARB;
  glUseProgram_fn              = glUseProgram ? glUseProgram : glUseProgramObjectARB;
}
