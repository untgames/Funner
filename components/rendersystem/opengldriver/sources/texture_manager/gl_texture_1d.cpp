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
  static Extension SGIS_generate_mipmap = "GL_SGIS_generate_mipmap",
                   Version_1_4          = "GL_VERSION_1_4";

  bool has_SGIS_generate_mipmap = GetContextManager().IsSupported (SGIS_generate_mipmap) || GetContextManager().IsSupported (Version_1_4);  

  Bind ();  

  for (size_t i = 0; i < mips_count; i++)
    glTexImage1D (GL_TEXTURE_1D, i, gl_internal_format (tex_desc.format), tex_desc.width >> i, 0, gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);

  if (tex_desc.generate_mips_enable)
    if (has_SGIS_generate_mipmap)
      glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, true);

  CheckErrors ("render::low_level::opengl::Texture1D::Texture1D");
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
  Texture::SetData (layer, mip_level, x, y, width, height, source_format, buffer);  
  
  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "mip_level", mip_level, (size_t)0, mips_count);
    
  if ((x + width) > (desc.width) >> mip_level)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "x + width", x + width, (size_t)0, desc.width >> mip_level);
  if (!width)
    return;
  if (is_compressed_format (source_format))
    RaiseInvalidArgument ("render::low_level::opengl::Texture1D::SetData", "source_format", source_format, "Can't set compressed data for 1d texture.");

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

  CheckErrors ("render::low_level::opengl::Texture1D::SetData");
}
