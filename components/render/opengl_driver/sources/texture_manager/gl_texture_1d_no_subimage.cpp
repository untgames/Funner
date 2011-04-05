#ifndef OPENGL_ES_SUPPORT

#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
    Конструктор / деструктор
*/

Texture1DNoSubimage::Texture1DNoSubimage (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data)
  : Texture (manager, tex_desc, GL_TEXTURE_1D, get_mips_count (tex_desc.width))
{
  const char* METHOD_NAME = "render::low_level::opengl::Texture1DNoSubimage::Texture1DNoSubimage";

    //установка текстуры в контекст OpenGL

  Bind ();

    //проверка корректности формата создаваемой текстуры
    
  switch (GetFormat ())
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
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Compressed 1D textures not supported (desc.format=%s)", get_name (GetFormat ()));
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_D32:
    case PixelFormat_S8:    
      throw xtl::format_not_supported_exception (METHOD_NAME, "Depth-stencil 1D textures not supported (desc.format=%s)", get_name (GetFormat ()));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.format", GetFormat ());
  }  
    
    //преобразование формата пикселей

  if (is_compressed (tex_desc.format))
    throw xtl::format_not_supported_exception (METHOD_NAME, "1D texture can't be compressed");

  gl_internal_format = get_gl_internal_format (GetFormat ());

    //создание mip-уровней
  
  if (data)
  {
    TextureDataSelector data_selector (tex_desc, data);

    GLenum gl_format = get_gl_format (tex_desc.format),
           gl_type   = get_gl_type (tex_desc.format);

    for (size_t i=0; i<GetMipsCount (); i++)
    {
      size_t level_width = tex_desc.width >> i;

      TextureLevelData level_data;

      data_selector.GetLevelData (level_width, 1, 1, level_data);

      glTexImage1D (GL_TEXTURE_1D, i, gl_internal_format, level_width, 0, gl_format, gl_type, level_data.data);

      data_selector.Next ();
    }
  }

    //проверка ошибок

  CheckErrors (METHOD_NAME);
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
