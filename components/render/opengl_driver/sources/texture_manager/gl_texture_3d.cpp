#ifndef OPENGL_ES_SUPPORT

#include "shared.h"

using namespace common;
using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture3D::Texture3D  (const ContextManager& manager, const TextureDesc& tex_desc, const TextureData* data)
  : Texture (manager, tex_desc, GL_TEXTURE_3D_EXT, get_mips_count (tex_desc.width, tex_desc.height, tex_desc.layers))
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture3D::Texture3D";
  
  if (data)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Texture initial data not supported");      

    //выбор текущего контекста и биндинг текстуры

  MakeContextCurrent ();
  
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
      throw xtl::format_not_supported_exception (METHOD_NAME, "3D compressed textures not supported (desc.format=%s)", get_name (GetFormat ()));
      return;
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_S8:    
      throw xtl::format_not_supported_exception (METHOD_NAME, "3D depth-stencil textures not supported (desc.format=%s)", get_name (GetFormat ()));
      return;
    default:
      throw xtl::make_argument_exception (METHOD_NAME, "desc.format", GetFormat ());
      return;
  }
  
    //преобразование формата текстуры
    
  GLenum gl_internal_format = get_gl_internal_format (GetFormat ()),
         gl_format          = get_gl_format (GetFormat ()),
         gl_type            = get_gl_type (GetFormat ());  

  if (glTexImage3D)
  {
    glTexImage3D (GL_PROXY_TEXTURE_3D_EXT, 1, gl_internal_format, tex_desc.width, tex_desc.height, tex_desc.layers, 0, gl_format, gl_type, 0);
  }
  else
  {
    glTexImage3DEXT (GL_PROXY_TEXTURE_3D_EXT, 1, gl_internal_format, tex_desc.width, tex_desc.height, tex_desc.layers, 0, gl_format, gl_type, 0);
  }

  GLint width = 0;

  glGetTexLevelParameteriv (GL_PROXY_TEXTURE_3D_EXT, 1, GL_TEXTURE_WIDTH, &width);

  if (!width)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Can't create 3d texture %ux%ux%u@%s (proxy texture fail)", 
                       tex_desc.width, tex_desc.height, tex_desc.layers, get_name (tex_desc.format));

    //создание mip-уровней

  size_t depth = tex_desc.layers;

  for (size_t mip_level=0; mip_level<GetMipsCount (); mip_level++)
  {
    MipLevelDesc level_desc;

    GetMipLevelDesc (mip_level, level_desc);        

    if (glTexImage3D)
    {
      glTexImage3D (GL_TEXTURE_3D_EXT, mip_level, gl_internal_format, level_desc.width, level_desc.height, depth, 0, gl_format, gl_type, 0);
    }
    else
    {
      glTexImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, gl_internal_format, level_desc.width, level_desc.height, depth, 0, gl_format, gl_type, 0);
    }

    if (depth > 1) depth /= 2;
  }

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

/*
    Работа с данными
      данная перегрузка обсуловлена необходимостью обхода некорректной работы функции glGetTexImage в драйверах ATI
*/

void Texture3D::GetData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  PixelFormat target_format,
  void*       buffer)
{  
    /*
        Функция всегда проходит через "ручное" копирование, поскольку определить баг на старых версиях драйверов ATI невозможно
        из-за crash при вызове glGetTexImage
    */

/*
    //если баг работы функции glGetTexImage не обнаружен - копирование производится как и в общем случае

  static Extension BUG_Texture3D_GetTexImage = "GLBUG_texture3D_get_tex_image";

  if (!IsSupported (BUG_Texture3D_GetTexImage))
  {
    Texture::GetData (layer, mip_level, x, y, width, height, target_format, buffer);
    return;
  }

*/

    //"ручное" копирование из внутреннего формата текстуры в требуемый

  if (!width || !height)
    return;  
    
  switch (target_format)
  {
    case PixelFormat_RGB8:
    {
      PixelFormat tmp_format = GetFormat () == PixelFormat_RGB8 ? PixelFormat_RGBA8 : GetFormat ();

      xtl::uninitialized_storage<char> tmp_buffer (get_image_size (width, height, tmp_format));

      Texture::GetData (layer, mip_level, x, y, width, height, tmp_format, tmp_buffer.data ());

      copy_image (width * height, tmp_format, tmp_buffer.data (), PixelFormat_RGB8, buffer);

      break;
    }
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_RGBA8:
      if (target_format != GetFormat ())
      {
        xtl::uninitialized_storage<char> tmp_buffer (get_image_size (width, height, GetFormat ()));

        Texture3D::GetData (layer, mip_level, x, y, width, height, GetFormat (), tmp_buffer.data ());

        copy_image (width * height, GetFormat (), tmp_buffer.data (), target_format, buffer);        

        break;
      }

        //no break
    default:
      Texture::GetData (layer, mip_level, x, y, width, height, target_format, buffer);
      break;
  }
}

/*
    Установка данных
*/

void Texture3D::SetUncompressedData
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
  if (glTexSubImage3D) glTexSubImage3D    (GL_TEXTURE_3D, mip_level, x, y, layer, width, height, 1, format, type, buffer);
  else                 glTexSubImage3DEXT (GL_TEXTURE_3D_EXT, mip_level, x, y, layer, width, height, 1, format, type, buffer);
}

void Texture3D::SetCompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, size_t, const void*)
{
  throw xtl::format_not_supported_exception ("render::low_level::opengl::Texture3D::SetCompressedData", "Compression for 3D textures not supported");
}

#endif
