#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace 
{

struct SamplerExtensions
{
  bool has_sgis_texture_lod;               //GL_SGIS_texture_lod
  bool has_ext_texture_lod_bias;           //GL_EXT_texture_lod_bias
  bool has_ext_texture_filter_anisotropic; //GL_EXT_texture_filter_anisotropic  
  bool has_ext_shadow_funcs;               //GL_EXT_shadow_funcs
  bool has_arb_texture_border_clamp;       //GL_ARB_texture_border_clamp
  bool has_arb_texture_mirrored_repeat;    //GL_ARB_texture_mirrored_repeat

  SamplerExtensions (const ContextManager& manager)
  {
    static Extension SGIS_texture_lod               = "GL_SGIS_texture_lod",
                     EXT_texture_lod_bias           = "GL_EXT_texture_lod_bias",
                     EXT_texture_filter_anisotropic = "GL_EXT_texture_filter_anisotropic",
                     EXT_shadow_funcs               = "GL_EXT_shadow_funcs",
                     ARB_texture_border_clamp       = "GL_ARB_texture_border_clamp",
                     ARB_texture_mirrored_repeat    = "GL_ARB_texture_mirrored_repeat",
                     Version_1_2                    = "GL_VERSION_1_2",
                     Version_1_3                    = "GL_VERSION_1_3",
                     Version_1_4                    = "GL_VERSION_1_4",
                     Version_1_5                    = "GL_VERSION_1_5";
                     
    manager.MakeContextCurrent ();
      
    has_sgis_texture_lod               = manager.IsSupported (SGIS_texture_lod) || manager.IsSupported (Version_1_2);
    has_ext_texture_lod_bias           = manager.IsSupported (EXT_texture_lod_bias) || manager.IsSupported (Version_1_4);
    has_ext_texture_filter_anisotropic = manager.IsSupported (EXT_texture_filter_anisotropic);
    has_ext_shadow_funcs               = manager.IsSupported (EXT_shadow_funcs) || manager.IsSupported (Version_1_5);
    has_arb_texture_border_clamp       = manager.IsSupported (ARB_texture_border_clamp) || manager.IsSupported (Version_1_3);
    has_arb_texture_mirrored_repeat    = manager.IsSupported (ARB_texture_mirrored_repeat) || manager.IsSupported (Version_1_4);
  }
};

GLenum get_gl_texture_target (OpenGLTextureTarget tex_target)
{
  switch (tex_target)
  {
    case OpenGLTextureTarget_Texture1D:        return GL_TEXTURE_1D;
    case OpenGLTextureTarget_Texture2D:        return GL_TEXTURE_2D;
    case OpenGLTextureTarget_TextureRectangle: return GL_TEXTURE_RECTANGLE_EXT;
    case OpenGLTextureTarget_Texture3D:        return GL_TEXTURE_3D;
    case OpenGLTextureTarget_TextureCubemap:   return GL_TEXTURE_CUBE_MAP_ARB;
    default: RaiseInvalidArgument ("get_gl_texture_target", "Unknown texture dimension"); return 0;
  }
}

GLenum get_gl_min_filter (TexMinFilter filter)
{
  switch (filter)
  {
    case TexMinFilter_Point:           return GL_NEAREST;
    case TexMinFilter_Linear:          return GL_LINEAR;
    case TexMinFilter_PointMipPoint:   return GL_NEAREST_MIPMAP_LINEAR;
    case TexMinFilter_LinearMipPoint:  return GL_NEAREST_MIPMAP_LINEAR;
    case TexMinFilter_PointMipLinear:  return GL_NEAREST_MIPMAP_LINEAR;
    case TexMinFilter_LinearMipLinear: return GL_LINEAR_MIPMAP_LINEAR;
    default:                           return GL_NEAREST_MIPMAP_LINEAR;
  }
}

GLenum get_gl_mag_filter (TexMinFilter filter)
{
  switch (filter)
  {
    case TexMagFilter_Point:  return GL_NEAREST;
    case TexMagFilter_Linear: return GL_LINEAR;
    default:                  return GL_LINEAR;
  }
}

GLenum get_gl_texcoord_wrap (TexcoordWrap wrap, const SamplerExtensions& ext, const char* METHOD_NAME)
{
  switch (wrap)
  {
    case TexcoordWrap_Mirror:        
      if (ext.has_arb_texture_mirrored_repeat)
        return GL_MIRRORED_REPEAT; 
      else
        RaiseNotSupported (METHOD_NAME, "Can't set wrap mode to %s (GL_ARB_texture_mirrored_repeat extension not supported)", get_name (wrap));
    case TexcoordWrap_ClampToBorder: 
      if (ext.has_arb_texture_border_clamp)
        return GL_CLAMP_TO_BORDER;
      else
        RaiseNotSupported (METHOD_NAME, "Can't set wrap mode to %s (GL_ARB_texture_border_clamp extension not supported)", get_name (wrap));
    case TexcoordWrap_Clamp:         return GL_CLAMP;
    case TexcoordWrap_Repeat:        
    default:                         return GL_REPEAT;
  }
}

GLenum get_gl_compare_function (CompareMode mode)
{
  switch (mode)
  {
    case CompareMode_AlwaysFail:   return GL_NEVER;
    case CompareMode_AlwaysPass:   return GL_ALWAYS;
    case CompareMode_Equal:        return GL_EQUAL;
    case CompareMode_NotEqual:     return GL_NOTEQUAL;
    case CompareMode_Less:         return GL_LESS;
    case CompareMode_LessEqual:    return GL_LEQUAL;
    case CompareMode_Greater:      return GL_GREATER;
    case CompareMode_GreaterEqual: return GL_GEQUAL;
    default: RaiseInvalidArgument ("get_gl_compare_function", "Unknown comparision function"); return 0;
  }
}

}

