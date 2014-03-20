#include "shared.h"

using namespace render::low_level::opengl;

namespace
{

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

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
/*  GlVersion_3_0,
  GlVersion_3_1,
  GlVersion_3_2,
  GlVersion_3_3,
  GlVersion_4_0,
  GlVersion_4_1,
  GlVersion_4_2,
  GlVersion_4_3,*/

  GlVersion_Num
};

/*
    Таблица множеств расширений, вошедших в версии стандарта OpenGL
*/

class StandardExtensionSet
{
  public:
    ExtensionSet versions [GlVersion_Num];

    static const StandardExtensionSet& Instance ()
    {
      return *common::Singleton<StandardExtensionSet>::Instance ();
    }

    StandardExtensionSet ()
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

#endif

}

/*
    Реализация ContextCaps
*/

ContextCaps::ContextCaps ()
{
  memset (this, 0, sizeof *this);
}

void ContextCaps::Init (const ExtensionSet& available_extension_set, const ExtensionSet& enabled_extension_set, const ContextSettings& settings)
{
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

    //маркеры используемых расширений

  static Extension ARB_depth_texture              = "GL_ARB_depth_texture",
                   ARB_fragment_shader            = "GL_ARB_fragment_shader",
                   ARB_multisample                = "GL_ARB_multisample",
                   ARB_multitexture               = "GL_ARB_multitexture",
                   ARB_occlusion_query            = "GL_ARB_occlusion_query",
                   ARB_seamless_cube_map          = "GL_ARB_seamless_cube_map",
                   ARB_shader_objects             = "GL_ARB_shader_objects",
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
                   SGIS_texture_edge_clamp        = "GL_SGIS_texture_edge_clamp",
                   SGIS_texture_lod               = "GL_SGIS_texture_lod";

  static Extension versions [GlVersion_Num] = {"GL_VERSION_1_2", "GL_VERSION_1_3", "GL_VERSION_1_4", "GL_VERSION_1_5",
                                               "GL_VERSION_2_0", "GL_VERSION_2_1"};

    //получение маски доступных расширений

  ExtensionSet ext = available_extension_set;

  const StandardExtensionSet& std_extension_set = StandardExtensionSet::Instance ();

  for (size_t version_id = GlVersion_1_2; version_id != GlVersion_Num; version_id++)
  {
    if (ext.Get (versions [version_id]))
      (ext |= std_extension_set.versions [version_id]) &= enabled_extension_set;
  }                   
  
  has_arb_depth_texture              = ext.Get (ARB_depth_texture);
  has_arb_fragment_shader            = ext.Get (ARB_fragment_shader);
  has_arb_multisample                = ext.Get (ARB_multisample);
  has_arb_multitexture               = ext.Get (ARB_multitexture);  
  has_arb_occlusion_query            = ext.Get (ARB_occlusion_query);
  has_arb_seamless_cube_map          = ext.Get (ARB_seamless_cube_map);
  has_arb_shader_objects             = ext.Get (ARB_shader_objects);
  has_arb_shading_language_100       = has_arb_shader_objects && ext.Get (ARB_shading_language_100);
  has_arb_shadow                     = ext.Get (ARB_shadow);
  has_arb_texture_border_clamp       = ext.Get (ARB_texture_border_clamp);
  has_arb_texture_compression        = ext.Get (ARB_texture_compression);
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
  has_ext_texture_compression_s3tc   = has_arb_texture_compression && ext.Get (EXT_texture_compression_s3tc);
  has_ext_texture_filter_anisotropic = ext.Get (EXT_texture_filter_anisotropic);
  has_ext_texture_lod_bias           = ext.Get (EXT_texture_lod_bias);
  has_ext_texture3d                  = ext.Get (EXT_texture3D);
  has_img_texture_compression_pvrtc  = false;
  has_sgis_generate_mipmap           = ext.Get (SGIS_generate_mipmap);
  has_sgis_texture_edge_clamp        = ext.Get (SGIS_texture_edge_clamp);
  has_sgis_texture_lod               = ext.Get (SGIS_texture_lod);
  has_amd_compressed_atc_texture     = false;

  if (has_arb_texture_rectangle) glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB, (GLint*)&max_rectangle_texture_size);
  if (has_arb_texture_cube_map)  glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, (GLint*)&max_cube_map_texture_size);
  if (has_ext_texture3d)         glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE_EXT, (GLint*)&max_3d_texture_size);

  glActiveTexture_fn           = has_arb_multitexture ? (glActiveTexture ? glActiveTexture : glActiveTextureARB) : 0;
  glBindBuffer_fn              = has_arb_vertex_buffer_object ? (glBindBuffer ? glBindBuffer : glBindBufferARB) : 0;
  glBlendEquation_fn           = has_ext_blend_minmax || has_ext_blend_subtract ? (glBlendEquation ? glBlendEquation : glBlendEquationEXT) : 0;
  glBlendEquationSeparate_fn   = has_ext_blend_equation_separate ? (glBlendEquationSeparate ? glBlendEquationSeparate : glBlendEquationSeparateEXT) : 0;
  glBlendFuncSeparate_fn       = has_ext_blend_func_separate ? (glBlendFuncSeparate ? glBlendFuncSeparate : glBlendFuncSeparateEXT) : 0;
  glBufferData_fn              = has_arb_vertex_buffer_object ? (glBufferData ? glBufferData : glBufferDataARB) : 0;
  glBufferSubData_fn           = has_arb_vertex_buffer_object ? (glBufferSubData ? glBufferSubData : glBufferSubDataARB) : 0;
  glClientActiveTexture_fn     = has_arb_multitexture ? (glClientActiveTexture ? glClientActiveTexture : glClientActiveTextureARB) : 0;
  glCompressedTexImage2D_fn    = has_arb_texture_compression ? (glCompressedTexImage2D ? glCompressedTexImage2D : glCompressedTexImage2DARB) : 0;
  glCompressedTexSubImage2D_fn = has_arb_texture_compression ? (glCompressedTexSubImage2D ? glCompressedTexSubImage2D : glCompressedTexSubImage2DARB) : 0;
  glDeleteBuffers_fn           = has_arb_vertex_buffer_object ? (glDeleteBuffers ? glDeleteBuffers : glDeleteBuffersARB) : 0;
  glGenBuffers_fn              = has_arb_vertex_buffer_object ? (glGenBuffers ? glGenBuffers : glGenBuffersARB) : 0;
  glGetBufferSubData_fn        = has_arb_vertex_buffer_object ? (glGetBufferSubData ? glGetBufferSubData : glGetBufferSubDataARB) : 0;
  glGetCompressedTexImage_fn   = has_arb_texture_compression ? (glGetCompressedTexImage ? glGetCompressedTexImage : glGetCompressedTexImageARB) : 0;
  glGetUniformLocation_fn      = has_arb_shader_objects ? (glGetUniformLocation ? glGetUniformLocation : (PFNGLGETUNIFORMLOCATIONPROC)glGetUniformLocationARB) : 0;

  if (has_ext_framebuffer_object)
  {
    glBindFramebuffer_fn                     = glBindFramebufferEXT;
    glIsRenderbuffer_fn                      = glIsRenderbufferEXT;
    glBindRenderbuffer_fn                    = glBindRenderbufferEXT;
    glDeleteRenderbuffers_fn                 = glDeleteRenderbuffersEXT;
    glGenRenderbuffers_fn                    = glGenRenderbuffersEXT;
    glRenderbufferStorage_fn                 = glRenderbufferStorageEXT;
    glGetRenderbufferParameteriv_fn          = glGetRenderbufferParameterivEXT;
    glIsFramebuffer_fn                       = glIsFramebufferEXT;
    glDeleteFramebuffers_fn                  = glDeleteFramebuffersEXT;
    glGenFramebuffers_fn                     = glGenFramebuffersEXT;
    glCheckFramebufferStatus_fn              = glCheckFramebufferStatusEXT;
    glFramebufferRenderbuffer_fn             = glFramebufferRenderbufferEXT;
    glFramebufferTexture1D_fn                = glFramebufferTexture1DEXT;
    glFramebufferTexture2D_fn                = glFramebufferTexture2DEXT;
    glFramebufferTexture3D_fn                = glFramebufferTexture3DEXT;
    glGetFramebufferAttachmentParameteriv_fn = glGetFramebufferAttachmentParameterivEXT;
    glGenerateMipmap_fn                      = glGenerateMipmapEXT;
  }
  
  if (has_arb_transpose_matrix)
  {
    glLoadTransposeMatrixd_fn = glLoadTransposeMatrixd ? glLoadTransposeMatrixd : (PFNGLLOADTRANSPOSEMATRIXDPROC)glLoadTransposeMatrixdARB;
    glLoadTransposeMatrixf_fn = glLoadTransposeMatrixf ? glLoadTransposeMatrixf : (PFNGLLOADTRANSPOSEMATRIXFPROC)glLoadTransposeMatrixfARB;
    glMultTransposeMatrixd_fn = glMultTransposeMatrixd ? glMultTransposeMatrixd : (PFNGLMULTTRANSPOSEMATRIXDPROC)glMultTransposeMatrixdARB;
    glMultTransposeMatrixf_fn = glMultTransposeMatrixf ? glMultTransposeMatrixf : (PFNGLMULTTRANSPOSEMATRIXFPROC)glMultTransposeMatrixfARB;
  }

  if (has_arb_shader_objects)
  {
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
    glUseProgram_fn              = glUseProgram ? glUseProgram : (PFNGLUSEPROGRAMPROC)glUseProgramObjectARB;
  }

  if (has_ffp)
  {
    if (has_arb_multitexture) glGetIntegerv (GL_MAX_TEXTURE_UNITS, (GLint*)&texture_units_count);
    else                      texture_units_count = 1;
  }
  else
  {
    if (has_arb_vertex_shader)
    {
      int vertex_shader_texture_units_count, pixel_shader_texture_units_count;

      glGetIntegerv (GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB, (GLint*)&vertex_shader_texture_units_count);
      glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS_ARB, (GLint*)&pixel_shader_texture_units_count);

      texture_units_count = stl::min (vertex_shader_texture_units_count, pixel_shader_texture_units_count);
    }
    else
      texture_units_count = 1;
  }
  
