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
  bool has_SGIS_generate_mipmap = GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4;

  Bind ();

  glTexImage1D (GL_TEXTURE_1D, 0, gl_internal_format (tex_desc.format), tex_desc.width, 0, gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);

  if (tex_desc.generate_mips_enable)
  {
    mips_count = (size_t)(log ((float)tex_desc.width) / log (2.f));

    if (has_SGIS_generate_mipmap)
      glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, true);
    else
    {
      for (size_t i = 1; i < mips_count; i++)
        glTexImage1D (GL_TEXTURE_1D, i, gl_internal_format (tex_desc.format), tex_desc.width >> i, 0, gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
    }
  }

  CheckErrors ("render::low_level::opengl::Texture1D::Texture1D");
}

/*
   Работа с данными
*/

void Texture1D::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture1D::SetData", "buffer");

  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "mip_level", mip_level, (size_t)0, mips_count);
  if ((x + width) > (desc.width) >> mip_level)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "x + width", x + width, (size_t)0, desc.width >> mip_level);
  if (!width)
    return;
  if (is_compressed_format (source_format))
    RaiseInvalidArgument ("render::low_level::opengl::Texture1D::SetData", "source_format", source_format, "Can't set compressed data for 1d texture.");

  bool has_SGIS_generate_mipmap = GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4;

  MakeContextCurrent ();

  Bind ();
  
  if (mip_level && has_SGIS_generate_mipmap)
    glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, false); 
  glTexSubImage1D (GL_TEXTURE_1D, mip_level, x, width, gl_format (source_format), gl_type (source_format), buffer);
  if (mip_level && has_SGIS_generate_mipmap)
    glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, true);
  
  if (desc.generate_mips_enable && !mip_level && !has_SGIS_generate_mipmap)
  {
    char* source_buffer = (char*)buffer;
    char* mip_buffer = new char [width >> 1 * texel_size (source_format)];

    for (size_t i = 1; i < mips_count; i++, source_buffer = mip_buffer)
    {
      scale_image_2x_down (source_format, width >> i, 1, source_buffer, mip_buffer);
      glTexSubImage1D (GL_TEXTURE_1D, i, x >> i, width >> i, gl_format (source_format), gl_type (source_format), mip_buffer);
    }

    delete [] mip_buffer;
  }

  CheckErrors ("render::low_level::opengl::Texture1D::SetData");
}

void Texture1D::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture1D::GetData", "buffer");
  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::GetData", "mip_level", mip_level, (size_t)0, mips_count);
  if (x)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::GetData", "x", x, (size_t)0, (size_t)0);
  if (width != (desc.width >> mip_level))
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::GetData", "width", width, desc.width >> mip_level, desc.width >> mip_level);
  if (is_compressed_format (target_format))
    RaiseInvalidArgument ("render::low_level::opengl::Texture2D::GetData", "target_format", target_format, "Can't get compressed data from 1d texture.");

  MakeContextCurrent ();
  Bind ();

  glGetTexImage (GL_TEXTURE_1D, mip_level, gl_format (target_format), gl_type (target_format), buffer);
  
  CheckErrors ("render::low_level::opengl::Texture1D::GetData");
}
