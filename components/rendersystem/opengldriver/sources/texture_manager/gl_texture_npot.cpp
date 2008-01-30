#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

TextureNPOT::TextureNPOT  (const ContextManager& manager, const TextureDesc& tex_desc)
  : Texture (manager, tex_desc, GL_TEXTURE_RECTANGLE_EXT)
{
  Bind ();

  glTexImage2D (GL_TEXTURE_RECTANGLE_EXT, 0, gl_internal_format (tex_desc.format), tex_desc.width, tex_desc.height, 0, 
                gl_format (tex_desc.format), gl_type (tex_desc.format), NULL);

  CheckErrors ("render::low_level::opengl::TextureNPOT::TextureNPOT");
}

/*
   Работа с данными
*/

void TextureNPOT::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  Texture::SetData (layer, mip_level, x, y, width, height, source_format, buffer);
  
  if (mip_level)
    RaiseOutOfRange ("render::low_level::opengl::TextureNPOT::SetData", "mip_level", mip_level, (size_t)0, (size_t)0);
  if ((x + width) > desc.width)
    RaiseOutOfRange ("render::low_level::opengl::TextureNPOT::SetData", "x + width", x + width, (size_t)0, desc.width);
  if ((y + height) > desc.height)
    RaiseOutOfRange ("render::low_level::opengl::TextureNPOT::SetData", "y + height", y + height, (size_t)0, desc.height);
  if (!width || !height)
    return;
  if (is_compressed_format (source_format))
    RaiseInvalidArgument ("render::low_level::opengl::TextureNPOT::SetData", "source_format");

  MakeContextCurrent ();
  Bind ();

  glTexSubImage2D (GL_TEXTURE_RECTANGLE_EXT, 0, x, y, width, height, gl_format (source_format), gl_type (source_format), buffer);

  CheckErrors ("render::low_level::opengl::TextureNPOT::SetData");
}