#else
  ExtensionSet ext = available_extension_set;

  static Extension OES_packed_depth_stencil       = "GL_OES_packed_depth_stencil",
                   EXT_texture_compression_s3tc   = "GL_EXT_texture_compression_s3tc",
//                   EXT_texture_filter_anisotropic = "GL_EXT_texture_filter_anisotropic",
                   IMG_texture_compression_pvrtc  = "GL_IMG_texture_compression_pvrtc",
                   AMD_compressed_ATC_texture     = "GL_AMD_compressed_ATC_texture",
                   ATI_texture_compression_atitc  = "GL_ATI_texture_compression_atitc";

  has_arb_multitexture               = true;
  has_arb_vertex_buffer_object       = true;
  has_ext_packed_depth_stencil       = ext.Get (OES_packed_depth_stencil);
  has_ext_texture_compression_s3tc   = ext.Get (EXT_texture_compression_s3tc);
  has_ext_texture_filter_anisotropic = false; //ext.Get (EXT_texture_filter_anisotropic);   //supported on Qualcomm Adreno 205 but generates error
  has_img_texture_compression_pvrtc  = ext.Get (IMG_texture_compression_pvrtc);
  has_amd_compressed_atc_texture     = ext.Get (AMD_compressed_ATC_texture) || ext.Get (ATI_texture_compression_atitc);

