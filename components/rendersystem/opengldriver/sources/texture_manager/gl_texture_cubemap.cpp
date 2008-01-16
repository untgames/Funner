#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

TextureCubemap::TextureCubemap  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_CUBE_MAP_ARB)
{
  static Extension SGIS_generate_mipmap = "GL_SGIS_generate_mipmap",
                   Version_1_4          = "GL_VERSION_1_4";

  bool has_SGIS_generate_mipmap = GetContextManager().IsSupported (SGIS_generate_mipmap) || GetContextManager().IsSupported (Version_1_4);

  Bind ();

  for (size_t i = 0; i < 6; i++)
  {
    if (is_compressed_format (tex_desc.format))   
      glCompressedTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, 0, gl_internal_format (tex_desc.format), tex_desc.width, 
                              tex_desc.height, 0, ((tex_desc.width * tex_desc.height) >> 4) * compressed_quad_size (tex_desc.format), NULL);
    else
      glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + i, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, 0, 
                    gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
  }

  if (tex_desc.generate_mips_enable)
  {
    tex_desc.width > tex_desc.height ? mips_count = (size_t)(log ((float)tex_desc.width) / log (2.f)) : 
                                       mips_count = (size_t)(log ((float)tex_desc.height) / log (2.f));

    if (has_SGIS_generate_mipmap)
      glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, true);
    else
    {
      size_t width = tex_desc.width; size_t height = tex_desc.height;

      for (size_t i = 1; i < mips_count; i++)
      {
        for (size_t j = 0; j < 6; j++)
        {
          if (is_compressed_format (tex_desc.format))   
            glCompressedTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, gl_internal_format (tex_desc.format), width, 
                                    height, 0, ((width * height) >> 4) / compressed_quad_size (tex_desc.format), NULL);
          else
            glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, gl_internal_format (tex_desc.format), width, height, 0, 
                          gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
        }
        if (width > 1) 
          width = width >> i;
        if (height > 1)
          height = height >> i;
      }
    }
  }

  CheckErrors ("render::low_level::opengl::TextureCubemap::TextureCubemap");
}

/*
   Работа с данными
*/

void TextureCubemap::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::TextureCubemap::SetData", "buffer");

  if (layer > 5)
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::SetData", "layer", layer, (size_t)0, (size_t)5);
  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::SetData", "mip_level", mip_level, (size_t)0, mips_count);
  if (((x + width) > (desc.width >> mip_level)) && ((x + width) != 1))
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::SetData", "x + width", x + width, (size_t)0, desc.width >> mip_level);
  if (((y + height) > (desc.height >> mip_level)) && ((y + height) != 1))
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::SetData", "y + height", y + height, (size_t)0, desc.height >> mip_level);
  if (!width || !height)
    return;
  if (is_compressed_format (desc.format))
  {
    if (desc.generate_mips_enable)
      RaiseInvalidOperation ("render::low_level::opengl::TextureCubemap::SetData", "Generate mipmaps not compatible with compressed textures.");
    if (x & 3)
      RaiseInvalidArgument ("render::low_level::opengl::TextureCubemap::SetData", "x", x, "x must be a multiple of 4.");
    if (y & 3)
      RaiseInvalidArgument ("render::low_level::opengl::TextureCubemap::SetData", "y", y, "y must be a multiple of 4.");
    if (width & 3)
      RaiseInvalidArgument ("render::low_level::opengl::TextureCubemap::SetData", "width", width, "width must be a multiple of 4.");
    if (height & 3)
      RaiseInvalidArgument ("render::low_level::opengl::TextureCubemap::SetData", "height", height, "height must be a multiple of 4.");
  }
  if (is_compressed_format (source_format))
    if (source_format != desc.format)
      RaiseInvalidArgument ("render::low_level::opengl::TextureCubemap::SetData", "source_format");

  static Extension SGIS_generate_mipmap = "GL_SGIS_generate_mipmap",
                   Version_1_4          = "GL_VERSION_1_4";

  bool has_SGIS_generate_mipmap = GetContextManager().IsSupported (SGIS_generate_mipmap) || GetContextManager().IsSupported (Version_1_4);

  MakeContextCurrent ();
  Bind ();

  if (is_compressed_format (source_format))
    glCompressedTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, x, y, width, height, gl_format (source_format), 
                               ((width * height) >> 4) * compressed_quad_size (source_format), buffer);
  else
  {
    if (mip_level && has_SGIS_generate_mipmap)
      glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, false); 
    glTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, x, y, width, height, gl_format (source_format), gl_type (source_format), buffer);
    if (mip_level && has_SGIS_generate_mipmap)
      glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, true);

    if (desc.generate_mips_enable && !mip_level && !has_SGIS_generate_mipmap)
    {    
      if (width > 1)
        width = width >> 1;
      if (height > 1)
        height = height >> 1;

      char* source_buffer = (char*)buffer;
      char* mip_buffer = new char [width * height * texel_size (source_format)];

      for (size_t i = 1; i < mips_count; i++, source_buffer = mip_buffer)
      {
        scale_image_2x_down (source_format, width, height, source_buffer, mip_buffer);

        glTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, i, x, y, width, height, gl_format (source_format), gl_type (source_format), mip_buffer);

        if (width > 1)
          width = width >> 1;
        if (height > 1)
          height = height >> 1;
      }

      delete [] mip_buffer;
    }
  }

  CheckErrors ("render::low_level::opengl::TextureCubemap::SetData");
}

void TextureCubemap::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::TextureCubemap::GetData", "buffer");
  if (layer > 5)
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::GetData", "layer", layer, (size_t)0, (size_t)5);
  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::GetData", "mip_level", mip_level, (size_t)0, mips_count);
  if (x)
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::GetData", "x", x, (size_t)0, (size_t)0);
  if (y)
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::GetData", "y", y, (size_t)0, (size_t)0);
  if (width != (desc.width >> mip_level))
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::GetData", "width", width, desc.width >> mip_level, desc.width >> mip_level);
  if (height != (desc.height >> mip_level))
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::GetData", "height", height, desc.height >> mip_level, desc.height >> mip_level);
  if (is_compressed_format (target_format))
    if (target_format != desc.format)
      RaiseInvalidArgument ("render::low_level::opengl::TextureCubemap::GetData", "target_format", target_format, "Can't get compressed texture data, format is different.");

  MakeContextCurrent ();
  Bind ();

  if (is_compressed_format (target_format))
    glGetCompressedTexImage (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, buffer);
  else
    glGetTexImage (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, gl_format (target_format), gl_type (target_format), buffer);
  
  CheckErrors ("render::low_level::opengl::TextureCubemap::GetData");
}
