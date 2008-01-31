#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    онструктор / деструктор
*/

Texture2D::Texture2D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_2D)
{
  TextureExtensions ext (GetContextManager ());
  
    //MakeContextCurrent???

  Bind ();
  
  for (size_t i=0; i<mips_count; i++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (i, level_desc);

    if (is_compressed_format (tex_desc.format) && ext.has_ext_texture_compression_s3tc)
    {
      size_t tex_size = level_desc.width * level_desc.height / 16 * compressed_quad_size (tex_desc.format); //???dup
     
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

  CheckErrors ("render::low_level::opengl::Texture2D::Texture2D");
}

/*
   –абота с данными
*/

namespace
{

void SetTexData2D (size_t mip_level, size_t x, size_t y, size_t, size_t width, size_t height, GLenum format, GLenum type, const void* data)
{
  glTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, format, type, data);
}

}

void Texture2D::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture2D::SetData";

  Texture::SetData (layer, mip_level, x, y, width, height, source_format, buffer);
  
  if (mip_level > mips_count)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, (size_t)0, mips_count);
  if (((x + width) > (desc.width >> mip_level)) && ((x + width) != 1))
    RaiseOutOfRange (METHOD_NAME, "x + width", x + width, (size_t)0, desc.width >> mip_level);
  if (((y + height) > (desc.height >> mip_level)) && ((y + height) != 1))
    RaiseOutOfRange (METHOD_NAME, "y + height", y + height, (size_t)0, desc.height >> mip_level);
  if (!width || !height)
    return;
  if (is_compressed_format (desc.format))
  {
    if (desc.generate_mips_enable)
      RaiseInvalidOperation (METHOD_NAME, "Generate mipmaps not compatible with compressed textures.");
    if (x & 3)
      RaiseInvalidArgument (METHOD_NAME, "x", x, "x must be a multiple of 4.");
    if (y & 3)
      RaiseInvalidArgument (METHOD_NAME, "y", y, "y must be a multiple of 4.");
    if (width & 3)
      RaiseInvalidArgument (METHOD_NAME, "width", width, "width must be a multiple of 4.");
    if (height & 3)
      RaiseInvalidArgument (METHOD_NAME, "height", height, "height must be a multiple of 4.");
  }
  if (is_compressed_format (source_format))
    if (source_format != desc.format)
      RaiseInvalidArgument (METHOD_NAME, "source_format");

  TextureExtensions ext (GetContextManager ());

  MakeContextCurrent ();
  Bind ();

  if (is_compressed_format (source_format))
  {
    if (ext.has_ext_texture_compression_s3tc)
    {
      if (ext.has_arb_texture_compression) glCompressedTexSubImage2DARB (GL_TEXTURE_2D, mip_level, x, y, width, height, gl_format (source_format),
                                                                        ((width * height) / 16) * compressed_quad_size (source_format), buffer);
      else                                 glCompressedTexSubImage2D    (GL_TEXTURE_2D, mip_level, x, y, width, height, gl_format (source_format),
                                                                        ((width * height) / 16) * compressed_quad_size (source_format), buffer);
    }
    else
    {
      xtl::uninitialized_storage <char> unpacked_buffer (width * height * unpack_texel_size (source_format));

      unpack_dxt (source_format, width, height, buffer, unpacked_buffer.data ());
      glTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, unpack_format (source_format), unpack_type (source_format), unpacked_buffer.data ());
    }
  }
  else
  {
    if (mip_level && ext.has_sgis_generate_mipmap) // уточнить про перегенерацию при установке mip_level != 0!!!
      glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, false); 

    glTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, gl_format (source_format), gl_type (source_format), buffer);
    
    if (mip_level && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, true);

    if (desc.generate_mips_enable && !mip_level && !ext.has_sgis_generate_mipmap)
      generate_mips (x, y, 0, width, height, source_format, buffer, &SetTexData2D);
  }

  CheckErrors (METHOD_NAME);
}
                                