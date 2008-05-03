#include <shared/context_caps.h>

using namespace render::low_level::opengl;

ContextCaps::ContextCaps ()
{
  memset (this, 0, sizeof *this);
}

void ContextCaps::Init (const ExtensionSet& extension_set)
{
  static Extension ARB_depth_texture              = "GL_ARB_depth_texture",
                   ARB_fragment_shader            = "GL_ARB_fragment_shader",
                   ARB_multisample                = "GL_ARB_multisample",
                   ARB_multitexture               = "GL_ARB_multitexture",
                   ARB_occlusion_query            = "GL_ARB_occlusion_query",
                   ARB_shading_language_100       = "GL_ARB_shading_language_100",
                   ARB_texture_border_clamp       = "GL_ARB_texture_border_clamp",
                   ARB_texture_compression        = "GL_ARB_texture_compression",
                   ARB_texture_cube_map           = "GL_ARB_texture_cube_map",
                   ARB_texture_mirrored_repeat    = "GL_ARB_texture_mirrored_repeat",
                   ARB_texture_non_power_of_two   = "GL_ARB_texture_non_power_of_two",
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
                   EXT_texture_rectangle          = "GL_EXT_texture_rectangle",
                   EXT_texture3D                  = "GL_EXT_texture3D",
                   NV_texture_rectangle           = "GL_NV_texture_rectangle",
                   SGIS_generate_mipmap           = "GL_SGIS_generate_mipmap",
                   SGIS_texture_lod               = "GL_SGIS_texture_lod",
                   Version_1_2                    = "GL_VERSION_1_2",
                   Version_1_3                    = "GL_VERSION_1_3",
                   Version_1_4                    = "GL_VERSION_1_4",
                   Version_1_5                    = "GL_VERSION_1_5",
                   Version_2_0                    = "GL_VERSION_2_0";

  has_arb_depth_texture              = extension_set.Get (ARB_depth_texture) || extension_set.Get (Version_1_4);  
  has_arb_fragment_shader            = extension_set.Get (ARB_fragment_shader) || extension_set.Get (Version_2_0);
  has_arb_multisample                = extension_set.Get (ARB_multisample) || extension_set.Get (Version_1_3);
  has_arb_multitexture               = extension_set.Get (ARB_multitexture) || extension_set.Get (Version_1_3);
  has_arb_occlusion_query            = extension_set.Get (ARB_occlusion_query) || extension_set.Get (Version_1_5);
  has_arb_shading_language_100       = extension_set.Get (ARB_shading_language_100) || extension_set.Get (Version_2_0);
  has_arb_texture_border_clamp       = extension_set.Get (ARB_texture_border_clamp) || extension_set.Get (Version_1_3);
  has_arb_texture_cube_map           = extension_set.Get (ARB_texture_cube_map) || extension_set.Get (EXT_texture_cube_map) ||
                                       extension_set.Get (Version_1_3);
  has_arb_texture_mirrored_repeat    = extension_set.Get (ARB_texture_mirrored_repeat) || extension_set.Get (Version_1_4);
  has_arb_texture_non_power_of_two   = extension_set.Get (ARB_texture_non_power_of_two) || extension_set.Get (Version_2_0);
  has_arb_transpose_matrix           = extension_set.Get (ARB_transpose_matrix) || extension_set.Get (Version_1_3);
  has_arb_vertex_buffer_object       = extension_set.Get (ARB_vertex_buffer_object) || extension_set.Get (Version_1_5);
  has_arb_vertex_shader              = extension_set.Get (ARB_vertex_shader) || extension_set.Get (Version_2_0);
  has_arb_window_pos                 = extension_set.Get (ARB_window_pos) || extension_set.Get (Version_1_4);
  has_ati_separate_stencil           = extension_set.Get (ATI_separate_stencil) || extension_set.Get (Version_2_0);
  has_ext_blend_equation_separate    = extension_set.Get (EXT_blend_equation_separate) || extension_set.Get (Version_2_0);
  has_ext_blend_func_separate        = extension_set.Get (EXT_blend_func_separate) || extension_set.Get (Version_1_4);
  has_ext_blend_minmax               = extension_set.Get (EXT_blend_minmax) || extension_set.Get (Version_1_2);
  has_ext_blend_subtract             = extension_set.Get (EXT_blend_subtract) || extension_set.Get (Version_1_2);
  has_ext_framebuffer_object         = extension_set.Get (EXT_framebuffer_object);
  has_ext_packed_depth_stencil       = extension_set.Get (EXT_packed_depth_stencil);
  has_ext_shadow_funcs               = extension_set.Get (EXT_shadow_funcs) || extension_set.Get (Version_1_5);
  has_ext_stencil_two_side           = extension_set.Get (EXT_stencil_two_side);
  has_ext_texture_compression_s3tc   = (extension_set.Get (ARB_texture_compression) || extension_set.Get (Version_1_3)) && 
                                       extension_set.Get (EXT_texture_compression_s3tc);
  has_ext_texture_filter_anisotropic = extension_set.Get (EXT_texture_filter_anisotropic);
  has_ext_texture_lod_bias           = extension_set.Get (EXT_texture_lod_bias) || extension_set.Get (Version_1_4);
  has_ext_texture_rectangle          = extension_set.Get (EXT_texture_rectangle) || extension_set.Get (NV_texture_rectangle);
  has_ext_texture3d                  = extension_set.Get (EXT_texture3D) || extension_set.Get (Version_1_2);
  has_sgis_generate_mipmap           = extension_set.Get (SGIS_generate_mipmap) || extension_set.Get (Version_1_4);  
  has_sgis_texture_lod               = extension_set.Get (SGIS_texture_lod) || extension_set.Get (Version_1_2);

  glGetIntegerv (GL_MAX_TEXTURE_SIZE, (GLint*)&max_texture_size);

  if (has_ext_texture_rectangle)          glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, (GLint*)&max_rectangle_texture_size);
  if (has_arb_texture_cube_map)           glGetIntegerv (GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB, (GLint*)&max_cube_map_texture_size);
  if (has_ext_texture3d)                  glGetIntegerv (GL_MAX_3D_TEXTURE_SIZE_EXT, (GLint*)&max_3d_texture_size);
  if (has_arb_multitexture)               glGetIntegerv (GL_MAX_TEXTURE_UNITS, (GLint*)&texture_units_count);
  if (has_ext_texture_filter_anisotropic) glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint*)&max_anisotropy);

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
