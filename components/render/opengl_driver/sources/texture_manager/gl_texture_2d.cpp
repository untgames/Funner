#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture2D::Texture2D (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data, bool ignore_null_data)
  : Texture (manager, tex_desc, GL_TEXTURE_2D, get_mips_count (tex_desc.width, tex_desc.height))
{
  const char* METHOD_NAME = "render::low_level::opengl::Texture2D::Texture2D";  

    //установка текстуры в контекст OpenGL

  Bind ();

     //преобразование формата пикселей

  GLenum gl_uncompressed_format = get_uncompressed_gl_format (tex_desc.format),
         gl_uncompressed_type   = get_uncompressed_gl_type (tex_desc.format),
         gl_internal_format;

  switch (tex_desc.format)
  {
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:
      if (!GetCaps ().has_img_texture_compression_pvrtc)
        throw xtl::format_not_supported_exception (METHOD_NAME, "PVRTC texture compresson not supported");
      break;
    case PixelFormat_ATC_RGB_AMD:
    case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:
    case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
      if (!GetCaps ().has_amd_compressed_atc_texture)
        throw xtl::format_not_supported_exception (METHOD_NAME, "AMD ATC texture compression not supported");
      break;
    default:
      break;
  }

  switch (tex_desc.format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      gl_internal_format = GetCaps ().has_ext_texture_compression_s3tc ? get_gl_internal_format (tex_desc.format) : get_uncompressed_gl_internal_format (tex_desc.format);
      break;
    default:
      gl_internal_format = get_gl_internal_format (tex_desc.format);    
      break;    
  }

  if (data || !ignore_null_data)
  {    
      //настройка расположения данных в буфере    

    glPixelStorei (GL_UNPACK_ALIGNMENT, 1); //выравнивание начала строк

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

    glPixelStorei (GL_UNPACK_SKIP_ROWS,   0); //количество пропускаемых строк
    glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0); //количество пропускаемых пикселей

#endif

      //создание mip-уровней

    TextureDataSelector data_selector (tex_desc, data);
    bool                is_compressed_data = is_compressed (tex_desc.format);

    PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D_fn = 0;

    bool supports_compressed = false, is_dxt = false;

    if (is_compressed_data)
    {
      switch (tex_desc.format) {
        case PixelFormat_DXT1:
        case PixelFormat_DXT3:
        case PixelFormat_DXT5:
          supports_compressed = GetCaps ().has_ext_texture_compression_s3tc;
          is_dxt              = true;
          break;
        case PixelFormat_RGB_PVRTC2:  //??? Эта проверка есть выше?
        case PixelFormat_RGB_PVRTC4:
        case PixelFormat_RGBA_PVRTC2:
        case PixelFormat_RGBA_PVRTC4:
          if (!GetCaps ().has_img_texture_compression_pvrtc)
            throw xtl::format_not_supported_exception (METHOD_NAME, "PVRTC texture compression not supported");          
          
          supports_compressed = true;
            
          break;
        case PixelFormat_ATC_RGB_AMD:
        case PixelFormat_ATC_RGBA_EXPLICIT_ALPHA_AMD:
        case PixelFormat_ATC_RGBA_INTERPOLATED_ALPHA_AMD:
          if (!GetCaps ().has_amd_compressed_atc_texture)
            throw xtl::format_not_supported_exception (METHOD_NAME, "AMD ATC texture compression not supported");

          supports_compressed = true;

          break;
        default:
          throw xtl::format_operation_exception (METHOD_NAME, "Unknown compressed texture format %d", tex_desc.format);
      }

      if (supports_compressed)
        glCompressedTexImage2D_fn = GetCaps ().glCompressedTexImage2D_fn;
    }
    
    xtl::uninitialized_storage<char> unpacked_buffer;      
    
    if (is_compressed_data && data && !supports_compressed && is_dxt)
    {
      unpacked_buffer.resize (get_uncompressed_image_size (tex_desc.width, tex_desc.height, tex_desc.format));  
    }

    size_t mips_count = GetMipsCount ();    

    for (size_t i=0; i<mips_count; i++)
    {
      MipLevelDesc level_desc;

      GetMipLevelDesc (i, level_desc);

      TextureLevelData level_data;    

      if (data_selector.GetLevelData (level_desc.width, level_desc.height, 1, level_data) && glCompressedTexImage2D_fn && is_compressed_data)
      {
        glCompressedTexImage2D_fn (GL_TEXTURE_2D, i, gl_internal_format, level_desc.width, level_desc.height, 0, level_data.size, level_data.data);
      }
      else
      {
#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

        glPixelStorei (GL_UNPACK_ROW_LENGTH, level_desc.width); //длина строки в пикселях (для нулевого mip-уровня)
        
#endif

        if (is_compressed_data && level_data.data && is_dxt)
        {
          unpack_dxt (tex_desc.format, level_desc.width, level_desc.height, level_data.data, unpacked_buffer.data ());

          glTexImage2D (GL_TEXTURE_2D, i, gl_internal_format, level_desc.width, level_desc.height, 0, gl_uncompressed_format, gl_uncompressed_type, unpacked_buffer.data ());
        }
        else
        {
          glTexImage2D (GL_TEXTURE_2D, i, gl_internal_format, level_desc.width, level_desc.height, 0, gl_uncompressed_format, gl_uncompressed_type, level_data.data);
        }      
      }

      data_selector.Next ();

#if !defined(OPENGL_ES_SUPPORT) && !defined(OPENGL_ES2_SUPPORT)

      glGetTexLevelParameteriv (GL_TEXTURE_2D, i, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&gl_internal_format);

#endif
    }    

     //установка реального внутреннего формата хранения пикселей (связано с установкой сжатого формата)

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

void Texture2D::SetUncompressedData
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
  glTexSubImage2D (GL_TEXTURE_2D, mip_level, x, y, width, height, format, type, buffer);
}

void Texture2D::SetCompressedData
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
  GetCaps ().glCompressedTexSubImage2D_fn (GL_TEXTURE_2D, mip_level, x, y, width, height, format, buffer_size, buffer);
}
