#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace 
{

/*
    Допустимые типы текстур
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
    Сопоставление типа текстуры её внутреннему идентификатору
*/

GLenum get_gl_texture_target (TexTargetId tex_target)
{
  switch (tex_target)
  {
    case TexTargetId_Texture1D:        return GL_TEXTURE_1D;
    case TexTargetId_Texture2D:        return GL_TEXTURE_2D;
    case TexTargetId_TextureRectangle: return GL_TEXTURE_RECTANGLE_ARB;
    case TexTargetId_Texture3D:        return GL_TEXTURE_3D;
    case TexTargetId_TextureCubemap:   return GL_TEXTURE_CUBE_MAP_ARB;
    default:
      throw xtl::make_argument_exception ("get_gl_texture_target", "tex_target", tex_target);
      return 0;
  }
}

TexTargetId get_target_id (GLenum tex_target)
{
  switch (tex_target)
  {
    case GL_TEXTURE_1D:            return TexTargetId_Texture1D;
    case GL_TEXTURE_2D:            return TexTargetId_Texture2D;
    case GL_TEXTURE_RECTANGLE_ARB: return TexTargetId_TextureRectangle;
    case GL_TEXTURE_3D:            return TexTargetId_Texture3D;
    case GL_TEXTURE_CUBE_MAP_ARB:  return TexTargetId_TextureCubemap;
    default:
      throw xtl::make_argument_exception ("get_TexTargetId", "tex_target", tex_target);
      return (TexTargetId)0;
  }
}

}

/*
    Описание реализации текстурного сэмплера
*/

typedef CommandListBuilder::ExecuterPtr          ExecuterPtr;
typedef xtl::array<ExecuterPtr, TexTargetId_Num> ExecuterArray;    

struct SamplerState::Impl
{
  SamplerDesc   desc;      //дескриптор сэмплера
  size_t        desc_hash; //хэш дескриптора
  ExecuterArray executers; //исполнители списков команд

  Impl () : desc_hash (0) {}
};

/*
    Конструктор
*/

SamplerState::SamplerState (const ContextManager& manager, const SamplerDesc& in_desc) 
  : ContextObject (manager),
    impl (new Impl)
{
  SetDesc (in_desc);
}

SamplerState::~SamplerState ()
{
}

/*
   Выбор сэмплера в контекст OpenGL
*/

void SamplerState::Bind (GLenum texture_target)
{
  TexTargetId target_id = get_target_id (texture_target);
  
    //выбор текущего контекста

  MakeContextCurrent ();
  
    //применение параметров сэмплирования

  impl->executers [target_id]->ExecuteCommands ();

    //проверка ошибок

  CheckErrors ("render::low_level::opengl::SamplerState::Bind");
}

/*
    Получение дескриптора сэмплера и его хэша
*/

void SamplerState::GetDesc (SamplerDesc& out_desc)
{
  out_desc = impl->desc;
}

size_t SamplerState::GetDescHash ()
{
  return impl->desc_hash;
}

/*
   Изменение дескриптора
*/

