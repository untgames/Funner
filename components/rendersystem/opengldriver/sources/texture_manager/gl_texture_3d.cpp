#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture3D::Texture3D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_3D_EXT)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture3D::Texture3D";

    //выбор текущего контекста и биндинг текстуры

  MakeContextCurrent ();
  
  Bind ();
  
    //проверка корректности формата создаваемой текстуры
    
  switch (desc.format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
      break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      RaiseNotSupported (METHOD_NAME, "3D compressed textures not supported (desc.format=%s)", get_name (desc.format));
      return;
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_S8:    
      RaiseNotSupported (METHOD_NAME, "3D depth-stencil textures not supported (desc.format=%s)", get_name (desc.format));
      return;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
      return;
  }
  
    //преобразование формата текстуры
    
  GLenum gl_internal_format = opengl::gl_internal_format (desc.format),
         gl_format          = opengl::gl_format (desc.format),
         gl_type            = opengl::gl_type (desc.format);
  
    //проверка возможности создания текстуры
    
      //??????????????1 и mip-уровни

  if (glTexImage3D)
  {
    glTexImage3D (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format, desc.width, desc.height, desc.layers, 0, gl_format, gl_type, 0);
  }
  else
  {
    glTexImage3DEXT (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format, desc.width, desc.height, desc.layers, 0, gl_format, gl_type, 0);
  }

  size_t proxy_width = 0, proxy_height = 0, proxy_depth = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_WIDTH, (int*)&proxy_width);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_HEIGHT, (int*)&proxy_height);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_DEPTH, (int*)&proxy_depth);

  if (!proxy_width || !proxy_height || !proxy_depth)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create 3D texture %ux%ux%x@%s (proxy texture fail)", desc.width, desc.height, desc.layers,
                       get_name (desc.format));
  }

    //создание mip-уровней

  size_t depth = desc.layers;  

  for (size_t mip_level=0; mip_level<mips_count; mip_level++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (mip_level, level_desc);    

    if (glTexImage3D)
    {
      glTexImage3D (GL_TEXTURE_3D_EXT, mip_level, gl_internal_format, level_desc.width, level_desc.height, depth, 0, gl_format, gl_type, 0);
    }
    else
    {
      glTexImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, gl_internal_format, level_desc.width, level_desc.height, depth, 0, gl_format, gl_type, 0);
    }
  }
  
    //включение автоматической генерации !!!!вынести в Texture::Texture!!!!
    
  TextureExtensions ext (GetContextManager ());//?????

  if (desc.generate_mips_enable && ext.has_sgis_generate_mipmap)
    glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

/*
    Установка данных
*/

void Texture3D::SetUncompressedData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  GLenum      format,
  GLenum      type,
  const void* buffer)
{
  if (glTexSubImage3D) glTexSubImage3D    (GL_TEXTURE_3D, mip_level, x, y, layer, width, height, 1, format, type, buffer);
  else                 glTexSubImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, format, type, buffer);
}

void Texture3D::SetCompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, size_t, const void*)
{
  RaiseNotSupported ("render::low_level::opengl::Texture3D::SetCompressedData", "Compression for 3D textures not supported");
}
