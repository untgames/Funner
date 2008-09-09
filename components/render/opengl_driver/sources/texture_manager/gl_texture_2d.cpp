#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture2D::Texture2D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_2D, get_mips_count (tex_desc.width, tex_desc.height))
{
  const char* METHOD_NAME = "render::low_level::opengl::Texture2D::Texture2D";  
  
    //установка текстуры в контекст OpenGL

  Bind ();

     //преобразование формата пикселей

  GLenum gl_internal_format = GetCaps ().has_ext_texture_compression_s3tc ? get_gl_internal_format (tex_desc.format) :
                              get_uncompressed_gl_internal_format (tex_desc.format),
         gl_format          = get_uncompressed_gl_format (tex_desc.format),
         gl_type            = get_uncompressed_gl_type (tex_desc.format);

    //проверка возможности создания текстуры

  glTexImage2D (GL_PROXY_TEXTURE_2D, 1, gl_internal_format, tex_desc.width, tex_desc.height, 0, gl_format, gl_type, 0);

  GLint proxy_width = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 1, GL_TEXTURE_WIDTH, &proxy_width);

  if (!proxy_width)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 2D texture %ux%u@%s. Reason: proxy texure fail", tex_desc.width,
    tex_desc.height, get_name (tex_desc.format));

      //создание mip-уровней      
      
  for (size_t i=0; i<GetMipsCount (); i++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (i, level_desc);

    glTexImage2D (GL_TEXTURE_2D, i, gl_internal_format, level_desc.width, level_desc.height, 0, gl_format, gl_type, 0);

    glGetTexLevelParameteriv (GL_TEXTURE_2D, i, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&gl_internal_format);
  }    

   //установка реального внутреннего формата хранения пикселей (связано с установкой сжатого формата)

  try
  {
    SetFormat (get_pixel_format (gl_internal_format));
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);
    
    throw;
  }  

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

/*
    Установка данных
*/

void Texture2D::SetUncompressedData
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
  glTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, format, type, buffer);
}

void Texture2D::SetCompressedData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  GLenum      format,
  size_t      buffer_size,
  const void* buffer)
{
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D_fn = 0;

  if (glCompressedTexSubImage2D) glCompressedTexSubImage2D_fn = glCompressedTexSubImage2D;
  else                           glCompressedTexSubImage2D_fn = glCompressedTexSubImage2DARB;

  glCompressedTexSubImage2D_fn (GL_TEXTURE_2D, mip_level, x, y, width, height, format, buffer_size, buffer);
}
