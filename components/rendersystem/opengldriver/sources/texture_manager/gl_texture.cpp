#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Константы
*/

const size_t DXT_EDGE_SIZE  = 4;                             //размер стороны блока DXT-сжатия
const size_t DXT_BLOCK_SIZE = DXT_EDGE_SIZE * DXT_EDGE_SIZE; //размер блока DXT-сжатия

/*
   Конструктор / деструктор
*/

Texture::Texture
 (const ContextManager& in_context_manager,
  const ExtensionsPtr&  in_extensions,
  const TextureDesc&    in_desc,
  GLenum                in_target,
  size_t                in_mips_count)
    : BindableTexture (in_context_manager),
      desc (in_desc),
      texture_id (0),
      target (in_target),
      mips_count (in_mips_count),
      binded_sampler_id (0),
      extensions (in_extensions)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::Texture";   

    //проверка корректности формата

  switch (desc.format)
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
        //проверка возможности работы совместно с mip-mapping
        
      if (desc.generate_mips_enable)
        RaiseInvalidOperation (METHOD_NAME, "Auto-generate mipmaps incompatible with compressed textures (desc.format=%s)",
                               get_name (desc.format));

        //проверка корректности начальных размеров

      if (mips_count < 3)
      {
        if (desc.width % 4)  RaiseInvalidArgument (METHOD_NAME, "desc.width", desc.width, "Reason: width is not multiple of 4");
        if (desc.height % 4) RaiseInvalidArgument (METHOD_NAME, "desc.height", desc.height, "Reason: height is not multiple of 4");
      }

      mips_count -= 2;

      break;
    case PixelFormat_D24S8:
      if (!GetExtensions ().has_ext_packed_depth_stencil)
        RaiseNotSupported (METHOD_NAME, "Can't create depth-stencil texture (GL_EXT_packed_depth_stencil not supported)");

    case PixelFormat_D16:
    case PixelFormat_D24X8:
      if (!GetExtensions ().has_arb_depth_texture)
        RaiseNotSupported (METHOD_NAME, "Can't create depth texture (GL_ARB_depth_texture and GL_VERSION_1_4 not supported)");

      break;
    case PixelFormat_S8:
      RaiseNotSupported (METHOD_NAME, "Stencil textures not supported");
      return;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
      return;
  }
  
    //проверка корректности флагов доступа
    
  switch (desc.access_flags)
  {
    case 0:
    case AccessFlag_Read:
    case AccessFlag_Write:
    case AccessFlag_Read | AccessFlag_Write:
      break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.access_flags", desc.access_flags);
      break;
  }

    //выделение нового OpenGL-идентификатора текстуры

  MakeContextCurrent ();
  glGenTextures      (1, &texture_id);
  
    //включение автоматической генерации    

  if (mips_count > 1 && desc.generate_mips_enable && GetExtensions ().has_sgis_generate_mipmap)
    glTexParameteri (GL_TEXTURE_3D_EXT, GL_GENERATE_MIPMAP_SGIS, GL_TRUE);  

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

