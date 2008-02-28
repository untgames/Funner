#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace 
{

/*
    ���������� ���� �������
*/

enum TexTargetId
{
  TexTargetId_Texture1D,
  TexTargetId_Texture2D,
  TexTargetId_TextureRectangle,
  TexTargetId_Texture3D,
  TexTargetId_TextureCubemap,

  TexTargetId_Num
};

/*
    ������������� ���� �������� � ����������� ��������������
*/

GLenum get_gl_texture_target (TexTargetId tex_target)
{
  switch (tex_target)
  {
    case TexTargetId_Texture1D:        return GL_TEXTURE_1D;
    case TexTargetId_Texture2D:        return GL_TEXTURE_2D;
    case TexTargetId_TextureRectangle: return GL_TEXTURE_RECTANGLE_EXT;
    case TexTargetId_Texture3D:        return GL_TEXTURE_3D;
    case TexTargetId_TextureCubemap:   return GL_TEXTURE_CUBE_MAP_ARB;
    default:
      RaiseInvalidArgument ("get_gl_texture_target", "tex_target", tex_target);
      return 0;
  }
}

TexTargetId get_target_id (GLenum tex_target)
{
  switch (tex_target)
  {
    case GL_TEXTURE_1D:            return TexTargetId_Texture1D;
    case GL_TEXTURE_2D:            return TexTargetId_Texture2D;
    case GL_TEXTURE_RECTANGLE_EXT: return TexTargetId_TextureRectangle;
    case GL_TEXTURE_3D:            return TexTargetId_Texture3D;
    case GL_TEXTURE_CUBE_MAP_ARB:  return TexTargetId_TextureCubemap;
    default:
      RaiseInvalidArgument ("get_TexTargetId", "tex_target", tex_target);
      return (TexTargetId)0;
  }
}

}

/*
   �����������
*/

SamplerState::SamplerState (const ContextManager& manager, const SamplerDesc& in_desc) 
  : ContextObject (manager), desc_hash (0), display_list (0)
{
  SetDesc (in_desc);
}

/*
   ����� �������� � �������� OpenGL
*/

void SamplerState::Bind (GLenum texture_target)
{
  TexTargetId target_id = get_target_id (texture_target);
  
    //����� �������� ���������

  MakeContextCurrent ();
  
    //���������� ���������� �������������

  glCallList (display_list + target_id);

    //�������� ������

  CheckErrors ("render::low_level::opengl::SamplerState::Bind");
}

/*
    ��������� ����������� ��������
*/

void SamplerState::GetDesc (SamplerDesc& out_desc)
{
  out_desc = desc;
}

/*
   ��������� �����������
*/

