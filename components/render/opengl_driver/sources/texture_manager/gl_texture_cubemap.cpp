#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    онструктор / деструктор
*/

TextureCubemap::TextureCubemap  (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data)
  : Texture (manager, tex_desc, GL_TEXTURE_CUBE_MAP, get_mips_count (tex_desc.width, tex_desc.height))
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureCubemap::TextureCubemap";
  
    //установка текстуры в контекст OpenGL

  Bind ();
  
    //проверка корректности дескриптора текстуры

  if (is_depth_stencil (tex_desc.format))
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create depth cubemap texture. Reason: depth texture may be only 1D or 2D");

  if (tex_desc.layers != 6)
    throw xtl::make_argument_exception (METHOD_NAME, "desc.layers", tex_desc.layers, "Cubemap texture must has desc.layers = 6");

  if (tex_desc.width != tex_desc.height)
    throw xtl::format_exception<xtl::bad_argument> (METHOD_NAME, "Cubemap texture sizes must be equal (desc.width=%u, desc.height=%u)", tex_desc.width, tex_desc.height);

     //преобразование формата пикселей

  GLenum gl_internal_format = GetCaps ().has_ext_texture_compression_s3tc ? get_gl_internal_format (tex_desc.format) :
                              get_uncompressed_gl_internal_format (tex_desc.format),
         gl_format          = get_uncompressed_gl_format (tex_desc.format),
         gl_type            = get_uncompressed_gl_type (tex_desc.format);
         
#ifndef OPENGL_ES_SUPPORT

    //проверка возможности создани€ текстуры

  glTexImage2D (GL_PROXY_TEXTURE_CUBE_MAP, 1, gl_internal_format, tex_desc.width, tex_desc.height, 0, gl_format, gl_type, 0);
  
  GLint proxy_width = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_CUBE_MAP, 1, GL_TEXTURE_WIDTH, &proxy_width);

  if (!proxy_width)
  {
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create cubemap texture %ux%u@%s. Reason: proxy texure fail",
                       tex_desc.width, tex_desc.height, get_name (tex_desc.format));
  }
    
#endif

    //создание mip-уровней
    
  for (size_t i=0; i<GetMipsCount (); i++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (i, level_desc);

    for (size_t j=0; j<6; j++)
    {
      glTexImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, gl_internal_format, level_desc.width, level_desc.height,
                    0, gl_format, gl_type, 0);
                    
#ifndef OPENGL_ES_SUPPORT

      glGetTexLevelParameteriv (GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, i, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&gl_internal_format);
      
#endif
    }
  }
  
   //установка реального внутреннего формата хранени€ пикселей (св€зано с установкой сжатого формата)
   
  try
  {
    SetFormat (get_pixel_format (gl_internal_format));
  }
  catch (xtl::exception& e)
  {
    e.touch (METHOD_NAME);

    throw;
  }

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

/*
    ѕолучение дескриптора сло€ текстуры
*/

void TextureCubemap::GetLayerDesc (size_t layer, LayerDesc& desc)
{
  if (layer > 6)
    throw xtl::make_range_exception ("render::low_level::opengl::TextureCubemap::GetLayerDesc", "layer", layer, 6);

  desc.target    = GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer;
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
  glTexSubImage2D (GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, mip_level, x, y, width, height, format, type, buffer);  
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
  GetCaps ().glCompressedTexSubImage2D_fn (GL_TEXTURE_CUBE_MAP_POSITIVE_X + layer, mip_level, x, y, width, height, format, buffer_size, buffer);
}
