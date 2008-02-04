#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture1D::Texture1D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_1D)
{
  const char* METHOD_NAME = "render::low_level::opengl::Texture1D::Texture1D";

  if (is_compressed_format (tex_desc.format))
    RaiseNotSupported (METHOD_NAME, "1D texture can't be compressed.");

  Bind ();
  
    //проверка возможности создания текстуры

  GLenum gl_internal_format = opengl::gl_internal_format (tex_desc.format),
         gl_format          = opengl::gl_format (tex_desc.format),
         gl_type            = opengl::gl_type (tex_desc.format);

  glTexImage1D (GL_PROXY_TEXTURE_1D, 1, gl_internal_format, tex_desc.width, 0, gl_format, gl_type, 0);

  GLint proxy_width = 0;  

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_1D, 1, GL_TEXTURE_WIDTH, &proxy_width);

  if (!proxy_width)
    RaiseNotSupported (METHOD_NAME, "Can't create 1d texture %u@%s. Reason: proxy texure fail", tex_desc.width, get_name (tex_desc.format));

    //создание текстуры

  for (size_t i=0; i<mips_count; i++)
    glTexImage1D (GL_TEXTURE_1D, i, gl_internal_format, tex_desc.width >> i, 0, gl_format, gl_type, 0);
    
  static Extension SGIS_generate_mipmap = "GL_SGIS_generate_mipmap",
                   Version_1_4          = "GL_VERSION_1_4";

  bool has_SGIS_generate_mipmap = IsSupported (SGIS_generate_mipmap) || IsSupported (Version_1_4);  
    

  if (tex_desc.generate_mips_enable && has_SGIS_generate_mipmap)
      glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, true);

  CheckErrors (METHOD_NAME);
}

/*
    Установка данных
*/

void Texture1D::SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t, size_t width, size_t, GLenum format, GLenum type, const void* buffer)
{
  glTexSubImage1D (GL_TEXTURE_1D, mip_level, x, width, format, type, buffer);
}

void Texture1D::SetCompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, size_t, const void*)
{
  RaiseNotSupported ("render::low_level::opengl::Texture1D::SetCompressedData", "Compression for 1D textures not supported");
}