Texture::~Texture ()
{
  try
  {
    MakeContextCurrent ();
    glDeleteTextures   (1, &texture_id);
    CheckErrors        ("");
  }
  catch (common::Exception& exception)
  {
    exception.Touch ("render::low_level::opengl::Texture::~Texture");

    LogPrintf ("%s\n", exception.Message ());
  }
  catch (std::exception& exception)
  {
    LogPrintf ("%s\n", exception.what ());
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
   Получение дескрипторов
*/

void Texture::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
}

void Texture::GetDesc (BindableTextureDesc& out_desc)
{
  out_desc.id     = texture_id;
  out_desc.target = target;
}

void Texture::GetDesc (RenderTargetTextureDesc& out_desc)
{
  static_cast<TextureDesc&> (out_desc) = desc;

  out_desc.id     = texture_id;
  out_desc.target = target;
}

/*
    Изменение формата текстуры
*/

void Texture::SetFormat (PixelFormat format)
{
  desc.format = format;
}

/*
    Установка / получение номера прикрепленного сэмплера
*/

void Texture::SetSamplerId (size_t id)
{
  binded_sampler_id = id;
}

size_t Texture::GetSamplerId ()
{
  return binded_sampler_id;
}

/*
   Выбор текстуры в контекст OpenGL
*/

void Texture::Bind ()
{
  MakeContextCurrent ();

  glBindTexture (target, texture_id);

  CheckErrors ("render::low_level::opengl::Texture::Bind");
}

/*
    Получение дескриптора mip-уровня текстуры
*/

void Texture::GetMipLevelDesc (size_t mip_level, MipLevelDesc& out_desc)
{
  size_t level_width  = desc.width >> mip_level,
         level_height = desc.height >> mip_level;
  
  switch (desc.format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
         //выравнивание размеров уровня по ближайшему снизу числу, делящемуся на 4

      level_width  &= ~3;
      level_height &= ~3;

      if (!level_width)  level_width  = 4;
      if (!level_height) level_height = 4;

      break;
    default:
      if (!level_width)  level_width  = 1;
      if (!level_height) level_height = 1;
      break;
  }

  out_desc.width  = level_width;
  out_desc.height = level_height;
}

/*
    Получение дескриптора слоя текстуры
*/

void Texture::GetLayerDesc (size_t layer, LayerDesc& desc)
{
  desc.target    = target;
  desc.new_index = layer;
}

/*
    Генерация mip-уровней
*/

namespace
{

size_t get_next_mip_size (size_t size)
{
  switch (size)
  {
    case 0:
    case 1:   return size;
    default:  return size / 2;
  }
}

}

void Texture::BuildMipmaps
 (size_t       x,
  size_t       y,
  size_t       z,
  size_t       width,
  size_t       height,
  PixelFormat  format,
  const void*  data)
{
  GLenum gl_format = get_gl_format (format),
         gl_type   = get_gl_type (format);

  const size_t buffer_size = get_image_size (get_next_mip_size (width), get_next_mip_size (height), format);

  xtl::uninitialized_storage<char> dst_buffer (buffer_size),
                                   src_buffer (buffer_size / 2); //на 4 делить не нужно!

  const char* src = reinterpret_cast<const char*> (data);
  char*       dst = dst_buffer.data ();
    
  for (size_t mip_level=1; width > 1 || height > 1; mip_level++)
  {
    scale_image_2x_down (format, width, height, src, dst);

    width   = get_next_mip_size (width);
    height  = get_next_mip_size (height);
    x      /= 2;
    y      /= 2;    

    if (target != GL_TEXTURE_CUBE_MAP)
      z /= 2;

    SetUncompressedData (z, mip_level, x, y, width, height, gl_format, gl_type, dst);

    dst_buffer.swap (src_buffer);

    dst = dst_buffer.data ();
    src = src_buffer.data ();
  }
}

/*
    Установка данных 
*/

void Texture::SetUncompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, GLenum, const void*)
{
  RaiseNotImplemented ("render::low_level::opengl::Texture::SetUncompressedData");
}

void Texture::SetCompressedData (size_t, size_t, size_t, size_t, size_t, size_t, GLenum, size_t, const void*)
{
  RaiseNotImplemented ("render::low_level::opengl::Texture::SetCompressedData");
}

/*
   Работа с данными
*/

