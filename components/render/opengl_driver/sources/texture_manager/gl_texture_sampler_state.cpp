#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Идентификаторы текстурных координат
*/

enum Texcoord
{
  Texcoord_U,
  Texcoord_V,
  Texcoord_W,

  Texcoord_Num
};

/*
    Описание реализации текстурного сэмплера
*/

struct SamplerState::Impl
{
  SamplerDesc desc;                    //дескриптор сэмплера
  size_t      desc_hash;               //хэш дескриптора
  GLenum      gl_min_filter;           //фильтр минимизации
  GLenum      gl_mag_filter;           //фильтр увеличения
  GLenum      gl_wrap [Texcoord_Num];  //параметры отсечения текстурных координат
  GLenum      gl_comparision_function; //функция сравнения depth-текстур

  Impl () : desc_hash (0), gl_min_filter (GL_NEAREST), gl_mag_filter (GL_NEAREST), gl_comparision_function (GL_ALWAYS)
  {
    memset (&desc, 0, sizeof desc);
    
    desc_hash = crc32 (&desc, sizeof desc);
    
    for (int i=0; i<Texcoord_Num; i++)
      gl_wrap [i] = GL_REPEAT;
  }
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

void SamplerState::Bind (GLenum tex_target, bool is_depth)
{
    //выбор текущего контекста

  MakeContextCurrent ();

  const ContextCaps& caps = GetCaps ();
  
    //установка параметров в контекст

  glTexParameteri (tex_target, GL_TEXTURE_MIN_FILTER, impl->gl_min_filter);
  glTexParameteri (tex_target, GL_TEXTURE_MAG_FILTER, impl->gl_mag_filter);

  if (caps.has_ext_texture_filter_anisotropic)
    glTexParameteri (tex_target, GL_TEXTURE_MAX_ANISOTROPY_EXT, impl->desc.max_anisotropy);

  glTexParameteri (tex_target, GL_TEXTURE_WRAP_S, impl->gl_wrap [Texcoord_U]);
  glTexParameteri (tex_target, GL_TEXTURE_WRAP_T, impl->gl_wrap [Texcoord_V]);

  switch (tex_target)
  {
    case GL_TEXTURE_3D:
    case GL_TEXTURE_CUBE_MAP:
      glTexParameteri (tex_target, GL_TEXTURE_WRAP_R, impl->gl_wrap [Texcoord_W]);
      break;
    default:
      break;        
  }

  if (caps.has_sgis_texture_lod)
  {
    glTexParameterf (tex_target, GL_TEXTURE_MIN_LOD, impl->desc.min_lod);
    glTexParameterf (tex_target, GL_TEXTURE_MAX_LOD, impl->desc.max_lod);
  }

  if (is_depth && (caps.has_ext_shadow_funcs || caps.has_arb_shadow))
  {
    glTexParameteri (tex_target, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri (tex_target, GL_TEXTURE_COMPARE_FUNC, impl->gl_comparision_function);
  }

  glTexParameterfv (tex_target, GL_TEXTURE_BORDER_COLOR, (const float*)impl->desc.border_color);

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
  static const char* METHOD_NAME           = "render::low_level::opengl::SamplerState::SetDesc";
  static Extension   BUG_texture_no_mipmap = "GLBUG_texture_no_mipmap";
  
    //проверка поддержки необхоимых расширений

  const ContextCaps& caps = GetCaps ();

  if (in_desc.max_anisotropy < 1)
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
  
  GLenum gl_min_filter, gl_mag_filter, gl_wrap [Texcoord_Num], gl_comparision_function;
  
  if (IsSupported (BUG_texture_no_mipmap))
  {
    switch (in_desc.min_filter)
    {
      case TexMinFilter_Point:
      case TexMinFilter_PointMipPoint:
      case TexMinFilter_PointMipLinear:  gl_min_filter = GL_NEAREST; break;      
      case TexMinFilter_Default:
      case TexMinFilter_Linear:
      case TexMinFilter_LinearMipPoint:
      case TexMinFilter_LinearMipLinear: gl_min_filter = GL_LINEAR; break;
      default:
        throw xtl::make_argument_exception (METHOD_NAME, "desc.min_filter", in_desc.min_filter);
    }    
  }
  else
  {
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
    }    
  }
  
  switch (in_desc.mag_filter)
  {
    case TexMagFilter_Point:  gl_mag_filter = GL_NEAREST; break;
    case TexMagFilter_Default:
    case TexMagFilter_Linear: gl_mag_filter = GL_LINEAR; break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.mag_filter", in_desc.mag_filter);
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
    }
  }

  switch (in_desc.comparision_function)
  {
    case CompareMode_AlwaysFail:   gl_comparision_function = GL_NEVER;    break;
    case CompareMode_AlwaysPass:   gl_comparision_function = GL_ALWAYS;   break;
    case CompareMode_Equal:        gl_comparision_function = GL_EQUAL;    break;
    case CompareMode_NotEqual:     gl_comparision_function = GL_NOTEQUAL; break;
    case CompareMode_Less:         gl_comparision_function = GL_LESS;     break;
    case CompareMode_LessEqual:    gl_comparision_function = GL_LEQUAL;   break;
    case CompareMode_Greater:      gl_comparision_function = GL_GREATER;  break;
    case CompareMode_GreaterEqual: gl_comparision_function = GL_GEQUAL;   break;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.comparision_function", in_desc.comparision_function);  
  }
  
  size_t max_anisotropy = in_desc.max_anisotropy;

  if (max_anisotropy > caps.max_anisotropy)
    max_anisotropy = caps.max_anisotropy;

    //сохранение параметров

  impl->desc                    = in_desc;
  impl->desc.max_anisotropy     = max_anisotropy;
  impl->desc_hash               = crc32 (&impl->desc, sizeof impl->desc);
  impl->gl_min_filter           = gl_min_filter;
  impl->gl_mag_filter           = gl_mag_filter;
  impl->gl_comparision_function = gl_comparision_function;

  for (size_t i=0; i<Texcoord_Num; i++)
    impl->gl_wrap [i] = gl_wrap [i];

    //оповещение о необходимости ребиндинга уровня

  StageRebindNotify (Stage_TextureManager);
}
