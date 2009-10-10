#ifndef OPENGL_ES_SUPPORT

#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture2DNoSubimage::Texture2DNoSubimage  (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data)
  : Texture (manager, tex_desc, GL_TEXTURE_2D,
    get_mips_count (tex_desc.width, tex_desc.height))
{
  const char* METHOD_NAME = "render::low_level::opengl::Texture2DNoSubimage::Texture2DNoSubimage";

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
      throw xtl::format_not_supported_exception (METHOD_NAME, "Compressed 2D textures not supported (desc.format=%s)", get_name (GetFormat ()));
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_S8:    
      throw xtl::format_not_supported_exception (METHOD_NAME, "Depth-stencil 2D textures not supported (desc.format=%s)", get_name (GetFormat ()));
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.format", GetFormat ());
  }  
  
    //преобразование формата

  gl_internal_format = get_gl_internal_format (GetFormat ());
  
  if (data)
  {
      //создание mip-уровней    

    TextureDataSelector data_selector (tex_desc, data);
    
    GLenum gl_format = get_gl_format (tex_desc.format),
           gl_type   = get_gl_type (tex_desc.format);    

    for (size_t i=0; i<GetMipsCount (); i++)
    {
      MipLevelDesc level_desc;

      GetMipLevelDesc (i, level_desc);

      glTexImage2D (GL_TEXTURE_2D, i, gl_internal_format, level_desc.width, level_desc.height, 0, gl_format, gl_type, data_selector.GetData ());

      data_selector.Next (level_desc.width, level_desc.height, 1);

#ifndef OPENGL_ES_SUPPORT

      glGetTexLevelParameteriv (GL_TEXTURE_2D, i, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&gl_internal_format);

#endif
    }

     //установка реального внутреннего формата хранениЯ пикселей (свЯзано с установкой сжатого формата)

    try
    {
      SetFormat (get_pixel_format (gl_internal_format));
    }
    catch (xtl::exception& e)
    {
      e.touch (METHOD_NAME);
      
      throw;
    }
  }

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

/*
    Установка данных
*/

void Texture2DNoSubimage::SetUncompressedData
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
  static const char* METHOD_NAME = "render::low_level::opengl::Texture2DNoSubimage::SetUncompressedData";
  
  MipLevelDesc level_desc;

  GetMipLevelDesc (mip_level, level_desc);  

  if (x || y || width != level_desc.width || height != level_desc.height)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Could not copy sub-image (x=%u, y=%u, width=%u, height=%u) "
      "in texture2D (mip_level=%u, width=%u, height=%u). This feature unsupported in texture emulation mode", x, y, width, height,
      mip_level, level_desc.width, level_desc.height);

  glTexImage2D (GL_TEXTURE_2D, mip_level, gl_internal_format, width, height, 0, format, type, buffer);
}

void Texture2DNoSubimage::SetCompressedData
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
  throw xtl::format_not_supported_exception ("render::low_level::opengl::Texture2DNoSubimage::SetCompressedData",
    "Compression not supported in texture emulation mode");
}

#endif