void Texture::SetData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  PixelFormat source_format,
  const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::SetData";
  
    //проверка возможности записи
    
  if (!(desc.access_flags & AccessFlag_Write))
    RaiseInvalidOperation (METHOD_NAME, "Can't set texture data (no AccessFlag_Write in desc.access_flags)");
  
    //проверка корректности номеров слоя и mip-уровня

  if (layer >= desc.layers)
    common::RaiseOutOfRange (METHOD_NAME, "layer", layer, desc.layers);

  if (mip_level >= mips_count)
    common::RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, mips_count);

    //сохранение неотсеченных размеров

  size_t unclamped_width  = width,
         unclamped_height = height;    

    //получение дескриптора mip-уровня и информации о слое текстуры

  MipLevelDesc level_desc;
  LayerDesc    layer_desc;

  GetMipLevelDesc (mip_level, level_desc);
  GetLayerDesc    (layer, layer_desc);

    //отсечение

  if (x >= level_desc.width || y >= level_desc.height)
    return;

  if (x + width > level_desc.width)
    width = level_desc.width - x;

  if (y + height > level_desc.height)
    height = level_desc.height - y;    

  if (!width || !height)
    return;

  if (!buffer)
    common::RaiseNullArgument (METHOD_NAME, "buffer");    
    
    //установка текстуры в контекст OpenGL

  Bind ();  

    //настройка параметров расположения данных в буфере

  glPixelStorei (GL_UNPACK_ROW_LENGTH,  unclamped_width); //длина строки в пикселях
  glPixelStorei (GL_UNPACK_ALIGNMENT,   1);               //выравнивание начала строк
  glPixelStorei (GL_UNPACK_SKIP_ROWS,   0);               //количество пропускаемых строк
  glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);               //количество пропускаемых пикселей

    //проверка совместимости форматов

  bool is_depth_stencil_source_format = is_depth_stencil (source_format),
       is_depth_stencil_tex_format    = is_depth_stencil (desc.format),
       is_compatible                  = (is_depth_stencil_tex_format  && is_depth_stencil_source_format) ||
                                        (!is_depth_stencil_tex_format && !is_depth_stencil_source_format);

  if (!is_compatible)
  {
    RaiseNotSupported (METHOD_NAME, "Texture format %s incompatible with source_format %s", get_name (desc.format),
      get_name (source_format));
  }                                    
                                
  if (source_format == PixelFormat_D24S8 && !GetExtensions ().has_ext_packed_depth_stencil)
    RaiseNotSupported (METHOD_NAME, "Unsupported format %s (GL_EXT_packed_depth_stencil not supported)", get_name (source_format));

    //преобразование формата пикселей
  
  GLenum gl_format = get_gl_format (source_format),
         gl_type   = get_gl_type (source_format);

  if (is_compressed (source_format))
  {
      //проверка корректности размеров

    if ((x % DXT_EDGE_SIZE) || (y % DXT_EDGE_SIZE))          RaiseNotSupported (METHOD_NAME, "Offset (%u, %u) in compressed texture must be a multiple of 4", x, y);
    if ((width % DXT_EDGE_SIZE) || (height % DXT_EDGE_SIZE)) RaiseNotSupported (METHOD_NAME, "Block size (%u, %u) in compressed texture must be a multiple of 4.", width, height);    

      //копирование сжатого образа

    if (GetExtensions ().has_ext_texture_compression_s3tc && source_format == desc.format)
    {
      size_t buffer_size = get_image_size (width, height, source_format);

      SetCompressedData (layer, mip_level, x, y, width, height, gl_format, buffer_size, buffer);
    }
    else
    {
      xtl::uninitialized_storage<char> unpacked_buffer (get_uncompressed_image_size (unclamped_width, unclamped_height, source_format));

      unpack_dxt (source_format, unclamped_width, unclamped_height, buffer, unpacked_buffer.data ());

      gl_format = get_uncompressed_gl_format (source_format);
      gl_type   = get_uncompressed_gl_type (source_format);

      SetUncompressedData (layer, mip_level, x, y, width, height, gl_format, gl_type, unpacked_buffer.data ());
    }
  }
  else
  {
      //копирование несжатого образа

      //преобразование данных буфера глубины

    xtl::uninitialized_storage<size_t> tmp_buffer;

    switch (source_format)
    {
      case PixelFormat_D16:
      {
        tmp_buffer.resize (unclamped_width * unclamped_height);

        size_t*               dst_pixel = tmp_buffer.data ();
        const unsigned short* src_pixel = static_cast<const unsigned short*> (buffer);

        for (size_t count=tmp_buffer.size (); count--; src_pixel++, dst_pixel++)
          *dst_pixel = size_t (*src_pixel) << 16;

        buffer  = tmp_buffer.data ();
        gl_type = GL_UNSIGNED_INT;

        break;
      }
      case PixelFormat_D24X8:
      {
        tmp_buffer.resize (unclamped_width * unclamped_height);        

        size_t*       dst_pixel = tmp_buffer.data ();
        const size_t* src_pixel = static_cast<const size_t*> (buffer);

        for (size_t count=tmp_buffer.size (); count--; src_pixel++, dst_pixel++)
          *dst_pixel = *src_pixel << 8;

        buffer = tmp_buffer.data ();

        break;
      }
      default:
        break;
    }      

    SetUncompressedData (layer, mip_level, x, y, width, height, gl_format, gl_type, buffer);

    if (desc.generate_mips_enable && !mip_level && !GetExtensions ().has_sgis_generate_mipmap)
    {
      BuildMipmaps (x, y, layer, width, height, source_format, buffer);
    }
  }     

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}

namespace
{

size_t align_size (size_t size, size_t align)
{
  return (size+align-1)&~(align-1);  
}

}

