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
  TextureExtensions ext (GetContextManager ());
  
    //MakeContextCurrent???

  Bind ();

    //создание mip-уровней

  for (size_t i=0; i<mips_count; i++)
  {        
    MipLevelDesc level_desc;

    GetMipLevelDesc (i, level_desc);

    for (size_t j=0; j<6; j++)
    {      
      if (is_compressed_format (tex_desc.format) && ext.has_ext_texture_compression_s3tc)
      {
        size_t tex_size = level_desc.width * level_desc.height / 16 * compressed_quad_size (tex_desc.format);
       
        if (ext.has_arb_texture_compression) glCompressedTexImage2DARB (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, gl_internal_format (tex_desc.format),
                                                                        level_desc.width, level_desc.height, 0, tex_size, 0);
        else                                 glCompressedTexImage2D    (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, gl_internal_format (tex_desc.format),
                                                                        level_desc.width, level_desc.height, 0, tex_size, 0);
      }
      else
      {
        glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, unpack_internal_format (tex_desc.format), level_desc.width,
                      level_desc.height, 0, unpack_format (tex_desc.format), unpack_type (tex_desc.format), 0);
      }
    }
  }

  if (tex_desc.generate_mips_enable && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, true);

  CheckErrors ("render::low_level::opengl::TextureCubemap::TextureCubemap");
}

/*
    Получение дескриптора слоя текстуры
*/

void TextureCubemap::GetLayerDesc (size_t layer, LayerDesc& desc)
{
  if (layer > 6)
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::GetLayerDesc", "layer", layer, 6);

  desc.target    = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer;
  desc.new_index = 0;
}

/*
   Работа с данными
*/

namespace
{

void SetTexDataCubemap (size_t mip_level, size_t x, size_t y, size_t z, size_t width, size_t height, GLenum format, GLenum type, const void* data)
{
  glTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + z, mip_level, x, y, width, height, format, type, data);
}

}

void TextureCubemap::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  Texture::SetData (layer, mip_level, x, y, width, height, source_format, buffer);
  
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

  TextureExtensions ext (GetContextManager ());

  MakeContextCurrent ();
  Bind ();

  if (is_compressed_format (source_format))
  {
    if (ext.has_ext_texture_compression_s3tc)
    {
      if (ext.has_arb_texture_compression) glCompressedTexSubImage2DARB (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, x, y, width, height, gl_format (source_format), 
                                                                        ((width * height) >> 4) * compressed_quad_size (source_format), buffer);
      else                                 glCompressedTexSubImage2D    (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, x, y, width, height, gl_format (source_format), 
                                                                        ((width * height) >> 4) * compressed_quad_size (source_format), buffer);
    }
    else
    {
      xtl::uninitialized_storage <char> unpacked_buffer (width * height * unpack_texel_size (source_format));

      unpack_dxt (source_format, width, height, buffer, unpacked_buffer.data ());
      glTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, x, y, width, height, unpack_format (source_format), unpack_type (source_format), unpacked_buffer.data ());
    }
  }
  else
  {
    if (mip_level && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, false); 
    
    glTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, x, y, width, height, gl_format (source_format), gl_type (source_format), buffer);
    
    if (mip_level && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, true);

    if (desc.generate_mips_enable && !mip_level && !ext.has_sgis_generate_mipmap)
    {    
      generate_cubemap_mips (x, y, layer, width, height, source_format, buffer, &SetTexDataCubemap);
    }
  }

  CheckErrors ("render::low_level::opengl::TextureCubemap::SetData");
}
