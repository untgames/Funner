#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Конструктор / деструктор
*/

Texture1DNoSubimage::Texture1DNoSubimage (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data)
  : Texture1D (manager, tex_desc, data, true)
{
  gl_internal_format = GetCaps ().has_ext_texture_compression_s3tc ? get_gl_internal_format (GetFormat ()) : get_uncompressed_gl_internal_format (GetFormat ());  
}

/*
    Установка данных
*/

void Texture1DNoSubimage::SetUncompressedData (size_t layer, size_t mip_level, size_t x, size_t, size_t width, size_t, GLenum format, GLenum type, const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture1DNoSubimage::SetUncompressedData";
  
  MipLevelDesc level_desc;

  GetMipLevelDesc (mip_level, level_desc);

  if (x || width != level_desc.width)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Could not copy sub-image (x=%u, width=%u) "
      "in texture1D (mip_level=%u, width=%u). This feature unsupported in texture emulation mode", x, width, mip_level, level_desc.width);

  glTexImage1D (GL_TEXTURE_1D, mip_level, gl_internal_format, width, 0, format, type, buffer);
}

void Texture1DNoSubimage::SetCompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, size_t, const void*)
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::Texture1DNoSubimage::SetCompressedData", "Compression for 1D textures not supported");
}

#endif
