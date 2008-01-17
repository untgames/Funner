#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

namespace
{

struct TextureExtensions
{
  bool has_ext_texture_compression_s3tc; //GL_EXT_texture_compression_s3tc
  bool has_sgis_generate_mipmap;         //GL_SGIS_generate_mipmap
  
  TextureExtensions (const ContextManager& manager)
  {
    static Extension SGIS_generate_mipmap         = "GL_SGIS_generate_mipmap",
                     EXT_texture_compression_s3tc = "GL_EXT_texture_compression_s3tc",
                     ARB_texture_compression      = "GL_ARB_texture_compression",
                     Version_1_3                  = "GL_VERSION_1_3",
                     Version_1_4                  = "GL_VERSION_1_4";
      
    has_ext_texture_compression_s3tc = (manager.IsSupported (ARB_texture_compression) || manager.IsSupported (Version_1_3)) && 
                                       manager.IsSupported (EXT_texture_compression_s3tc);
    has_sgis_generate_mipmap         = (manager.IsSupported (SGIS_generate_mipmap) || manager.IsSupported (Version_1_4));
  }
};

}

/*
   Конструктор / деструктор
*/

Texture3D::Texture3D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_3D_EXT)
{
  TextureExtensions ext (GetContextManager ());

  Bind ();

  if (is_compressed_format (tex_desc.format))   
  {   
    if (ext.has_ext_texture_compression_s3tc)
      glCompressedTexImage3D (GL_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers,
                              0, ((tex_desc.width * tex_desc.height) >> 4) * compressed_quad_size (tex_desc.format), NULL);
    else
      glTexImage3DEXT (GL_TEXTURE_3D_EXT, 0, unpack_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers, 0, 
                       unpack_format (tex_desc.format), unpack_type (tex_desc.format), NULL);
  }
  else
    glTexImage3DEXT (GL_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers, 0, 
                     gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);

  if (tex_desc.generate_mips_enable)
  {
    tex_desc.width > tex_desc.height ? mips_count = (size_t)(log ((float)tex_desc.width) / log (2.f)) : 
                                       mips_count = (size_t)(log ((float)tex_desc.height) / log (2.f));

    if (ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, true);
    else
    {
      size_t width = tex_desc.width; size_t height = tex_desc.height;

      for (size_t i = 1; i < mips_count; i++)
      {
        if (is_compressed_format (tex_desc.format))   
        {
          if (ext.has_ext_texture_compression_s3tc)
            glCompressedTexImage3D (GL_TEXTURE_3D_EXT, i, gl_internal_format (tex_desc.format), width, height, tex_desc.layers,
                                    0, ((width * height) >> 4) / compressed_quad_size (tex_desc.format), NULL);
          else
            glTexImage3DEXT (GL_TEXTURE_3D_EXT, i, unpack_internal_format (tex_desc.format), width, height, tex_desc.layers, 0, 
                             unpack_format (tex_desc.format), unpack_type (tex_desc.format), NULL);
        }
        else
          glTexImage3DEXT (GL_TEXTURE_3D_EXT, i, gl_internal_format (tex_desc.format), width, height, tex_desc.layers, 0, 
                           gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);
        if (width > 1) 
          width = width >> i;
        if (height > 1)
          height = height >> i;
      }
    }
  }

  CheckErrors ("render::low_level::opengl::Texture3D::Texture3D");
}

/*
   Работа с данными
*/