/*
   Конструктор
*/

SamplerState::SamplerState (const ContextManager& manager, const SamplerDesc& in_desc) 
  : ContextObject (manager), max_anisotropy (1)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SamplerState::SamplerState";
  bool has_ext_texture_filter_anisotropic (manager.IsSupported ("GL_EXT_texture_filter_anisotropic"));

  MakeContextCurrent ();

  display_list = glGenLists (OpenGLTextureTarget_Num);
  
  if (!display_list)
    Raise <Exception> (METHOD_NAME, "Can't generate display lists");

  if (has_ext_texture_filter_anisotropic)
    glGetIntegerv (GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, (GLint*)&max_anisotropy);

  CheckErrors (METHOD_NAME);

  SetDesc (in_desc);
}

/*
   Выбор сэмплера в контекст OpenGL
*/

void SamplerState::Bind (OpenGLTextureTarget tex_target)
{
  MakeContextCurrent ();

  glCallList (display_list + tex_target);

  CheckErrors ("render::low_level::opengl::SamplerState::Bind");
}

/*
   Изменение/получение дескриптора
*/

void SamplerState::SetDesc (const SamplerDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SamplerState::SetDesc";
  SamplerExtensions  ext (GetContextManager ());

  if (ext.has_ext_texture_filter_anisotropic)
    if (desc.max_anisotropy > max_anisotropy)
      RaiseNotSupported (METHOD_NAME, "Can't set anisotropy level %u (max anisotropy level %u)", desc.max_anisotropy, max_anisotropy);

  if (!ext.has_ext_shadow_funcs && desc.comparision_function != CompareMode_AlwaysPass)
    RaiseNotSupported (METHOD_NAME, "Can't set comparision function %s (GL_EXT_shadow_funcs extension not supported)", get_name (desc.comparision_function));

  MakeContextCurrent ();

  GLenum min_filter = get_gl_min_filter (desc.min_filter), mag_filter = get_gl_mag_filter (desc.min_filter);
  GLenum s_wrap = get_gl_texcoord_wrap (desc.wrap_u, ext, METHOD_NAME), t_wrap = get_gl_texcoord_wrap (desc.wrap_v, ext, METHOD_NAME),
         r_wrap = get_gl_texcoord_wrap (desc.wrap_w, ext, METHOD_NAME);
  GLenum compare_function = get_gl_compare_function (desc.comparision_function);

    //запись команд в контексте OpenGL

  for (size_t i = 0; i < OpenGLTextureTarget_Num; i++)
  {
    GLenum tex_target = get_gl_texture_target ((OpenGLTextureTarget)i);

    glNewList (display_list + i, GL_COMPILE);

    glTexParameteri (tex_target, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri (tex_target, GL_TEXTURE_MAG_FILTER, mag_filter);

    if (!ext.has_ext_texture_filter_anisotropic)
      glTexParameteri (tex_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, desc.max_anisotropy);                 

    glTexParameteri (tex_target, GL_TEXTURE_WRAP_S, s_wrap);
    glTexParameteri (tex_target, GL_TEXTURE_WRAP_T, t_wrap);
    glTexParameteri (tex_target, GL_TEXTURE_WRAP_R, r_wrap);

    if (ext.has_sgis_texture_lod)
    {
      glTexParameterf (tex_target, GL_TEXTURE_MIN_LOD, desc.min_lod);
      glTexParameterf (tex_target, GL_TEXTURE_MAX_LOD, desc.max_lod);
    }

    if (ext.has_ext_texture_lod_bias)
      glTexEnvf (GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, desc.mip_lod_bias);

    if (ext.has_ext_shadow_funcs)
    {
      glTexParameteri (tex_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
      glTexParameteri (tex_target, GL_TEXTURE_COMPARE_FUNC, compare_function);
    }

    glTexParameterfv (tex_target, GL_TEXTURE_BORDER_COLOR, desc.border_color);
    
    glEndList ();  
  }

  CheckErrors (METHOD_NAME);

  desc = in_desc;
}
