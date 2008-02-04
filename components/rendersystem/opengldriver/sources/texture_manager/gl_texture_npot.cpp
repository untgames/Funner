#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    онструктор / деструктор
*/

TextureNPOT::TextureNPOT  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_RECTANGLE_EXT)
{
  static const char* METHOD_NAME = "render::low_level::opengl::TextureNPOT::TextureNPOT";

  if (tex_desc.generate_mips_enable)
    RaiseNotSupported (METHOD_NAME, "Mip maps for non power of two textures not supported.");

  if (is_compressed_format (tex_desc.format))
    RaiseNotSupported (METHOD_NAME, "Non power of two texture can't be compressed.");

  Bind ();

    //преобразование формата текстуры
    
  GLenum gl_internal_format = opengl::gl_internal_format (desc.format),
         gl_format          = opengl::gl_format (desc.format),
         gl_type            = opengl::gl_type (desc.format);  

  glTexImage2D (GL_PROXY_TEXTURE_RECTANGLE_EXT, 0, gl_internal_format, tex_desc.width, tex_desc.height, 0, 
                gl_format, gl_type, NULL);
  
  GLint width = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_RECTANGLE_EXT, 1, GL_TEXTURE_WIDTH, &width);

  if (!width)
    RaiseNotSupported (METHOD_NAME, "Can't create rectangle texture %ux%ux%u@%s (proxy texture fail)", 
                       tex_desc.width, tex_desc.height, tex_desc.layers, get_name (tex_desc.format));
  
  glTexImage2D (GL_TEXTURE_RECTANGLE_EXT, 0, gl_internal_format, tex_desc.width, tex_desc.height, 0, 
                gl_format, gl_type, NULL);

  CheckErrors (METHOD_NAME);
}

/*
    ”становка данных
*/

void TextureNPOT::SetUncompressedData
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
  if (mip_level)
    RaiseNotSupported ("render::low_level::opengl::TextureNPOT::SetUncompressedData",
      "Mip-mapping not supported for rectangle-textures (mip_level=%u)", mip_level);

  glTexSubImage2D (GL_TEXTURE_RECTANGLE_EXT, 0, x, y, width, height, format, type, buffer);
}

void TextureNPOT::SetCompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, size_t, const void*)
{
  RaiseNotSupported ("render::low_level::opengl::TextureNPOT::SetCompressedData", "Compression for NPOT textures not supported");
}
