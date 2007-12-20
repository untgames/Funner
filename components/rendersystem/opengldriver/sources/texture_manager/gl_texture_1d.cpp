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
  glTexImage1D (GL_TEXTURE_1D, 0, GLInternalFormat (tex_desc.format), tex_desc.width, 0, GLFormat (tex_desc.format), GLType (tex_desc.format), NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  if (tex_desc.generate_mips_enable)
  {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    mips_count = (size_t)(log ((float)tex_desc.width) / log (2.f));

    if (has_SGIS_generate_mipmap)
      glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, true);
    else
    {
      for (size_t i = 1; i < mips_count; i++)
        glTexImage1D (GL_TEXTURE_1D, i, GLInternalFormat (tex_desc.format), tex_desc.width >> i, 0, GLFormat (tex_desc.format), GLType (tex_desc.format), NULL);
    }
  }
}

/*
   Работа с данными
*/

void Texture1D::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, const void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture1D::SetData", "buffer");

  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "mip_level", 0, mips_count);
  if (x + width > desc.width)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "x + width", 0, desc.width);
  if (!width)
    return;

  bool has_SGIS_generate_mipmap = GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4;

  Bind ();

  if (mip_level && has_SGIS_generate_mipmap)
    glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, false); 
  glTexSubImage1D (GL_TEXTURE_1D, mip_level, x, width, GLFormat (desc.format), GLType (desc.format), buffer);
  if (mip_level && has_SGIS_generate_mipmap)
    glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, true);
  
  if (desc.generate_mips_enable && !mip_level && !has_SGIS_generate_mipmap)
  {
    char* source_buffer = (char*)buffer;
    char* mip_buffer = new char [width >> 1 * TexelSize (desc.format)];

    for (size_t i = 1; i < mips_count; i++, source_buffer = mip_buffer)
    {
      ScaleImage2XDown (desc.format, width >> i, 1, source_buffer, mip_buffer);
      glTexSubImage1D (GL_TEXTURE_1D, i, x >> i, width >> i, GLFormat (desc.format), GLType (desc.format), mip_buffer);
    }

    delete [] mip_buffer;
  }
}

void Texture1D::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture1D::SetData", "buffer");
  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::GetData", "mip_level", 0, mips_count);
  if (x)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::GetData", "x", 0, 0);
  if (width != desc.width)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::GetData", "width", desc.width, desc.width);

  glGetTexImage (GL_TEXTURE_1D, mip_level, GLFormat (desc.format), GLType (desc.format), buffer);
}
