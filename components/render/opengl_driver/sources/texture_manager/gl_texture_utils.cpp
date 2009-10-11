#include "shared.h"

using namespace common;

/*
   Получение параметров текстуры
*/

namespace render
{

namespace low_level
{

namespace opengl
{

#ifndef OPENGL_ES_SUPPORT

//преобразование формата хранения пикселей к внутреннему формату OpenGL
GLint get_gl_internal_format (PixelFormat format)
{
  static const char* METHOD_NAME = "render::low_level::opengl::get_gl_internal_format";

  switch (format)
  {
    case PixelFormat_L8:          return GL_LUMINANCE8;
    case PixelFormat_A8:          return GL_ALPHA8;
    case PixelFormat_LA8:         return GL_LUMINANCE8_ALPHA8;
    case PixelFormat_RGB8:        return GL_RGB8;
    case PixelFormat_RGBA8:       return GL_RGBA8;
    case PixelFormat_DXT1:        return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    case PixelFormat_DXT3:        return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case PixelFormat_DXT5:        return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case PixelFormat_D16:         return GL_DEPTH_COMPONENT16_ARB;
    case PixelFormat_D24X8:       return GL_DEPTH_COMPONENT24_ARB;
    case PixelFormat_D24S8:       return GL_DEPTH24_STENCIL8_EXT;
    case PixelFormat_S8:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Stencil textures not supported");
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:
      throw xtl::format_not_supported_exception (METHOD_NAME, "PVRTC textures not supported");
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format", format);
  }
}

//преобразование формата хранения пикселей к формату буферов OpenGL
GLenum get_gl_format (PixelFormat format)
{
  static const char* METHOD_NAME = "render::low_level::opengl::get_gl_format";

  switch (format)
  {
    case PixelFormat_DXT1:  return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    case PixelFormat_DXT3:  return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case PixelFormat_DXT5:  return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case PixelFormat_L8:    return GL_LUMINANCE;
    case PixelFormat_A8:    return GL_ALPHA;
    case PixelFormat_LA8:   return GL_LUMINANCE_ALPHA;
    case PixelFormat_RGB8:  return GL_RGB;
    case PixelFormat_RGBA8: return GL_RGBA;
    case PixelFormat_D16:
    case PixelFormat_D24X8: return GL_DEPTH_COMPONENT;
    case PixelFormat_D24S8: return GL_DEPTH_STENCIL_EXT;
    case PixelFormat_S8:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Stencil textures not supported");
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:
      throw xtl::format_not_supported_exception (METHOD_NAME, "PVRTC textures not supported");
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format", format);
  }
}

//преобразование формата хранения пикселей к типу элементов в буферах OpenGL
GLenum get_gl_type (PixelFormat format)
{
  static const char* METHOD_NAME = "render::low_level::opengl::get_gl_type";

  switch (format)
  {
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8: 
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:   return GL_UNSIGNED_BYTE;
    case PixelFormat_D16:    return GL_UNSIGNED_SHORT;
    case PixelFormat_D24X8:  return GL_UNSIGNED_INT;
    case PixelFormat_D24S8:  return GL_UNSIGNED_INT_24_8_EXT;
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:
      throw xtl::format_not_supported_exception (METHOD_NAME, "PVRTC textures not supported");
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format", format);
  }
}

//получение формата хранения пикселей по GL-формату
PixelFormat get_pixel_format (GLenum gl_format)
{
  switch (gl_format)
  {
    case GL_RGB8:                          return PixelFormat_RGB8;
    case GL_RGBA8:                         return PixelFormat_RGBA8;
    case GL_ALPHA8:                        return PixelFormat_A8;
    case GL_LUMINANCE8:                    return PixelFormat_L8;
    case GL_LUMINANCE8_ALPHA8:             return PixelFormat_LA8;
    case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:  return PixelFormat_DXT1;
    case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT: return PixelFormat_DXT3;
    case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT: return PixelFormat_DXT5;    
    case GL_DEPTH_COMPONENT16:             return PixelFormat_D16;
    case GL_DEPTH_COMPONENT24:             return PixelFormat_D24X8;
    case GL_DEPTH24_STENCIL8_EXT:          return PixelFormat_D24S8;
    default:
      throw xtl::format_not_supported_exception ("render::low_level::get_pixel_format", "Unknown gl_format=%04x", gl_format);
  }
}

#else //OPENGL_ES_SUPPORT

//преобразование формата хранения пикселей к внутреннему формату OpenGL
GLint get_gl_internal_format (PixelFormat format)
{
  static const char* METHOD_NAME = "render::low_level::opengl::get_gl_internal_format";

  switch (format)
  {
    case PixelFormat_L8:          return GL_LUMINANCE;
    case PixelFormat_A8:          return GL_ALPHA;
    case PixelFormat_LA8:         return GL_LUMINANCE_ALPHA;
    case PixelFormat_RGB8:        return GL_RGB;
    case PixelFormat_RGBA8:       return GL_RGBA;
    case PixelFormat_RGB_PVRTC2:  return GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
    case PixelFormat_RGB_PVRTC4:  return GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG;    
    case PixelFormat_RGBA_PVRTC2: return GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
    case PixelFormat_RGBA_PVRTC4: return GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
    case PixelFormat_D16:         return GL_DEPTH_COMPONENT16_OES;
    case PixelFormat_D24X8:       return GL_DEPTH_COMPONENT24_OES;
    case PixelFormat_D24S8:       return GL_DEPTH_STENCIL_OES; 
    case PixelFormat_S8:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Stencil textures not supported.");
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format", format);
  }
}

//преобразование формата хранения пикселей к формату буферов OpenGL
GLenum get_gl_format (PixelFormat format)
{
  static const char* METHOD_NAME = "render::low_level::opengl::get_gl_format";

  switch (format)
  {
    case PixelFormat_L8:          return GL_LUMINANCE;
    case PixelFormat_A8:          return GL_ALPHA;
    case PixelFormat_LA8:         return GL_LUMINANCE_ALPHA;
    case PixelFormat_RGB8:        return GL_RGB;
    case PixelFormat_RGBA8:       return GL_RGBA;
    case PixelFormat_D16:
    case PixelFormat_D24X8:       return GL_DEPTH_COMPONENT24_OES;
    case PixelFormat_D24S8:       return GL_DEPTH_STENCIL_OES;
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4: return 0;
    case PixelFormat_S8:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Stencil textures not supported");
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      throw xtl::format_not_supported_exception (METHOD_NAME, "DXT textures not supported");
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format", format);
  }
}

//преобразование формата хранения пикселей к типу элементов в буферах OpenGL
GLenum get_gl_type (PixelFormat format)
{
  static const char* METHOD_NAME = "render::low_level::opengl::get_gl_type";

  switch (format)
  {
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8: 
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:  return GL_UNSIGNED_BYTE;
    case PixelFormat_D16:   return GL_UNSIGNED_SHORT;
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
      throw xtl::format_not_supported_exception (METHOD_NAME, "Depth24 component not supported");
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "format", format);
  }
}

//получение формата хранения пикселей по GL-формату
PixelFormat get_pixel_format (GLenum gl_format)
{
  switch (gl_format)
  {
    case GL_RGB:                              return PixelFormat_RGB8;
    case GL_RGBA:                             return PixelFormat_RGBA8;
    case GL_ALPHA:                            return PixelFormat_A8;
    case GL_LUMINANCE:                        return PixelFormat_L8;
    case GL_LUMINANCE_ALPHA:                  return PixelFormat_LA8;
    case GL_DEPTH_COMPONENT16_OES:            return PixelFormat_D16;
    case GL_DEPTH_COMPONENT24_OES:            return PixelFormat_D24X8;
    case GL_DEPTH_STENCIL_OES:                return PixelFormat_D24S8;
    case GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG:  return PixelFormat_RGB_PVRTC2;
    case GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG:  return PixelFormat_RGB_PVRTC4;
    case GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG: return PixelFormat_RGBA_PVRTC2;
    case GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG: return PixelFormat_RGBA_PVRTC4;
    default:
      throw xtl::format_not_supported_exception ("render::low_level::get_pixel_format", "Unknown gl_format=%04x", gl_format);
  }
}

#endif

//распакованный внутренний формат OpenGL
GLenum get_uncompressed_gl_internal_format (PixelFormat format)
{
  return get_gl_internal_format (get_uncompressed_format (format));  
}

//распакованный формат OpenGL
GLenum get_uncompressed_gl_format (PixelFormat format)
{
  return get_gl_format (get_uncompressed_format (format));
}

//распакованный тип OpenGL
GLenum get_uncompressed_gl_type (PixelFormat format)
{
  return get_gl_type (get_uncompressed_format (format));
}

//получение ближайшей сверху степени двойки
size_t get_next_higher_power_of_two (size_t k) 
{
  if (!k)
    return 1;

  k--;

  for (size_t i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

}

}

}
