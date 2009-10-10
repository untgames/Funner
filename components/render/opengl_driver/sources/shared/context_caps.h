#ifndef RENDER_GL_DRIVER_CONTEXT_CAPS_HEADER
#define RENDER_GL_DRIVER_CONTEXT_CAPS_HEADER

#include <shared/gl.h>
#include <shared/extensions.h>

namespace render
{

namespace low_level
{

//forward declaration
struct DeviceCaps;

namespace opengl
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///јппаратно поддерживаемые возможности контекста
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ContextCaps
{
///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕоддерживаемые расширени€
///////////////////////////////////////////////////////////////////////////////////////////////////
  bool has_arb_depth_texture;              //GL_ARB_depth_texture
  bool has_arb_fragment_shader;            //GL_ARB_fragment_shader
  bool has_arb_multisample;                //GL_ARB_multisample
  bool has_arb_multitexture;               //GL_ARB_multitexture
  bool has_arb_occlusion_query;            //GL_ARB_occlusion_query
  bool has_arb_shading_language_100;       //GL_ARB_shading_language_100
  bool has_arb_shadow;                     //GL_ARB_shadow
  bool has_arb_texture_border_clamp;       //GL_ARB_texture_border_clamp
  bool has_arb_texture_cube_map;           //GL_ARB_texture_cube_map
  bool has_arb_texture_mirrored_repeat;    //GL_ARB_texture_mirrored_repeat
  bool has_arb_texture_non_power_of_two;   //GL_ARB_texture_non_power_of_two
  bool has_arb_texture_rectangle;          //GL_ARB_texture_rectangle
  bool has_arb_transpose_matrix;           //GL_ARB_transpose_matrix
  bool has_arb_vertex_buffer_object;       //GL_ARB_vertex_buffer_object
  bool has_arb_vertex_shader;              //GL_ARB_vertex_shader
  bool has_arb_window_pos;                 //GL_ARB_window_pos
  bool has_ati_separate_stencil;           //GL_ATI_separate_stencil
  bool has_ext_blend_equation_separate;    //GL_EXT_blend_equation_separate
  bool has_ext_blend_func_separate;        //GL_EXT_blend_func_separate
  bool has_ext_blend_minmax;               //GL_EXT_blend_minmax
  bool has_ext_blend_subtract;             //GL_EXT_blend_subtract
  bool has_ext_framebuffer_object;         //GL_EXT_framebuffer_object
  bool has_ext_packed_depth_stencil;       //GL_EXT_packed_depth_stencil
  bool has_ext_shadow_funcs;               //GL_EXT_shadow_funcs
  bool has_ext_stencil_two_side;           //GL_EXT_stencil_two_side
  bool has_ext_texture_compression_s3tc;   //GL_EXT_texture_compression_s3tc
  bool has_ext_texture_filter_anisotropic; //GL_EXT_texture_filter_anisotropic
  bool has_ext_texture_lod_bias;           //GL_EXT_texture_lod_bias
  bool has_ext_texture3d;                  //GL_EXT_texture3D
  bool has_sgis_generate_mipmap;           //GL_SGIS_generate_mipmap
  bool has_sgis_texture_lod;               //GL_SGIS_texture_lod
  
///////////////////////////////////////////////////////////////////////////////////////////////////
///ћаксимальные возможности аппаратуры
///////////////////////////////////////////////////////////////////////////////////////////////////
  size_t max_3d_texture_size;        //максимальный размер 3d текстуры
  size_t max_anisotropy;             //максимально возможна€ степень анизотропии
  size_t max_cube_map_texture_size;  //максимальный размер cubemap текстуры
  size_t max_rectangle_texture_size; //максимальный размер текстуры со сторонами не степени 2 дл€ устройства
  size_t max_texture_size;           //максимальный размер текстуры дл€ устройства
  size_t texture_units_count;        //количество слотов мультитекстурировани€

///////////////////////////////////////////////////////////////////////////////////////////////////
///‘ункции
///////////////////////////////////////////////////////////////////////////////////////////////////
  PFNGLACTIVETEXTUREPROC                       glActiveTexture_fn;
  PFNGLBINDBUFFERPROC                          glBindBuffer_fn;
  PFNGLBLENDEQUATIONPROC                       glBlendEquation_fn;
  PFNGLBLENDEQUATIONSEPARATEPROC               glBlendEquationSeparate_fn;
  PFNGLBLENDFUNCSEPARATEPROC                   glBlendFuncSeparate_fn;
  PFNGLBUFFERDATAPROC                          glBufferData_fn;
  PFNGLBUFFERSUBDATAPROC                       glBufferSubData_fn;
  PFNGLCLIENTACTIVETEXTUREPROC                 glClientActiveTexture_fn;
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC             glCompressedTexSubImage2D_fn;
  PFNGLDELETEBUFFERSPROC                       glDeleteBuffers_fn;
  PFNGLGENBUFFERSPROC                          glGenBuffers_fn;

  PFNGLISRENDERBUFFERPROC                      glIsRenderbuffer_fn;
  PFNGLBINDRENDERBUFFERPROC                    glBindRenderbuffer_fn;
  PFNGLDELETERENDERBUFFERSPROC                 glDeleteRenderbuffers_fn;
  PFNGLGENRENDERBUFFERSPROC                    glGenRenderbuffers_fn;
  PFNGLRENDERBUFFERSTORAGEPROC                 glRenderbufferStorage_fn;
  PFNGLGETRENDERBUFFERPARAMETERIVPROC          glGetRenderbufferParameteriv_fn;
  PFNGLISFRAMEBUFFERPROC                       glIsFramebuffer_fn;
  PFNGLBINDFRAMEBUFFERPROC                     glBindFramebuffer_fn;
  PFNGLDELETEFRAMEBUFFERSPROC                  glDeleteFramebuffers_fn;
  PFNGLGENFRAMEBUFFERSPROC                     glGenFramebuffers_fn;
  PFNGLCHECKFRAMEBUFFERSTATUSPROC              glCheckFramebufferStatus_fn;
  PFNGLFRAMEBUFFERRENDERBUFFERPROC             glFramebufferRenderbuffer_fn;
  PFNGLFRAMEBUFFERTEXTURE2DPROC                glFramebufferTexture2D_fn;
  PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC glGetFramebufferAttachmentParameteriv_fn;

#ifndef OPENGL_ES_SUPPORT
  PFNGLGETBUFFERSUBDATAPROC        glGetBufferSubData_fn;
  PFNGLGETCOMPRESSEDTEXIMAGEPROC   glGetCompressedTexImage_fn;
  PFNGLGETUNIFORMLOCATIONPROC      glGetUniformLocation_fn;
  PFNGLLOADTRANSPOSEMATRIXDPROC    glLoadTransposeMatrixd_fn;    
  PFNGLLOADTRANSPOSEMATRIXFPROC    glLoadTransposeMatrixf_fn;
  PFNGLMULTTRANSPOSEMATRIXDPROC    glMultTransposeMatrixd_fn;
  PFNGLMULTTRANSPOSEMATRIXFPROC    glMultTransposeMatrixf_fn;  
  PFNGLUNIFORM1FVPROC              glUniform1fv_fn;
  PFNGLUNIFORM1IVPROC              glUniform1iv_fn;
  PFNGLUNIFORM2FVPROC              glUniform2fv_fn;
  PFNGLUNIFORM2IVPROC              glUniform2iv_fn;
  PFNGLUNIFORM3FVPROC              glUniform3fv_fn;
  PFNGLUNIFORM3IVPROC              glUniform3iv_fn;
  PFNGLUNIFORM4FVPROC              glUniform4fv_fn;
  PFNGLUNIFORM4IVPROC              glUniform4iv_fn;
  PFNGLUNIFORMMATRIX2FVPROC        glUniformMatrix2fv_fn;
  PFNGLUNIFORMMATRIX3FVPROC        glUniformMatrix3fv_fn;
  PFNGLUNIFORMMATRIX4FVPROC        glUniformMatrix4fv_fn;
  PFNGLUSEPROGRAMPROC              glUseProgram_fn;

  PFNGLFRAMEBUFFERTEXTURE1DPROC    glFramebufferTexture1D_fn;
  PFNGLFRAMEBUFFERTEXTURE3DPROC    glFramebufferTexture3D_fn;
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////
/// онструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
  ContextCaps ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///»нициализаци€
///////////////////////////////////////////////////////////////////////////////////////////////////
  void Init (const ExtensionSet& available_extension_set, const ExtensionSet& enabled_extension_set);

///////////////////////////////////////////////////////////////////////////////////////////////////
///ѕреобразование в формат возможностей устройства
///////////////////////////////////////////////////////////////////////////////////////////////////
  void GetDeviceCaps (DeviceCaps&) const;
};

}

}

}

#endif
