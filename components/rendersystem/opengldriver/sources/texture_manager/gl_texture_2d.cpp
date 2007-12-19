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
  bool has_SGIS_generate_mipmap = GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4;

  Bind ();
  glTexImage2D (GL_TEXTURE_2D, 0, GLInternalFormat (tex_desc.format), tex_desc.width, tex_desc.height, 0, GLFormat (tex_desc.format), GL_UNSIGNED_BYTE, NULL);
  if (tex_desc.generate_mips_enable)
  {
    tex_desc.width > tex_desc.height ? mips_count = (size_t)(log ((float)tex_desc.width) / log (2.f)) : mips_count = (size_t)(log ((float)tex_desc.height) / log (2.f));

    if (has_SGIS_generate_mipmap)
      glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);
    else
    {
      size_t width = tex_desc.width; size_t height = tex_desc.height;

      for (size_t i = 1; i < mips_count; i++)
      {
        glTexImage2D (GL_TEXTURE_2D, i, GLInternalFormat (tex_desc.format), width, height, 0, GLFormat (tex_desc.format), GL_UNSIGNED_BYTE, NULL);
        if (width > 1) 
          width = width >> i;
        if (height > 1)
          height = height >> i;
      }
    }
  }
}

/*
   Работа с данными
*/

void Texture2D::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, const void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture2D::SetData", "buffer");

  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture1D::SetData", "mip_level", 0, mips_count);
  if (x + width > desc.width)
    RaiseOutOfRange ("render::low_level::opengl::Texture2D::SetData", "x + width", 0, desc.width);
  if (y + height > desc.height)
    RaiseOutOfRange ("render::low_level::opengl::Texture2D::SetData", "y + height", 0, desc.height);
  if (!width || !height)
    return;

  bool has_SGIS_generate_mipmap = GLEW_SGIS_generate_mipmap || GLEW_VERSION_1_4;

  Bind ();

  if (mip_level && has_SGIS_generate_mipmap)
    glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, false); 
  glTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, GLFormat (desc.format), GL_UNSIGNED_BYTE, buffer);
  if (mip_level && has_SGIS_generate_mipmap)
    glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);

  if (desc.generate_mips_enable && !mip_level && !has_SGIS_generate_mipmap)
  {    
    if (width > 1)
      width = width >> 1;
    if (height > 1)
      height = height >> 1;

    char* source_buffer = (char*)buffer;
    char* mip_buffer = new char [width * height * TexelSize (desc.format)];

    for (size_t i = 1; i < mips_count; i++, source_buffer = mip_buffer)
    {
      ScaleImage2XDown (desc.format, width, height, source_buffer, mip_buffer);

      glTexSubImage2D (GL_TEXTURE_2D, i, x, y, width, height, GLFormat (desc.format), GL_UNSIGNED_BYTE, mip_buffer);

      if (width > 1)
        width = width >> 1;
      if (height > 1)
        height = height >> 1;
    }

    delete [] mip_buffer;
  }
}

void Texture2D::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture2D::SetData", "buffer");

  RaiseNotImplemented ("render::low_level::opengl::Texture2D::GetData"); 
}
