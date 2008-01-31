#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture3D::Texture3D  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_3D_EXT)
{
  TextureExtensions ext (GetContextManager ());
  
    //MakeContextCurrent???

  Bind ();
  
  int iformat = -1, width = -1;
  
   glCompressedTexImage3D (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, tex_desc.layers,
                                    0, ((tex_desc.width * tex_desc.height) >> 4) * compressed_quad_size (tex_desc.format), 0);

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_INTERNAL_FORMAT, &iformat);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_WIDTH, &width);
  
  printf ("Proxy3D: iformat=%04x width =%u \n",iformat, width);            
  
  size_t depth = desc.layers;  

  for (size_t i = 0; i < mips_count; i++)
  {
    MipLevelDesc level_desc;
    
    GetMipLevelDesc (i, level_desc);
    
    printf ("Level %u, depth = %u, width = %u, height = %u\n", i, depth, level_desc.width, level_desc.height);
    
    if (is_compressed_format (tex_desc.format) && ext.has_ext_texture_compression_s3tc)
    {
      size_t tex_size = depth * level_desc.width * level_desc.height / 16 * compressed_quad_size (tex_desc.format); //???dup
     
      glCompressedTexImage3D (GL_TEXTURE_3D_EXT, i, gl_internal_format (tex_desc.format),
                              level_desc.width, level_desc.height, depth, 0, tex_size, 0);                              
    }
    else
    {
      glTexImage3DEXT (GL_TEXTURE_3D_EXT, i, unpack_internal_format (tex_desc.format), level_desc.width,
                    level_desc.height, depth, 0, unpack_format (tex_desc.format), unpack_type (tex_desc.format), 0);
    }
    
      int iformat = -1;
      
      glGetTexLevelParameteriv (GL_TEXTURE_3D_EXT, i, GL_TEXTURE_INTERNAL_FORMAT, &iformat);
      
      printf ("Constructor: mip_level=%d wanted format=%04x iformat=%04x\n", i, gl_internal_format (tex_desc.format), iformat);          

//    if (depth > 1) depth /= 2; //???

//    glTexImage3DEXT (GL_TEXTURE_3D_EXT, i, gl_internal_format, width, height, tex_desc.layers, 0, 
//                     gl_format, gl_type, NULL);

  }

  if (tex_desc.generate_mips_enable && ext.has_sgis_generate_mipmap)
    glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, true);

  CheckErrors ("render::low_level::opengl::Texture3D::Texture3D");
}

/*
   Работа с данными
*/

namespace
{

void SetTexData3D (size_t mip_level, size_t x, size_t y, size_t z, size_t width, size_t height, GLenum format, GLenum type, const void* data)
{
  glTexSubImage3D (GL_TEXTURE_3D, mip_level, x, y, z, width, height, 1, format, type, data);

    //проверка расширений!!!!!
//  if (glTexSubImage3D)         glTexSubImage3D    (GL_TEXTURE_3D, mip_level, x, y, z, width, height, 1, format, type, data);
//  else if (glTexSubImage3DEXT) glTexSubImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, x, y, z, width, height, 1, format, type, data);
     //????....
}

}

void Texture3D::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  Texture::SetData (layer, mip_level, x, y, width, height, source_format, buffer);
  
  if (mip_level > mips_count)
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::SetData", "mip_level", mip_level, (size_t)0, mips_count);
//  if (layer > desc.layers)

  size_t layers = desc.layers >> mip_level;
  
  if (!layers)
    layers = 1;

  if (layer >= layers)
    RaiseOutOfRange ("render::low_level::opengl::Texture3D::SetData", "layer", layer, layers);
    
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
    {
      int iformat = -1;
      
      glGetTexLevelParameteriv (GL_TEXTURE_3D, mip_level, GL_TEXTURE_INTERNAL_FORMAT, &iformat);
      
      printf ("???? mip_level=%d iformat=%04x\n", mip_level, iformat);      
      
      glCompressedTexSubImage3D (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, gl_format (source_format), 
                                 ((width * height) >> 4) * compressed_quad_size (source_format), buffer);
    
      printf ("\nat set data: layer = %u, mip = %u\n", layer, mip_level);
      printf ("buffer [1]: %02x\n", ((char*)buffer)[1]&0xff);
    }
    else
    {
      xtl::uninitialized_storage <char> unpacked_buffer (width * height * unpack_texel_size (source_format));

      unpack_dxt (source_format, width, height, buffer, unpacked_buffer.data ());
      glTexSubImage3D (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, unpack_format (source_format), unpack_type (source_format), unpacked_buffer.data ());
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
      generate_mips (x, y, layer, width, height, source_format, buffer, &SetTexData3D);
    }
  }

  CheckErrors ("render::low_level::opengl::Texture3D::SetData");
}
