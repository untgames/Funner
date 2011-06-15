#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   ����������� / ����������
*/

Texture2D::Texture2D (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data, bool ignore_null_data)
  : Texture (manager, tex_desc, GL_TEXTURE_2D, get_mips_count (tex_desc.width, tex_desc.height))
{
  const char* METHOD_NAME = "render::low_level::opengl::Texture2D::Texture2D";  

    //��������� �������� � �������� OpenGL

  Bind ();

     //�������������� ������� ��������

  GLenum gl_uncompressed_format = get_uncompressed_gl_format (tex_desc.format),
         gl_uncompressed_type   = get_uncompressed_gl_type (tex_desc.format),
         gl_internal_format;

  switch (tex_desc.format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      gl_internal_format = GetCaps ().has_ext_texture_compression_s3tc ? get_gl_internal_format (tex_desc.format) : get_uncompressed_gl_internal_format (tex_desc.format);
      break;
    case PixelFormat_RGB_PVRTC2:
    case PixelFormat_RGB_PVRTC4:
    case PixelFormat_RGBA_PVRTC2:
    case PixelFormat_RGBA_PVRTC4:
      if (!GetCaps ().has_img_texture_compression_pvrtc)
        throw xtl::format_not_supported_exception (METHOD_NAME, "PVRTC texture compresson not supported");

    default:
      gl_internal_format = get_gl_internal_format (tex_desc.format);    
      break;    
  }

#ifndef OPENGL_ES_SUPPORT

    //�������� ����������� �������� ��������

//  glTexImage2D (GL_PROXY_TEXTURE_2D, 1, gl_internal_format, tex_desc.width, tex_desc.height, 0, gl_uncompressed_format, gl_uncompressed_type, 0);

//  GLint proxy_width = 0;

//  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_2D, 1, GL_TEXTURE_WIDTH, &proxy_width);

//  if (!proxy_width)
//    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 2D texture %ux%u@%s. Reason: proxy texure fail", tex_desc.width,
//      tex_desc.height, get_name (tex_desc.format));
            
#endif

  if (data || !ignore_null_data)
  {
      //��������� ������������ ������ � ������    

    glPixelStorei (GL_UNPACK_ALIGNMENT,   1); //������������ ������ �����
    glPixelStorei (GL_UNPACK_SKIP_ROWS,   0); //���������� ������������ �����
    glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0); //���������� ������������ ��������

      //�������� mip-�������

    TextureDataSelector data_selector (tex_desc, data);
    bool                is_compressed_data = is_compressed (tex_desc.format);

    PFNGLCOMPRESSEDTEXIMAGE2DPROC glCompressedTexImage2D_fn = 0;
    
    if (is_compressed_data && GetCaps ().has_ext_texture_compression_s3tc)
      glCompressedTexImage2D_fn = GetCaps ().glCompressedTexImage2D_fn;

    xtl::uninitialized_storage<char> unpacked_buffer;      
    
    if (is_compressed_data && data && !GetCaps ().has_ext_texture_compression_s3tc)
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
        glPixelStorei (GL_UNPACK_ROW_LENGTH, level_desc.width); //����� ������ � �������� (��� �������� mip-������)
        
        if (is_compressed_data && level_data.data)
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

#ifndef OPENGL_ES_SUPPORT

      glGetTexLevelParameteriv (GL_TEXTURE_2D, i, GL_TEXTURE_INTERNAL_FORMAT, (GLint*)&gl_internal_format);

#endif
    }    

     //��������� ��������� ����������� ������� �������� �������� (������� � ���������� ������� �������)

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

    //�������� ������

  CheckErrors (METHOD_NAME);
}

/*
    ��������� ������
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