#ifdef OPENGL_ES_SUPPORT
  static Extension OES_blend_equation_separate = "GL_OES_blend_equation_separate",
                   OES_blend_func_separate     = "GL_OES_blend_func_separate",
                   OES_blend_subtract          = "GL_OES_blend_subtract",
                   OES_framebuffer_object      = "GL_OES_framebuffer_object",
                   OES_texture_cube_map        = "GL_OES_texture_cube_map";

  has_arb_texture_cube_map           = ext.Get (OES_texture_cube_map);
  has_ext_blend_equation_separate    = ext.Get (OES_blend_equation_separate);
  has_ext_blend_func_separate        = ext.Get (OES_blend_func_separate);
  has_ext_blend_subtract             = ext.Get (OES_blend_subtract);
  has_ext_framebuffer_object         = ext.Get (OES_framebuffer_object);
  has_sgis_generate_mipmap           = true;
  has_arb_multisample                = true;
#elif defined(OPENGL_ES2_SUPPORT)
  has_arb_shading_language_100       = true;
  has_arb_fragment_shader            = true;
  has_arb_vertex_shader              = true;
  has_arb_texture_cube_map           = true;
  has_ext_framebuffer_object         = true;
  has_arb_multisample                = false;
  has_sgis_generate_mipmap           = false;
  has_ext_blend_equation_separate    = true;
  has_ext_blend_func_separate        = true;
  has_ext_blend_subtract             = true;
  has_ati_separate_stencil           = true;
  has_arb_texture_border_clamp       = ext.Get ("GL_NV_texture_border_clamp");
  glUniform1fv_fn                    = glUniform1fv;
  glUniform1iv_fn                    = glUniform1iv;
  glUniform2fv_fn                    = glUniform2fv;
  glUniform2iv_fn                    = glUniform2iv;
  glUniform3fv_fn                    = glUniform3fv;
  glUniform3iv_fn                    = glUniform3iv;
  glUniform4fv_fn                    = glUniform4fv;
  glUniform4iv_fn                    = glUniform4iv;
  glUniformMatrix2fv_fn              = glUniformMatrix2fv;
  glUniformMatrix3fv_fn              = glUniformMatrix3fv;
  glUniformMatrix4fv_fn              = glUniformMatrix4fv;
  glUseProgram_fn                    = glUseProgram;
