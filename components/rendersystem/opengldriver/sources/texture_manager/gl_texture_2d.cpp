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
  TextureExtensions ext (GetContextManager ());
  const char* METHOD_NAME = "render::low_level::opengl::Texture2D::Texture2D";
  int width = 0;

  if ((tex_desc.width & 3) && is_compressed_format (tex_desc.format))
    RaiseInvalidArgument (METHOD_NAME, "tex_desc.width", tex_desc.width,
                          "Texture width for compressed image must be a multiple 4");
  
  if ((tex_desc.height & 3) && is_compressed_format (tex_desc.format))
    RaiseInvalidArgument (METHOD_NAME, "tex_desc.height", tex_desc.height,
                          "Texture height for compressed image must be a multiple 4");

  Bind ();

  size_t tex_size; //???dup
  
  //MakeContextCurrent???
 
  if (is_compressed_format (tex_desc.format) && ext.has_ext_texture_compression_s3tc)
  {
    tex_size = tex_desc.width * tex_desc.height / 16 * compressed_quad_size (tex_desc.format); //???dup

    if (ext.has_arb_texture_compression) glCompressedTexImage2DARB (GL_PROXY_TEXTURE_2D, 1, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, 0, 
                                                                   tex_size, NULL);
    else                                 glCompressedTexImage2D    (GL_PROXY_TEXTURE_2D, 1, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, 0, 
                                                                   tex_size, NULL);
  }
  else
    glTexImage2D (GL_PROXY_TEXTURE_2D, 1, unpack_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, 0, 
                  unpack_format (tex_desc.format), unpack_type (tex_desc.format), NULL);
  
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 1, GL_TEXTURE_WIDTH, &width);

  if (!width)
    Raise <Exception> (METHOD_NAME, "Can't create 2d texture %ux%ux%u@%s (proxy texture fail)", tex_desc.width, tex_desc.height, tex_desc.layers, get_name (tex_desc.format));

  for (size_t i=0; i<mips_count; i++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (i, level_desc);

    if (is_compressed_format (tex_desc.format) && ext.has_ext_texture_compression_s3tc)
    {
      tex_size = level_desc.width * level_desc.height / 16 * compressed_quad_size (tex_desc.format); //???dup

      if (ext.has_arb_texture_compression) glCompressedTexImage2DARB (GL_TEXTURE_2D, i, gl_internal_format (tex_desc.format),
                                                                      level_desc.width, level_desc.height, 0, tex_size, 0);
      else                                 glCompressedTexImage2D    (GL_TEXTURE_2D, i, gl_internal_format (tex_desc.format),
                                                                      level_desc.width, level_desc.height, 0, tex_size, 0);
    }
    else
    {
      glTexImage2D (GL_TEXTURE_2D, i, unpack_internal_format (tex_desc.format), level_desc.width,
                    level_desc.height, 0, unpack_format (tex_desc.format), unpack_type (tex_desc.format), 0);
    }
  }

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
  if (glCompressedTexSubImage2D)
  {
    glCompressedTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, format, buffer_size, buffer);
  }
  else
  {
    glCompressedTexSubImage2DARB (GL_TEXTURE_2D, mip_level, x, y, width, height, format, buffer_size, buffer);
  }
}
