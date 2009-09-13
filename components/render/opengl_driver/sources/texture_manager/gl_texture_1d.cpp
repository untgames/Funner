#ifndef OPENGL_ES_SUPPORT

#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture1D::Texture1D (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data)
  : Texture (manager, tex_desc, GL_TEXTURE_1D, get_mips_count (tex_desc.width))
{
  const char* METHOD_NAME = "render::low_level::opengl::Texture1D::Texture1D";
  
  if (data)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Texture initial data not supported");
  
    //установка текстуры в контекст OpenGL

  Bind ();
  
    //преобразование формата пикселей

  if (is_compressed (tex_desc.format))
    throw xtl::format_not_supported_exception (METHOD_NAME, "1D texture can't be compressed");

  GLenum gl_internal_format = get_gl_internal_format (tex_desc.format),
         gl_format          = get_gl_format (tex_desc.format),
         gl_type            = get_gl_type (tex_desc.format);  
  
    //проверка возможности создания текстуры

  glTexImage1D (GL_PROXY_TEXTURE_1D, 1, gl_internal_format, tex_desc.width, 0, gl_format, gl_type, 0);

  GLint proxy_width = 0;  

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_1D, 1, GL_TEXTURE_WIDTH, &proxy_width);

  if (!proxy_width)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 1D texture %u@%s. Reason: proxy texure fail", tex_desc.width, get_name (tex_desc.format));

    //создание текстуры

  for (size_t i=0; i<GetMipsCount (); i++)
    glTexImage1D (GL_TEXTURE_1D, i, gl_internal_format, tex_desc.width >> i, 0, gl_format, gl_type, 0);

    //проверка ошибок

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
  throw xtl::format_not_supported_exception ("render::low_level::opengl::Texture1D::SetCompressedData", "Compression for 1D textures not supported");
}

#endif