#endif
  glActiveTexture_fn           = glActiveTexture;
  glBindBuffer_fn              = glBindBuffer;
  glBlendEquation_fn           = has_ext_blend_subtract ? glBlendEquationOES : 0;
  glBlendEquationSeparate_fn   = has_ext_blend_equation_separate ? glBlendEquationSeparateOES : 0;
  glBlendFuncSeparate_fn       = has_ext_blend_func_separate ? glBlendFuncSeparateOES : 0;
  glBufferData_fn              = glBufferData;
  glBufferSubData_fn           = glBufferSubData;
  glCompressedTexImage2D_fn    = glCompressedTexImage2D;
  glCompressedTexSubImage2D_fn = glCompressedTexSubImage2D;
  glDeleteBuffers_fn           = glDeleteBuffers;
  glGenBuffers_fn              = glGenBuffers;
#ifndef OPENGL_ES2_SUPPORT
  glClientActiveTexture_fn = glClientActiveTexture;
#endif

  if (has_ext_framebuffer_object)
  {
    glBindFramebuffer_fn                     = glBindFramebufferOES;
    glIsRenderbuffer_fn                      = glIsRenderbufferOES;
    glBindRenderbuffer_fn                    = glBindRenderbufferOES;
    glDeleteRenderbuffers_fn                 = glDeleteRenderbuffersOES;
    glGenRenderbuffers_fn                    = glGenRenderbuffersOES;
    glRenderbufferStorage_fn                 = glRenderbufferStorageOES;
    glGetRenderbufferParameteriv_fn          = glGetRenderbufferParameterivOES;
    glIsFramebuffer_fn                       = glIsFramebufferOES;
    glDeleteFramebuffers_fn                  = glDeleteFramebuffersOES;
    glGenFramebuffers_fn                     = glGenFramebuffersOES;
    glCheckFramebufferStatus_fn              = glCheckFramebufferStatusOES;
    glFramebufferRenderbuffer_fn             = glFramebufferRenderbufferOES;
    glFramebufferTexture2D_fn                = glFramebufferTexture2DOES;
    glGetFramebufferAttachmentParameteriv_fn = glGetFramebufferAttachmentParameterivOES;
    glGenerateMipmap_fn                      = glGenerateMipmapOES;
  }

#ifdef OPENGL_ES_SUPPORT
  glGetIntegerv (GL_MAX_TEXTURE_UNITS, (GLint*)&texture_units_count);  //has_arb_multitexture always true
#elif defined(OPENGL_ES2_SUPPORT)
  glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS, (GLint*)&texture_units_count);  //has_arb_multitexture always true
#endif

#endif

#ifdef OPENGL_ES2_SUPPORT
  has_ffp = false;
#else
  has_ffp = settings.IsFfpAllowed ();
#endif

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, (GLint*)&max_texture_size);

  if (has_ext_texture_filter_anisotropic) glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint*)&max_anisotropy);
  else                                    max_anisotropy = 1;

  if (has_ffp)
  {
#ifndef OPENGL_ES2_SUPPORT
    if (has_arb_multitexture) glGetIntegerv (GL_MAX_TEXTURE_UNITS, (GLint*)&texture_units_count);
    else                      texture_units_count = 1;
#endif
  }