void SamplerState::SetDesc (const SamplerDesc& in_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::SamplerState::SetDesc";
  
    //проверка поддержки необхожимых расширений

  const ContextCaps& caps = GetCaps ();      

  if (in_desc.max_anisotropy > caps.max_anisotropy) // && caps.has_ext_texture_filter_anisotropic)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't set desc.max_anisotropy=%u (max anisotropy level %u)", in_desc.max_anisotropy, caps.max_anisotropy);
    
  if (in_desc.max_anisotropy < 1.0f)
    throw xtl::make_argument_exception (METHOD_NAME, "desc.max_anisotropy", in_desc.max_anisotropy, "Maximum anisotropy must be >= 1.0");

  if (!caps.has_ext_texture3d && in_desc.wrap_w != TexcoordWrap_Default)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't set desc.wrap_w=%s (GL_EXT_texture3D extension not supported)", get_name (in_desc.wrap_w));
    
  switch (in_desc.comparision_function)
  {
    case CompareMode_AlwaysPass:
      break;
    case CompareMode_LessEqual:
    case CompareMode_GreaterEqual:
      if (!caps.has_arb_shadow)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Can't set desc.comparision_function=%s (GL_ARB_shadow extension not supported)", get_name (in_desc.comparision_function));

      break;  
    case CompareMode_AlwaysFail:
    case CompareMode_Equal:
    case CompareMode_NotEqual:
    case CompareMode_Less:
    case CompareMode_Greater:
      if (!caps.has_ext_shadow_funcs)
        throw xtl::format_not_supported_exception (METHOD_NAME, "Can't set desc.comparision_function=%s (GL_EXT_shadow_funcs extension not supported)", get_name (in_desc.comparision_function));

      break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.comparision_function", in_desc.comparision_function);
      break;
  }  
    
  if (!caps.has_sgis_texture_lod)
  {
    if (in_desc.min_lod != 0.0f)
      throw xtl::format_not_supported_exception (METHOD_NAME, "Can't set desc.min_lod=%g (GL_SGIS_texture_lod extension not supported)", in_desc.min_lod);

    if (in_desc.max_lod != FLT_MAX)
      throw xtl::format_not_supported_exception (METHOD_NAME, "Can't set desc.max_lod=%g (GL_SGIS_texture_lod extension not supported)", in_desc.max_lod);
  }

  if (in_desc.max_lod < in_desc.min_lod)
    throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "desc.max_lod must be gerater or equal than desc.min_lod (desc.min_lod=%g desc.max_lod=%g)",
      in_desc.min_lod, in_desc.max_lod);

  if (in_desc.mip_lod_bias != 0.0f)
    throw xtl::format_not_supported_exception (METHOD_NAME, "desc.mip_lod_bias=%g not supported", in_desc.mip_lod_bias);    

    //преобразование дескриптора сэмплера    

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
    case TexMinFilter_Point:           gl_min_filter = GL_NEAREST; break;
    case TexMinFilter_Linear:          gl_min_filter = GL_LINEAR; break;
    case TexMinFilter_PointMipPoint:   gl_min_filter = GL_NEAREST_MIPMAP_NEAREST; break;
    case TexMinFilter_LinearMipPoint:  gl_min_filter = GL_LINEAR_MIPMAP_NEAREST; break;
    case TexMinFilter_PointMipLinear:  gl_min_filter = GL_NEAREST_MIPMAP_LINEAR; break;
    case TexMinFilter_Default:
    case TexMinFilter_LinearMipLinear: gl_min_filter = GL_LINEAR_MIPMAP_LINEAR; break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.min_filter", in_desc.min_filter);
      break;
  }
  
  switch (in_desc.mag_filter)
  {
    case TexMagFilter_Point:  gl_mag_filter = GL_NEAREST; break;
    case TexMagFilter_Default:
    case TexMagFilter_Linear: gl_mag_filter = GL_LINEAR; break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.mag_filter", in_desc.mag_filter);
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
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't set %s=%s (GL_ARB_texture_mirrored_repeat not supported)", wrap_name [i], get_name (wrap [i]));

        break;
      case TexcoordWrap_ClampToBorder: 
        if (caps.has_arb_texture_border_clamp)
          gl_wrap [i] = GL_CLAMP_TO_BORDER;
        else
          throw xtl::format_not_supported_exception (METHOD_NAME, "Can't set %s=%s (GL_ARB_texture_border_clamp not supported)", wrap_name [i], get_name (wrap [i]));
          
        break;
      case TexcoordWrap_Clamp:
        gl_wrap [i] = GL_CLAMP;
        break;
      case TexcoordWrap_Repeat:
        gl_wrap [i] = GL_REPEAT;
        break;
      default:
        throw xtl::make_argument_exception (METHOD_NAME, wrap_name [i], wrap [i]);
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
      throw xtl::make_argument_exception (METHOD_NAME, "desc.comparision_function", in_desc.comparision_function);
      break;
  }

    //выбор текущего контекста

  MakeContextCurrent ();
  
    //создание списков инициализации сэмплеров для различных типов текстур
    
  CommandListBuilder cmd_list;
  ExecuterArray      new_executers;

  for (size_t i=0; i<TexTargetId_Num; i++)
  {
    GLenum tex_target = get_gl_texture_target ((TexTargetId)i);    
    
    cmd_list.Add (glTexParameteri, tex_target, GL_TEXTURE_MIN_FILTER, gl_min_filter);
    cmd_list.Add (glTexParameteri, tex_target, GL_TEXTURE_MAG_FILTER, gl_mag_filter);

    if (caps.has_ext_texture_filter_anisotropic)
      cmd_list.Add (glTexParameteri, tex_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, in_desc.max_anisotropy);

    cmd_list.Add (glTexParameteri, tex_target, GL_TEXTURE_WRAP_S, gl_wrap [Texcoord_U]);
    cmd_list.Add (glTexParameteri, tex_target, GL_TEXTURE_WRAP_T, gl_wrap [Texcoord_V]);

    if (caps.has_ext_texture3d)
    {
      switch (tex_target)
      {
        case GL_TEXTURE_3D:
        case GL_TEXTURE_CUBE_MAP:
          cmd_list.Add (glTexParameteri, tex_target, GL_TEXTURE_WRAP_R, gl_wrap [Texcoord_W]);
          break;
        default:
          break;        
      }
    }

    if (caps.has_sgis_texture_lod)
    {
      cmd_list.Add (glTexParameterf, tex_target, GL_TEXTURE_MIN_LOD, in_desc.min_lod);
      cmd_list.Add (glTexParameterf, tex_target, GL_TEXTURE_MAX_LOD, in_desc.max_lod);
    }

    if (caps.has_ext_shadow_funcs || caps.has_arb_shadow)
    {
      cmd_list.Add (glTexParameteri, tex_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
      cmd_list.Add (glTexParameteri, tex_target, GL_TEXTURE_COMPARE_FUNC, gl_comparision_function);
    }

    cmd_list.Add (glTexParameterfv, tex_target, GL_TEXTURE_BORDER_COLOR, (const float*)in_desc.border_color);
    
      //создание исполнителя команд

    new_executers [i] = cmd_list.Finish ();

      //проверка ошибок

    CheckErrors (METHOD_NAME);
  }  

    //сохранение параметров

  impl->desc      = in_desc;
  impl->desc_hash = crc32 (&impl->desc, sizeof impl->desc);
  impl->executers = new_executers;

    //оповещение о необходимости ребиндинга уровня

  StageRebindNotify (Stage_TextureManager);
}
