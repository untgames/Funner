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
            
  static Extension SGIS_generate_mipmap = "GL_SGIS_generate_mipmap",
                   Version_1_4          = "GL_VERSION_1_4";

  bool has_SGIS_generate_mipmap = GetContextManager().IsSupported (SGIS_generate_mipmap) || GetContextManager().IsSupported (Version_1_4);  
  int  width = 0;

  Bind ();  
  
  glTexImage1D (GL_PROXY_TEXTURE_1D, 1, gl_internal_format (tex_desc.format), tex_desc.width, 0, gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
  
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_1D, 1, GL_TEXTURE_WIDTH, &width);

  if (!width)
    Raise <Exception> (METHOD_NAME, "Can't create 1d texture %ux%ux%u@%s (proxy texture failed)", tex_desc.width, tex_desc.height, tex_desc.layers, get_name (tex_desc.format));

  for (size_t i = 0; i < mips_count; i++)
    glTexImage1D (GL_TEXTURE_1D, i, gl_internal_format (tex_desc.format), tex_desc.width >> i, 0, gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);

  if (tex_desc.generate_mips_enable)
    if (has_SGIS_generate_mipmap)
      glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, true);

  CheckErrors (METHOD_NAME);
}

/*
   Работа с данными
*/

namespace
{

void SetTexData1D (size_t mip_level, size_t x, size_t, size_t, size_t width, size_t, GLenum format, GLenum type, const void* data)
{
  glTexSubImage1D (GL_TEXTURE_1D, mip_level, x, width, format, type, data);
}

}

void Texture1D::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture1D::SetData";

  Texture::SetData (layer, mip_level, x, y, width, height, source_format, buffer);  
  
  if (mip_level > mips_count)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, (size_t)0, mips_count);
    
  if ((x + width) > (desc.width) >> mip_level)
    RaiseOutOfRange (METHOD_NAME, "x + width", x + width, (size_t)0, desc.width >> mip_level);
  if (!width)
    return;
  if (is_compressed_format (source_format))
    RaiseInvalidArgument (METHOD_NAME, "source_format", source_format, "Can't set compressed data for 1d texture.");

  static Extension SGIS_generate_mipmap = "GL_SGIS_generate_mipmap",
                   Version_1_4          = "GL_VERSION_1_4";

  bool has_SGIS_generate_mipmap = GetContextManager().IsSupported (SGIS_generate_mipmap) || GetContextManager().IsSupported (Version_1_4);

  MakeContextCurrent ();

  Bind ();
  
  if (mip_level && has_SGIS_generate_mipmap)
    glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, false); 

  glTexSubImage1D (GL_TEXTURE_1D, mip_level, x, width, gl_format (source_format), gl_type (source_format), buffer);

  if (mip_level && has_SGIS_generate_mipmap)
    glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, true);

  if (desc.generate_mips_enable && !mip_level && !has_SGIS_generate_mipmap)
  {
    generate_mips (x, 0, 0, width, 1, source_format, buffer, &SetTexData1D);
  }

  CheckErrors (METHOD_NAME);
}
