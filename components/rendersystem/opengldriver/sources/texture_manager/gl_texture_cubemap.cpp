#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    онструктор / деструктор
*/

TextureCubemap::TextureCubemap  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_CUBE_MAP_ARB)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureCubemap::TextureCubemap";
  
    //установка текстуры в контекст OpenGL

  Bind ();

  TextureExtensions ext (GetContextManager ());
  
    //проверка корректности дескриптора текстуры

  if (is_depth_format (tex_desc.format))
    RaiseNotSupported (METHOD_NAME, "Can't create depth cubemap texture. Reason: depth texture may be only 1D or 2D");

  if (tex_desc.layers != 6)
    RaiseInvalidArgument (METHOD_NAME, "desc.layers", tex_desc.layers, "Cubemap texture must has desc.layers = 6");

  if (tex_desc.width != tex_desc.height)
    Raise<ArgumentException> (METHOD_NAME, "Cubemap texture sizes must be equal (desc.width=%u, desc.height=%u)", tex_desc.width, tex_desc.height);

     //преобразование формата пикселей

  GLenum gl_internal_format = ext.has_ext_texture_compression_s3tc ? opengl::gl_internal_format (tex_desc.format) : unpack_internal_format (tex_desc.format),
         gl_format          = unpack_format (tex_desc.format),
         gl_type            = unpack_type (tex_desc.format);

    //проверка возможности создани€ текстуры

  glTexImage2D (GL_PROXY_TEXTURE_CUBE_MAP, 1, gl_internal_format, tex_desc.width, tex_desc.height, 0, gl_format, gl_type, 0);
  
  GLint proxy_width = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_CUBE_MAP, 1, GL_TEXTURE_WIDTH, &proxy_width);

  if (!proxy_width)
  {
    RaiseNotSupported (METHOD_NAME, "Can't create cubemap texture %ux%u@%s. Reason: proxy texure fail",
                       tex_desc.width, tex_desc.height, get_name (tex_desc.format));
  }
  
    //создание mip-уровней
    
  for (size_t i=0; i<mips_count; i++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (i, level_desc);

    for (size_t j=0; j<6; j++)
    {
      glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, gl_internal_format, level_desc.width, level_desc.height,
                    0, gl_format, gl_type, 0);

      glGetTexLevelParameteriv (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + j, i, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&gl_internal_format);
    }
  }
  
   //установка реального внутреннего формата хранени€ пикселей (св€зано с установкой сжатого формата)
   
  try
  {
    desc.format = get_pixel_format (gl_internal_format);
  }
  catch (common::Exception& e)
  {
    e.Touch (METHOD_NAME);

    throw;
  }

    //настройка режима генерации mip-уровней

  if (tex_desc.generate_mips_enable && ext.has_sgis_generate_mipmap)
    glTexParameteri (GL_TEXTURE_CUBE_MAP_ARB, GL_GENERATE_MIPMAP_SGIS, true);

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

/*
    ѕолучение дескриптора сло€ текстуры
*/

void TextureCubemap::GetLayerDesc (size_t layer, LayerDesc& desc)
{
  if (layer > 6)
    RaiseOutOfRange ("render::low_level::opengl::TextureCubemap::GetLayerDesc", "layer", layer, 6);

  desc.target    = GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer;
  desc.new_index = 0;
}

/*
    ”становка данных
*/

void TextureCubemap::SetUncompressedData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  GLenum      format,
  GLenum      type,
  const void* buffer)
{
  glTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, x, y, width, height, format, type, buffer);  
}

void TextureCubemap::SetCompressedData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  GLenum      format,
  size_t      buffer_size,
  const void* buffer)
{
  PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC glCompressedTexSubImage2D_fn = 0;

  if (glCompressedTexSubImage2D) glCompressedTexSubImage2D_fn = glCompressedTexSubImage2D;
  else                           glCompressedTexSubImage2D_fn = glCompressedTexSubImage2DARB;

  glCompressedTexSubImage2D_fn (GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB + layer, mip_level, x, y, width, height, format, buffer_size, buffer);
}