void Texture::GetData
 (size_t      layer,
  size_t      mip_level,
  size_t      x,
  size_t      y,
  size_t      width,
  size_t      height,
  PixelFormat target_format,
  void*       buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::GetData";

    //проверка возможности чтения

  if (!(desc.access_flags & AccessFlag_Read))
    RaiseInvalidOperation (METHOD_NAME, "Can't get texture data (no AccessFlag_Read in desc.access_flags)");

    //проверка корректности номеров слоя и mip-уровня

  if (layer >= desc.layers)
    common::RaiseOutOfRange (METHOD_NAME, "layer", layer, desc.layers);

  if (mip_level >= mips_count)
    common::RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, mips_count);
    
    //получение дескриптора mip-уровня и информации о слое текстуры

  MipLevelDesc level_desc;
  LayerDesc    layer_desc;

  GetMipLevelDesc (mip_level, level_desc);
  GetLayerDesc    (layer, layer_desc);  

    //отсечение
    
  size_t unclamped_width  = width,
         unclamped_height = height;

  if (x >= level_desc.width || y >= level_desc.height)
    return;    
    
  if (x + width > level_desc.width)
    width = level_desc.width - x;

  if (y + height > level_desc.height)
    height = level_desc.height - y;    

  if (!width || !height)
    return;    

  if (!buffer)
    common::RaiseNullArgument (METHOD_NAME, "buffer");    

    //проверка совместимости форматов

  bool is_depth_stencil_target_format = is_depth_stencil (target_format),
       is_depth_stencil_tex_format    = is_depth_stencil (desc.format),
       is_compatible                  = (is_depth_stencil_tex_format  && is_depth_stencil_target_format) ||
                                        (!is_depth_stencil_tex_format && !is_depth_stencil_target_format);

  if (target_format == PixelFormat_D24S8 && desc.format != PixelFormat_D24S8)
    is_compatible = false;

  if (!is_compatible)
  {
    common::RaiseNotSupported (METHOD_NAME, "Texture format %s incompatible with target_format %s", get_name (desc.format),
      get_name (target_format));
  }
  
    //установка текстуры в контекст OpenGL  

  Bind ();
  
    //настройка параметров расположения данных в буфере
    
  glPixelStorei (GL_PACK_ROW_LENGTH,  0); //размер строки в пикселях
  glPixelStorei (GL_PACK_ALIGNMENT,   1); //выравнивание начала строк
  glPixelStorei (GL_PACK_SKIP_ROWS,   0); //количество пропускаемых строк
  glPixelStorei (GL_PACK_SKIP_PIXELS, 0); //количество пропускаемых пикселей

  static Extension EXT_texture3d = "GL_EXT_texture3d",
                   Version_1_2   = "GL_VERSION_1_2";

  if (GetExtensions ().has_ext_texture3d)
  {
    glPixelStorei (GL_PACK_SKIP_IMAGES_EXT,  0); //количество пропускаемых слоев
    glPixelStorei (GL_PACK_IMAGE_HEIGHT_EXT, 0); //высота слоя в пикселях
  }

    //проверка частного случая копирования полного образа

  bool is_full_image = width == level_desc.width && height == level_desc.height && desc.layers == 1 && !x && !y;

  if (is_compressed (target_format))
  {
      //проверка возможности копирования
      
    static Extension EXT_texture_compression_s3tc = "GL_EXT_texture_compression_s3tc";
    
    if (!GetExtensions ().has_ext_texture_compression_s3tc)
      common::RaiseNotSupported (METHOD_NAME, "Texture packing not supported (GL_EXT_texture_compression_s3tc not supported)");
      
    if (target_format != desc.format)
      common::RaiseNotSupported (METHOD_NAME, "Can't get compressed texture data, target format %s mismatch with texture format %s", 
                         get_name (target_format), get_name (desc.format));

      //получение необходимой точки входа

    PFNGLGETCOMPRESSEDTEXIMAGEPROC glGetCompressedTexImage_fn = 0;

    if (glGetCompressedTexImage) glGetCompressedTexImage_fn = glGetCompressedTexImage;
    else                         glGetCompressedTexImage_fn = glGetCompressedTexImageARB;

    if (is_full_image)
    {      
      glGetCompressedTexImage_fn (layer_desc.target, mip_level, buffer);
    }
    else
    {
        //проверка корректности смещения и размеров запрашиваемого образа

      if (x % DXT_EDGE_SIZE || y % DXT_EDGE_SIZE)
        common::RaiseNotSupported (METHOD_NAME, "Offset (%u, %u) in compressed texture must be a multiple of 4", x, y);

      if (width % DXT_EDGE_SIZE || height % DXT_EDGE_SIZE)
        common::RaiseNotSupported (METHOD_NAME, "Block size (%u, %u) in compressed texture must be a multiple of 4.", width, height);

        //переход к размерности блока в DXT

      x                /= DXT_EDGE_SIZE;
      y                /= DXT_EDGE_SIZE;
      width            /= DXT_EDGE_SIZE;
      height           /= DXT_EDGE_SIZE;
      unclamped_width  /= DXT_EDGE_SIZE;
      unclamped_height /= DXT_EDGE_SIZE;

        //копирование полного образа во временный буфер

      size_t layer_size = get_image_size (level_desc.width, level_desc.height, target_format),
             quad_size  = get_image_size (DXT_EDGE_SIZE, DXT_EDGE_SIZE, target_format);

      xtl::uninitialized_storage<char> temp_buffer (layer_size * desc.layers);

      glGetCompressedTexImage_fn (layer_desc.target, mip_level, temp_buffer.data ());

        //копирование части образа в пользовательский буфер        

      size_t src_line_size    = level_desc.width / DXT_EDGE_SIZE * quad_size,
             src_start_offset = layer_desc.new_index * layer_size + y * src_line_size + x * quad_size,
             block_size       = width * quad_size,
             dst_line_size    = unclamped_width * quad_size;

      const char* src = temp_buffer.data () + src_start_offset;
      char*       dst = reinterpret_cast<char*> (buffer);

      for (size_t i=0; i<height; i++, src += src_line_size, dst += dst_line_size)
        memcpy (dst, src, block_size);
    }
  }
  else
  {
    if (is_compressed (desc.format))
      common::RaiseNotSupported (METHOD_NAME, "Can't get uncompressed data (format %s) from compressed texture (format %s)", get_name (target_format), get_name (desc.format));

      //копирование несжатого образа
      
    GLenum gl_format = get_gl_format (target_format),
           gl_type   = get_gl_type (target_format);

    if (is_full_image)
    {
      glGetTexImage (layer_desc.target, mip_level, gl_format, gl_type, buffer);

      switch (target_format)
      {
        case PixelFormat_D24X8:
        {
          size_t* pixel = static_cast<size_t*> (buffer);

          for (size_t count=width*height; count--; pixel++)
            *pixel >>= 8;

          break;
        }
        default:
          break;
      }
    }
    else
    {
        //копирование полного образа текстуры во временный буфер

      size_t texel_size = get_texel_size (target_format);

      xtl::uninitialized_storage<char> temp_buffer (get_image_size (level_desc.width, level_desc.height, desc.layers, target_format));

      glGetTexImage (layer_desc.target, mip_level, gl_format, gl_type, temp_buffer.data ());

        //копирование части образа в пользовательсий буфер

      size_t src_line_size    = level_desc.width * texel_size,
             src_layer_size   = src_line_size * level_desc.height,
             src_start_offset = layer_desc.new_index * src_layer_size + y * src_line_size + x * texel_size,
             block_size       = width * texel_size,
             dst_line_size    = unclamped_width * texel_size;

      switch (target_format)
      {
        default:
        {
          const char* src = temp_buffer.data () + src_start_offset;
          char*       dst = reinterpret_cast<char*> (buffer);

          for (size_t i=0; i<height; i++, src += src_line_size, dst += block_size)
            memcpy (dst, src, block_size);

          break;          
        }
        case PixelFormat_D24X8:
        {
            //преобразование данных буфера глубины

          const size_t* src = reinterpret_cast<const size_t*> (temp_buffer.data () + src_start_offset);
          size_t*       dst = reinterpret_cast<size_t*> (buffer);

          block_size    /= sizeof (size_t);
          src_line_size /= sizeof (size_t);
          dst_line_size /= sizeof (size_t);
          
          size_t src_skip_size = src_line_size - block_size,
                 dst_skip_size = dst_line_size - block_size;

          for (size_t i=0; i<height; i++, src += src_skip_size, dst += dst_skip_size)
          {
            for (size_t j=0; j<block_size; j++, src++, dst++)
              *dst = *src >> 8;
          }

          break;
        }
      }
    }
  }

    //проверка ошибок

  CheckErrors (METHOD_NAME);
}
