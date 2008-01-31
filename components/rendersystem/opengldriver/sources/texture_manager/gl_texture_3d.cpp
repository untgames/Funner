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
  static const char* METHOD_NAME = "render::low_level::opengl::Texture3D::Texture3D";

    //выбор текущего контекста и биндинг текстуры

  MakeContextCurrent ();
  
  Bind ();
  
    //проверка корректности формата создаваемой текстуры
    
  switch (desc.format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
      break;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      RaiseNotSupported (METHOD_NAME, "3D compressed textures not supported (desc.format=%s)", get_name (desc.format));
      return;
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_S8:    
      RaiseNotSupported (METHOD_NAME, "3D depth-stencil textures not supported (desc.format=%s)", get_name (desc.format));
      return;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
      return;
  }
  
    //преобразование формата текстуры
    
  GLenum gl_internal_format = opengl::gl_internal_format (desc.format),
         gl_format          = opengl::gl_format (desc.format),
         gl_type            = opengl::gl_type (desc.format);
  
    //проверка возможности создания текстуры
    
      //??????????????1 и mip-уровни

  if (glTexImage3D)
  {
    glTexImage3D (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format, desc.width, desc.height, desc.layers, 0, gl_format, gl_type, 0);
  }
  else
  {
    glTexImage3DEXT (GL_PROXY_TEXTURE_3D_EXT, 0, gl_internal_format, desc.width, desc.height, desc.layers, 0, gl_format, gl_type, 0);
  }

  size_t proxy_width = 0, proxy_height = 0, proxy_depth = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_WIDTH, (int*)&proxy_width);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_HEIGHT, (int*)&proxy_height);
  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 0, GL_TEXTURE_DEPTH, (int*)&proxy_depth);

  if (!proxy_width || !proxy_height || !proxy_depth)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create 3D texture %ux%ux%x@%s (proxy texture fail)", desc.width, desc.height, desc.layers,
                       get_name (desc.format));
  }

    //создание mip-уровней

  size_t depth = desc.layers;  

  for (size_t mip_level=0; mip_level<mips_count; mip_level++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (mip_level, level_desc);    

    if (glTexImage3D)
    {
      glTexImage3D (GL_TEXTURE_3D_EXT, mip_level, gl_internal_format, level_desc.width, level_desc.height, depth, 0, gl_format, gl_type, 0);
    }
    else
    {
      glTexImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, gl_internal_format, level_desc.width, level_desc.height, depth, 0, gl_format, gl_type, 0);
    }
  }
  
    //включение автоматической генерации !!!!вынести в Texture::Texture!!!!
    
  TextureExtensions ext (GetContextManager ());//?????

  if (desc.generate_mips_enable && ext.has_sgis_generate_mipmap)
    glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

/*
   Работа с данными
*/

namespace
{

void SetTexData3D (size_t mip_level, size_t x, size_t y, size_t z, size_t width, size_t height, GLenum format, GLenum type, const void* data)
{
  if (glTexSubImage3D) glTexSubImage3D    (GL_TEXTURE_3D, mip_level, x, y, z, width, height, 1, format, type, data);
  else                 glTexSubImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, x, y, z, width, height, 1, format, type, data);
}

}

void Texture3D::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture3D::SetData";
  static Extension  EXT_texture3D = "GL_EXT_texture3D";

  Texture::SetData (layer, mip_level, x, y, width, height, source_format, buffer);
  
  if (mip_level > mips_count)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, (size_t)0, mips_count);
//  if (layer > desc.layers)

  size_t layers = desc.layers >> mip_level;
  
  if (!layers)
    layers = 1;

  if (layer >= layers)
    RaiseOutOfRange (METHOD_NAME, "layer", layer, layers);
    
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
    if (ext.has_ext_texture_compression_s3tc) //проверить!!!!!!
    {
      if (glCompressedTexSubImage3D)
      {
        glCompressedTexSubImage3D (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, gl_format (source_format), 
                                      ((width * height) >> 4) * compressed_quad_size (source_format), buffer);
      }
      else
      {
        glCompressedTexSubImage3DARB (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, gl_format (source_format),
                                      ((width * height) >> 4) * compressed_quad_size (source_format), buffer);
      }
    }
    else
    {
      xtl::uninitialized_storage <char> unpacked_buffer (width * height * unpack_texel_size (source_format));

      unpack_dxt (source_format, width, height, buffer, unpacked_buffer.data ());
      
      if (glTexSubImage3D)
      {
        glTexSubImage3D (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, unpack_format (source_format),
                         unpack_type (source_format), unpacked_buffer.data ());
      }
      else
      {
        glTexSubImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, unpack_format (source_format),
                            unpack_type (source_format), unpacked_buffer.data ());
      }
    }
  }
  else
  {
    if (mip_level && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, false);             
      
    if (glTexSubImage3D)
    {
      glTexSubImage3D (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, gl_format (source_format), gl_type (source_format), buffer);
    }
    else
    {
      glTexSubImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, gl_format (source_format), gl_type (source_format), buffer);
    }
    
    if (mip_level && ext.has_sgis_generate_mipmap)
      glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, true);

    if (desc.generate_mips_enable && !mip_level && !ext.has_sgis_generate_mipmap)
    {
      generate_mips (x, y, layer, width, height, source_format, buffer, &SetTexData3D);
    }
  }

  CheckErrors (METHOD_NAME);
}