void SamplerState::SetDesc (const SamplerDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SamplerState::SetDesc";

    //������� �������� ���������� ����������!!!!
  
  const ContextCaps& caps = GetCaps ();

  if (in_desc.max_anisotropy > caps.max_anisotropy)
    if (caps.has_ext_texture_filter_anisotropic)
      RaiseNotSupported (METHOD_NAME, "Can't set anisotropy level %u (max anisotropy level %u)", in_desc.max_anisotropy, caps.max_anisotropy);

  if (!caps.has_ext_shadow_funcs && in_desc.comparision_function != CompareMode_AlwaysPass)
    RaiseNotSupported (METHOD_NAME, "Can't set comparision function %s (GL_EXT_shadow_funcs extension not supported)", get_name (in_desc.comparision_function));
  
    //�������������� ����������� ��������    
  
  enum Texcoord
  {
    Texcoord_U,
    Texcoord_V,
    Texcoord_W,

    Texcoord_Num
  };
  
  GLenum gl_min_filter, gl_mag_filter, gl_wrap [Texcoord_Num], gl_comparision_function;
  
  switch (in_desc.min_filter)
  {
    case TexMinFilter_Point:           gl_min_filter =  GL_NEAREST; break;
    case TexMinFilter_Linear:          gl_min_filter =  GL_LINEAR; break;
    case TexMinFilter_PointMipPoint:   gl_min_filter =  GL_NEAREST_MIPMAP_NEAREST; break;
    case TexMinFilter_LinearMipPoint:  gl_min_filter =  GL_LINEAR_MIPMAP_NEAREST; break;
    case TexMinFilter_PointMipLinear:  gl_min_filter =  GL_NEAREST_MIPMAP_LINEAR; break;
    case TexMinFilter_Default:
    case TexMinFilter_LinearMipLinear: gl_min_filter =  GL_LINEAR_MIPMAP_LINEAR; break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.min_filter", in_desc.min_filter);
      break;
  }
  
  switch (in_desc.mag_filter)
  {
    case TexMagFilter_Point:  gl_mag_filter = GL_NEAREST; break;
    case TexMagFilter_Default:
    case TexMagFilter_Linear: gl_mag_filter = GL_LINEAR; break;    
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.mag_filter", in_desc.mag_filter);
      break;
  }

  TexcoordWrap wrap [Texcoord_Num] = {in_desc.wrap_u, in_desc.wrap_v, in_desc.wrap_w};
  
  static const char* wrap_name [Texcoord_Num] = {"desc.wrap_u", "desc.wrap_v", "desc.wrap_w"};

  for (size_t i=0; i<Texcoord_Num; i++)
  {
    switch (wrap [i])
    {
      case TexcoordWrap_Mirror:
        if (caps.has_arb_texture_mirrored_repeat)
          gl_wrap [i] = GL_MIRRORED_REPEAT; 
        else
          RaiseNotSupported (METHOD_NAME, "Can't set %s=%s (GL_ARB_texture_mirrored_repeat not supported)", wrap_name [i], get_name (wrap [i]));

        break;
      case TexcoordWrap_ClampToBorder: 
        if (caps.has_arb_texture_border_clamp)
          gl_wrap [i] = GL_CLAMP_TO_BORDER;
        else
          RaiseNotSupported (METHOD_NAME, "Can't set %s=%s (GL_ARB_texture_border_clamp not supported)", wrap_name [i], get_name (wrap [i]));
          
        break;
      case TexcoordWrap_Clamp:
        gl_wrap [i] = GL_CLAMP;
        break;
      case TexcoordWrap_Repeat:
        gl_wrap [i] = GL_REPEAT;
        break;
      default:
        RaiseInvalidArgument (METHOD_NAME, wrap_name [i], wrap [i]);
        break;
    }
  }
  
  switch (in_desc.comparision_function)
  {
    case CompareMode_AlwaysFail:   gl_comparision_function = GL_NEVER; break;
    case CompareMode_AlwaysPass:   gl_comparision_function = GL_ALWAYS; break;
    case CompareMode_Equal:        gl_comparision_function = GL_EQUAL; break;
    case CompareMode_NotEqual:     gl_comparision_function = GL_NOTEQUAL; break;
    case CompareMode_Less:         gl_comparision_function = GL_LESS; break;
    case CompareMode_LessEqual:    gl_comparision_function = GL_LEQUAL; break;
    case CompareMode_Greater:      gl_comparision_function = GL_GREATER; break;
    case CompareMode_GreaterEqual: gl_comparision_function = GL_GEQUAL; break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.comparision_function", in_desc.comparision_function);
      break;
  }

    //����� �������� ���������

  MakeContextCurrent ();
  
    //������ ������ � ��������� OpenGL

      ///�������!!! ������� �� ������� ��������    
    
  if (!display_list)
  {
    display_list = glGenLists (TexTargetId_Num);

    if (!display_list)
      RaiseError (METHOD_NAME);
  }
  
    //�������� ������� ������������� ��������� ��� ��������� ����� �������

  for (size_t i=0; i<TexTargetId_Num; i++)
  {
    GLenum tex_target = get_gl_texture_target ((TexTargetId)i);

    glNewList (display_list + i, GL_COMPILE);

    glTexParameteri (tex_target, GL_TEXTURE_MIN_FILTER, gl_min_filter);
    glTexParameteri (tex_target, GL_TEXTURE_MAG_FILTER, gl_mag_filter);

    if (!caps.has_ext_texture_filter_anisotropic)
      glTexParameteri (tex_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, in_desc.max_anisotropy);

    glTexParameteri (tex_target, GL_TEXTURE_WRAP_S, gl_wrap [Texcoord_U]);
    glTexParameteri (tex_target, GL_TEXTURE_WRAP_T, gl_wrap [Texcoord_V]);
    glTexParameteri (tex_target, GL_TEXTURE_WRAP_R, gl_wrap [Texcoord_W]);

    if (caps.has_sgis_texture_lod)
    {
      glTexParameterf (tex_target, GL_TEXTURE_MIN_LOD, in_desc.min_lod);
      glTexParameterf (tex_target, GL_TEXTURE_MAX_LOD, in_desc.max_lod);
    }

    if (caps.has_ext_texture_lod_bias) //?????????
      glTexEnvf (GL_TEXTURE_FILTER_CONTROL_EXT, GL_TEXTURE_LOD_BIAS_EXT, in_desc.mip_lod_bias);

    if (caps.has_ext_shadow_funcs) ///�������� ��������!!!!!!
    {
      glTexParameteri (tex_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
      glTexParameteri (tex_target, GL_TEXTURE_COMPARE_FUNC, gl_comparision_function);
    }

    glTexParameterfv (tex_target, GL_TEXTURE_BORDER_COLOR, in_desc.border_color);

    glEndList ();  
  }

    //�������� ������

  CheckErrors (METHOD_NAME);

    //���������� �����������

  desc      = in_desc;
  desc_hash = crc32 (&desc, sizeof desc);
}
