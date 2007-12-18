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
  Bind ();
  glTexImage1D (GL_TEXTURE_1D, 0, GLInternalFormat (tex_desc.format), tex_desc.width, 0, GLFormat (tex_desc.format), GL_UNSIGNED_BYTE, NULL);
  if (tex_desc.generate_mips_enable)
  {
    mips_count = (size_t)(log ((float)tex_desc.width) / log (2.f));
/*    if (IsExtensionSupported ("GL_SGIS_generate_mipmap"))
      glTexParameteri (GL_TEXTURE_1D, GL_GENERATE_MIPMAP_SGIS, true);
    else
    {
      for (size_t i = 1; i < mips_count; i++)
        glTexImage1D (GL_TEXTURE_1D, i, GLInternalFormat (tex_desc.format), tex_desc.width >> i, 0, GLFormat (tex_desc.format), GL_UNSIGNED_BYTE, NULL);
    }*/
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

  Bind ();
  glTexSubImage1D (GL_TEXTURE_1D, mip_level, x, width, GLFormat (desc.format), GL_UNSIGNED_BYTE, buffer);
/*  if (tex_desc.generate_mips_enable)
  {
    if (!IsExtensionSupported ("GL_SGIS_generate_mipmap"))
    {
      char buffer[tex_desc.width >> 1 * TexelSize (tex_desc.format)];

      for (size_t i = 1; i < mips_count; i++)
      {
        ScaleImage
        glTexImage1D (GL_TEXTURE_1D, i, GLInternalFormat (tex_desc.format), tex_desc.width >> i, 0, GLFormat (tex_desc.format), GL_UNSIGNED_BYTE, NULL);
      }
    }
  }*/
}

void Texture1D::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture1D::SetData", "buffer");
}