#ifndef OPENGL_ES_SUPPORT  
  else
  {
    if (has_arb_vertex_shader)
    {
      int vertex_shader_texture_units_count = 0, pixel_shader_texture_units_count = 0;

      glGetIntegerv (GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS_ARB, (GLint*)&vertex_shader_texture_units_count);
      glGetIntegerv (GL_MAX_TEXTURE_IMAGE_UNITS_ARB, (GLint*)&pixel_shader_texture_units_count);

      texture_units_count = stl::min (vertex_shader_texture_units_count, pixel_shader_texture_units_count);
    }
    else
      texture_units_count = 1;
  }

  if (has_arb_vertex_shader)
  {
#ifdef OPENGL_ES2_SUPPORT
    glVertexAttribPointer_fn       = glVertexAttribPointer;
    glDisableVertexAttribArray_fn  = glDisableVertexAttribArray;
    glEnableVertexAttribArray_fn   = glEnableVertexAttribArray;
#else
    glVertexAttribPointer_fn       = glVertexAttribPointer ? glVertexAttribPointer : glVertexAttribPointerARB;
    glDisableVertexAttribArray_fn  = glDisableVertexAttribArray ? glDisableVertexAttribArray : glDisableVertexAttribArrayARB;
    glEnableVertexAttribArray_fn   = glEnableVertexAttribArray ? glEnableVertexAttribArray : glEnableVertexAttribArrayARB;

    if (ext.Get ("GL_NV_vertex_program4"))
      glVertexAttribIPointer_fn = glVertexAttribIPointer ? glVertexAttribIPointer : glVertexAttribIPointerEXT;

    if (ext.Get ("GL_ARB_vertex_attrib_64bit") || ext.Get ("GL_EXT_vertex_attrib_64bit"))
      glVertexAttribLPointer_fn  = glVertexAttribLPointer ? glVertexAttribLPointer : glVertexAttribLPointerEXT;
#endif
  }
#endif

  size_t settings_max_texture_size    = settings.GetInteger (ContextSettingsInteger_MaxTextureSize),
         settings_max_anisotropy      = settings.GetInteger (ContextSettingsInteger_MaxAnisotropy),
         settings_texture_units_count = settings.GetInteger (ContextSettingsInteger_TextureUnitsCount);

  if (settings_max_texture_size)
  {
    if (max_3d_texture_size > settings_max_texture_size)        max_3d_texture_size        = settings_max_texture_size;
    if (max_cube_map_texture_size > settings_max_texture_size)  max_cube_map_texture_size  = settings_max_texture_size;
    if (max_rectangle_texture_size > settings_max_texture_size) max_rectangle_texture_size = settings_max_texture_size;    
    if (max_texture_size > settings_max_texture_size)           max_texture_size           = settings_max_texture_size;
  }

  if (settings_max_anisotropy && max_anisotropy > settings_max_anisotropy)                max_anisotropy      = settings_max_anisotropy;
  if (settings_texture_units_count && texture_units_count > settings_texture_units_count) texture_units_count = settings_texture_units_count;
}

/*
    Преобразование в формат возможностей устройства
*/

void ContextCaps::GetDeviceCaps (render::low_level::DeviceCaps& caps) const
{
  memset (&caps, 0, sizeof (DeviceCaps));
  
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)
  caps.has_texture1d = true;
#else
  caps.has_texture1d = false;
#endif

  caps.max_texture_2d_size            = max_rectangle_texture_size > max_texture_size ? max_rectangle_texture_size : max_texture_size;
  caps.max_texture_1d_size            = caps.has_texture1d ? caps.max_texture_2d_size : 0;
  caps.max_texture_3d_size            = max_3d_texture_size;
  caps.max_texture_cubemap_size       = max_cube_map_texture_size;
  caps.max_texture_anisotropy         = max_anisotropy;
  caps.samplers_count                 = texture_units_count;
  caps.has_advanced_blend             = has_ext_blend_equation_separate && has_ext_blend_func_separate && has_ext_blend_minmax && has_ext_blend_subtract;
  caps.has_depth_texture              = has_arb_depth_texture;
  caps.has_multisample                = has_arb_multisample;
  caps.has_occlusion_queries          = has_arb_occlusion_query;
  caps.has_shadow_maps                = has_arb_shadow && has_arb_depth_texture && has_ext_shadow_funcs;
  caps.has_texture_anisotropic_filter = has_ext_texture_filter_anisotropic;
  caps.has_texture_cube_map           = has_arb_texture_cube_map;
  caps.has_texture_lod_bias           = has_ext_texture_lod_bias || has_sgis_texture_lod;
  caps.has_texture_mirrored_repeat    = has_arb_texture_mirrored_repeat;
  caps.has_texture_non_power_of_two   = has_arb_texture_non_power_of_two || has_arb_texture_rectangle;
  caps.has_texture3d                  = has_ext_texture3d;
  caps.has_two_side_stencil           = has_ati_separate_stencil || has_ext_stencil_two_side;
  caps.has_right_hand_viewport        = true;
}
