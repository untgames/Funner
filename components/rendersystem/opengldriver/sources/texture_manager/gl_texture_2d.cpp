#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture2D::Texture2D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_2D)
{
    //игнорирование неиспользуемых размеров
    
  desc.layers = 1;

  const char* METHOD_NAME = "render::low_level::opengl::Texture2D::Texture2D";
  
    //установка текстуры в контекст OpenGL

  Bind ();

    //определение поддерживаемых расширений

  TextureExtensions ext (GetContextManager ());    

    //создание текстуры

  if (is_compressed_format (tex_desc.format) && ext.has_ext_texture_compression_s3tc)
  {
      //получение необходимой точки входа

    PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D_fn = 0;

    if (glCompressedTexImage2D) glCompressedTexImage2D_fn = glCompressedTexImage2D;
    else                        glCompressedTexImage2D_fn = glCompressedTexImage2DARB;

      //проверка возможности создания текстуры

    GLenum gl_internal_format = opengl::gl_internal_format (tex_desc.format);    

    glCompressedTexImage2D_fn (GL_PROXY_TEXTURE_2D, 1, gl_internal_format, tex_desc.width, tex_desc.height, 0,
                               get_compressed_image_size (tex_desc.format, tex_desc.width, tex_desc.height), 0);

    GLint proxy_width = 0;

    glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 1, GL_TEXTURE_WIDTH, &proxy_width);

    if (!proxy_width)
      RaiseNotSupported (METHOD_NAME, "Can't create compressed 2D texture %ux%u@%s. Reason: proxy texure fail", tex_desc.width,
      tex_desc.height, get_name (tex_desc.format));
      
      //создание mip-уровней

    for (size_t i=0; i<mips_count; i++)
    {
      MipLevelDesc level_desc;

      GetMipLevelDesc (i, level_desc);

      glCompressedTexImage2D_fn (GL_TEXTURE_2D, i, gl_internal_format, level_desc.width, level_desc.height, 0,
                                 get_compressed_image_size (tex_desc.format, level_desc.width, level_desc.height), 0);
    }
  }
  else
  {
       //преобразование формата пикселей

     GLenum gl_internal_format = unpack_internal_format (tex_desc.format),
            gl_format          = unpack_format (tex_desc.format),
            gl_type            = unpack_type (tex_desc.format);
            
      //проверка возможности создания текстуры
      
    glTexImage2D (GL_PROXY_TEXTURE_2D, 1, gl_internal_format, tex_desc.width, tex_desc.height, 0, gl_format, gl_type, 0);
    
    GLint proxy_width = 0;

    glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 1, GL_TEXTURE_WIDTH, &proxy_width);

    if (!proxy_width)
      RaiseNotSupported (METHOD_NAME, "Can't create 2D texture %ux%u@%s. Reason: proxy texure fail", tex_desc.width,
      tex_desc.height, get_name (tex_desc.format));
      
        //создание mip-уровней
        
    for (size_t i=0; i<mips_count; i++)
    {
      MipLevelDesc level_desc;

      GetMipLevelDesc (i, level_desc);
      
      glTexImage2D (GL_TEXTURE_2D, i, gl_internal_format, level_desc.width, level_desc.height, 0, gl_format, gl_type, 0);
    }
  }

    //установка режима генерации mip-уровней          

  if (tex_desc.generate_mips_enable && ext.has_sgis_generate_mipmap)
    glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);

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