void Texture3D::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture3D::SetData", "buffer");

  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::SetData", "mip_level", mip_level, (size_t)0, mips_count);
  if (layer > desc.layers)
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::SetData", "layer",layer, (size_t)0, desc.layers);
  if (((x + width) > (desc.width >> mip_level)) && ((x + width) != 1))
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::SetData", "x + width", x + width, (size_t)0, desc.width >> mip_level);
  if (((y + height) > (desc.height >> mip_level)) && ((y + height) != 1))
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::SetData", "y + height", y + height, (size_t)0, desc.height >> mip_level);
  if (!width || !height)
    return;
  if (is_compressed_format (desc.format))
  {
    if (desc.generate_mips_enable)
      RaiseInvalidOperation ("render::low_level::opengl::Texture3D::SetData", "Generate mipmaps not compatible with compressed textures.");
    if (x & 3)
      RaiseInvalidArgument ("render::low_level::opengl::Texture3D::SetData", "x", x, "x must be a multiple of 4.");
    if (y & 3)
      RaiseInvalidArgument ("render::low_level::opengl::Texture3D::SetData", "y", y, "y must be a multiple of 4.");
    if (width & 3)
      RaiseInvalidArgument ("render::low_level::opengl::Texture3D::SetData", "width", width, "width must be a multiple of 4.");
    if (height & 3)
      RaiseInvalidArgument ("render::low_level::opengl::Texture3D::SetData", "height", height, "height must be a multiple of 4.");
  }
  if (is_compressed_format (source_format))
    if (source_format != desc.format)
      RaiseInvalidArgument ("render::low_level::opengl::Texture3D::SetData", "source_format");

  TextureExtensions ext (GetContextManager ());

  MakeContextCurrent ();
  Bind ();

  if (is_compressed_format (source_format))
  {
    if (ext.has_ext_texture_compression_s3tc)
      glCompressedTexSubImage3D (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, gl_format (source_format), 
                                 ((width * height) >> 4) * compressed_quad_size (source_format), buffer);
    else
    {
      char* unpacked_buffer = new char [width * height * unpack_texel_size (source_format)];

      unpack_dxt (source_format, width, height, buffer, unpacked_buffer);
      glTexSubImage3D (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, unpack_format (source_format), unpack_type (source_format), unpacked_buffer);

      delete [] unpacked_buffer;
    }
  }
  else
  {
    if (mip_level && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, false); 
    glTexSubImage3D (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, gl_format (source_format), gl_type (source_format), buffer);
    if (mip_level && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, true);

    if (desc.generate_mips_enable && !mip_level && !ext.has_sgis_generate_mipmap)
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

        glTexSubImage3D (GL_TEXTURE_3D_EXT, i, x, y, layer, width, height, 1, gl_format (source_format), gl_type (source_format), mip_buffer);

        if (width > 1)
          width = width >> 1;
        if (height > 1)
          height = height >> 1;
      }

      delete [] mip_buffer;
    }
  }

  CheckErrors ("render::low_level::opengl::Texture3D::SetData");
}

void Texture3D::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  if (!buffer)
    RaiseNullArgument ("render::low_level::opengl::Texture3D::GetData", "buffer");
  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::GetData", "mip_level", mip_level, (size_t)0, mips_count);
  if (x)
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::GetData", "x", x, (size_t)0, (size_t)0);
  if (y)
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::GetData", "y", y, (size_t)0, (size_t)0);
  if (width != (desc.width >> mip_level))
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::GetData", "width", width, desc.width >> mip_level, desc.width >> mip_level);
  if (height != (desc.height >> mip_level))
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::GetData", "height", height, desc.height >> mip_level, desc.height >> mip_level);
  if (is_compressed_format (target_format))
    if (target_format != desc.format)
      RaiseInvalidArgument ("render::low_level::opengl::Texture3D::GetData", "target_format", target_format, "Can't get compressed texture data, format is different.");

  MakeContextCurrent ();
  Bind ();

  TextureExtensions ext (GetContextManager ());

  char*  temp_buffer;
  size_t layer_size;
  
  if (is_compressed_format (target_format) && ext.has_ext_texture_compression_s3tc)
    layer_size = ((width * height) >> 4) * compressed_quad_size (target_format);
  else
  {
    if (texel_size (target_format) == 3)
      layer_size = width * height * 4;
    else
      layer_size = width * height * texel_size (target_format);
  }
  temp_buffer = new char [layer_size * desc.layers];

  if (is_compressed_format (target_format))
  {
    if (ext.has_ext_texture_compression_s3tc)
      glGetCompressedTexImage (GL_TEXTURE_3D_EXT, mip_level, temp_buffer);
    else
      glGetTexImage (GL_TEXTURE_3D_EXT, mip_level, unpack_format (target_format), unpack_type (target_format), temp_buffer);
  }
  else
    glGetTexImage (GL_TEXTURE_3D_EXT, mip_level, gl_format (target_format), gl_type (target_format), temp_buffer);
  
  if (is_compressed_format (target_format) && !ext.has_ext_texture_compression_s3tc)
    pack_dxt (target_format, width, height, temp_buffer + layer_size * layer, buffer);
  else
    memcpy (buffer, temp_buffer + layer_size * layer, layer_size);
  
  delete [] temp_buffer;  

  CheckErrors ("render::low_level::opengl::Texture3D::GetData");
}
