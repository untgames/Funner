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

Texture::Texture  (const ContextManager& manager, const TextureDesc& tex_desc, GLenum in_target)
  : ContextObject (manager), target (in_target), desc (tex_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::Texture";
  
    //+размеры текстуры (нулевые)!!!

    //расчёт числа mip-уровней

  size_t max_edge_size = desc.width > desc.height ? desc.width : desc.height;

  mips_count = (size_t)(log ((float)max_edge_size) / log (2.f)) + 1;  

    //проверка корректности формата

  static Extension EXT_packed_depth_stencil = "GL_EXT_packed_depth_stencil",
                   ARB_depth_texture        = "GL_ARB_depth_texture",
                   Version_1_4              = "GL_VERSION_1_4";

  switch (tex_desc.format)
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
      if (!IsSupported (EXT_packed_depth_stencil))
        RaiseNotSupported (METHOD_NAME, "Can't create depth-stencil texture (GL_EXT_packed_depth_stencil not supported)");

    case PixelFormat_D16:
    case PixelFormat_D24X8:
      if (!IsSupported (ARB_depth_texture) && !IsSupported (Version_1_4))
        RaiseNotSupported (METHOD_NAME, "Can't create depth texture (GL_ARB_depth_texture & GL_VERSION_1_4 not supported)");

      break;
    case PixelFormat_S8:
      RaiseNotSupported (METHOD_NAME, "Stencil textures not supported");
      return;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", tex_desc.format);
      return;
  }  

    //выделение нового OpenGL-идентификатора текстуры

  MakeContextCurrent ();
  glGenTextures      (1, &texture_id);

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
   Получение дескриптора
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

/*
   Выбор текстуры в контекст OpenGL
*/

void Texture::Bind ()
{
  MakeContextCurrent ();
  glBindTexture(target, texture_id);
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
  GLenum gl_format = opengl::gl_format (format),
         gl_type   = opengl::gl_type (format);

  const size_t buffer_size = get_next_mip_size (width) * get_next_mip_size (height) * texel_size (format);

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

    if (target != GL_TEXTURE_CUBE_MAP) //????????
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
  
       //объединить код с GetData!!!
  
    //проверка корректности номеров слоя и mip-уровня

  if (layer >= desc.layers)
    common::RaiseOutOfRange (METHOD_NAME, "layer", layer, desc.layers);

  if (mip_level >= mips_count)
    common::RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, mips_count);

    //получение дескриптора mip-уровня

  MipLevelDesc level_desc;

  GetMipLevelDesc (mip_level, level_desc);  

    //отсечение

  if (x >= level_desc.width)
    return;

  if (y >= level_desc.height)
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

  MakeContextCurrent ();

  Bind ();

    //проверка совместимости форматов

  bool is_depth_source_format = is_depth_format (source_format),
       is_depth_tex_format    = is_depth_format (desc.format),
       is_compatible          = (is_depth_tex_format  && is_depth_source_format) ||
                                (!is_depth_tex_format && !is_depth_source_format);

  if (!is_compatible)
  {
    RaiseNotSupported (METHOD_NAME, "Texture format %s incompatible with source_format %s", get_name (desc.format),
      get_name (source_format));
  }                                    
                                
  if (source_format == PixelFormat_D24S8)
  {
    static Extension EXT_packed_depth_stencil = "GL_EXT_packed_depth_stencil";

    if (!IsSupported (EXT_packed_depth_stencil))
      RaiseNotSupported (METHOD_NAME, "Unsupported format %s (GL_EXT_packed_depth_stencil not supported)", get_name (source_format));
  }  

    //получение информации о доступных расширениях

  TextureExtensions ext (GetContextManager ());

    //получение информации о слое текстуры
    
  LayerDesc layer_desc;

  GetLayerDesc (layer, layer_desc);
  
    //преобразование формата пикселей
  
  GLenum gl_format = opengl::gl_format (source_format),
         gl_type   = opengl::gl_type (source_format);

  if (is_compressed_format (source_format))
  {
      //проверка корректности размеров

    if ((x % DXT_EDGE_SIZE) || (y % DXT_EDGE_SIZE))          RaiseNotSupported (METHOD_NAME, "Offset (%u, %u) in compressed texture must be a multiple of 4", x, y);
    if ((width % DXT_EDGE_SIZE) || (height % DXT_EDGE_SIZE)) RaiseNotSupported (METHOD_NAME, "Block size (%u, %u) in compressed texture must be a multiple of 4.", width, height);    

      //копирование сжатого образа

    if (ext.has_ext_texture_compression_s3tc && source_format == desc.format)
    {
      size_t buffer_size = width * height / DXT_BLOCK_SIZE * compressed_quad_size (source_format);
      
      SetCompressedData (layer, mip_level, x, y, width, height, gl_format, buffer_size, buffer);
    }
    else
    {
      xtl::uninitialized_storage<char> unpacked_buffer (width * height * unpack_texel_size (source_format));

      unpack_dxt (source_format, width, height, buffer, unpacked_buffer.data ());

      gl_format = unpack_format (source_format);
      gl_type   = unpack_type (source_format);

      SetUncompressedData (layer, mip_level, x, y, width, height, gl_format, gl_type, unpacked_buffer.data ());
    }     
  }
  else
  {
      //копирование несжатого образа      
      
      //преобразование данных буфера глубины
      
    xtl::uninitialized_storage<size_t> tmp_buffer;  //неверно!!! после отсечения!!!
      
    switch (source_format)
    {
      case PixelFormat_D16:
      {
        tmp_buffer.resize (width * height);

        size_t*               dst_pixel = tmp_buffer.data ();
        const unsigned short* src_pixel = static_cast<const unsigned short*> (buffer);

        for (size_t count=width*height; count--; src_pixel++, dst_pixel++)
          *dst_pixel = size_t (*src_pixel) << 16;

        buffer  = tmp_buffer.data ();
        gl_type = GL_UNSIGNED_INT;

        break;
      }
      case PixelFormat_D24X8:
      {
        tmp_buffer.resize (width * height);        

        size_t*       dst_pixel = tmp_buffer.data ();
        const size_t* src_pixel = static_cast<const size_t*> (buffer);

        for (size_t count=width*height; count--; src_pixel++, dst_pixel++)
          *dst_pixel = *src_pixel << 8;

        buffer = tmp_buffer.data ();

        break;
      }
      default:
        break;
    }      

     // уточнить про перегенерацию при установке mip_level != 0!!!

    SetUncompressedData (layer, mip_level, x, y, width, height, gl_format, gl_type, buffer);

    if (desc.generate_mips_enable && !mip_level && !ext.has_sgis_generate_mipmap)
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
  
    //проверка корректности номеров слоя и mip-уровня

  if (layer >= desc.layers)
    common::RaiseOutOfRange (METHOD_NAME, "layer", layer, desc.layers);

  if (mip_level >= mips_count)
    common::RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, mips_count);
    
    //получение дескриптора mip-уровня
    
  MipLevelDesc level_desc;

  GetMipLevelDesc (mip_level, level_desc);
    
    //отсечение

  if (x >= level_desc.width)
    return;
    
  if (y >= level_desc.height)
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

  bool is_depth_target_format = is_depth_format (target_format),
       is_depth_tex_format    = is_depth_format (desc.format),
       is_compatible          = (is_depth_tex_format  && is_depth_target_format) ||
                                (!is_depth_tex_format && !is_depth_target_format);

  if (target_format == PixelFormat_D24S8 && desc.format != PixelFormat_D24S8)
    is_compatible = false;

  if (!is_compatible)
  {
    common::RaiseNotSupported (METHOD_NAME, "Texture format %s incompatible with target_format %s", get_name (desc.format),
      get_name (target_format));
  }
  
    //установка текстуры в контекст OpenGL
  
  MakeContextCurrent ();

  Bind ();
  
    //получение информации о доступных расширениях

  TextureExtensions ext (GetContextManager ());
  
    //получение информации о слое текстуры

  LayerDesc layer_desc;
  
  GetLayerDesc (layer, layer_desc);

  bool is_full_image = width == level_desc.width && height == level_desc.height && desc.layers == 1 && !x && !y;

  if (is_compressed_format (target_format))
  {
      //копирование сжатого образа
    
    if (!ext.has_ext_texture_compression_s3tc)
      common::RaiseNotSupported (METHOD_NAME, "Texture packing not supported (GL_EXT_texture_compression_s3tc not supported)");
      
    if (target_format != desc.format)
      common::RaiseNotSupported (METHOD_NAME, "Can't get compressed texture data, target format %s mismatch with texture format %s", 
                         get_name (target_format), get_name (desc.format));
 
    if (is_full_image)
    {
      if (ext.has_arb_texture_compression) glGetCompressedTexImageARB (layer_desc.target, mip_level, buffer);
      else                                 glGetCompressedTexImage    (layer_desc.target, mip_level, buffer);
    }
    else
    {
      if ((x % DXT_EDGE_SIZE) || (y % DXT_EDGE_SIZE))          common::RaiseNotSupported (METHOD_NAME, "Offset (%u, %u) in compressed texture must be a multiple of 4", x, y);
      if ((width % DXT_EDGE_SIZE) || (height % DXT_EDGE_SIZE)) common::RaiseNotSupported (METHOD_NAME, "Block size (%u, %u) in compressed texture must be a multiple of 4.", width, height);

        //Переход к размерности блока в DXT

      x      /= DXT_EDGE_SIZE;
      y      /= DXT_EDGE_SIZE;
      width  /= DXT_EDGE_SIZE;
      height /= DXT_EDGE_SIZE;

        //копирование полного образа во временный буфер

      size_t quad_size  = compressed_quad_size (target_format),
             layer_size = level_desc.width * level_desc.height / DXT_BLOCK_SIZE * quad_size;
      
      xtl::uninitialized_storage<char> temp_buffer (layer_size * desc.layers);
      

      if (glGetCompressedTexImage) glGetCompressedTexImage    (layer_desc.target, mip_level, temp_buffer.data ());
      else                         glGetCompressedTexImageARB (layer_desc.target, mip_level, temp_buffer.data ());


        //копирование части образа в пользовательский буфер
               
      size_t quad_line_size = level_desc.width / DXT_EDGE_SIZE * quad_size,
             start_offset   = layer_desc.new_index * layer_size + y * quad_line_size + x * quad_size,
             block_size     = width * quad_size;
             
      const char* src = temp_buffer.data () + start_offset;
      char*       dst = reinterpret_cast<char*> (buffer);
      
      for (size_t i = 0; i < height; i++)
      {
        memcpy (dst, src, block_size);

        src += quad_line_size;
        dst += block_size;
      }
    }
  }
  else
  {
    if (is_compressed_format (desc.format))
      common::RaiseNotSupported (METHOD_NAME, "Can't get uncompressed data (format %s) from compressed texture (format %s)", get_name (target_format), get_name (desc.format));

      //копирование несжатого образа
      
    GLenum gl_tex_format = gl_format (target_format),
           gl_tex_type   = gl_type (target_format);

    if (is_full_image)
    {
      glGetTexImage (layer_desc.target, mip_level, gl_tex_format, gl_tex_type, buffer);      

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
        
      size_t texel_size     = opengl::texel_size (target_format),
//             tmp_texel_size = target != GL_TEXTURE_3D_EXT ? texel_size : align_size (texel_size, 4); //обход бага (?) ATI OpenGL
             tmp_texel_size = target == GL_TEXTURE_3D_EXT  && opengl::texel_size (desc.format) == 3 ? texel_size = 4 : texel_size; //обход бага (?) ATI OpenGL

      if (opengl::texel_size (desc.format) > tmp_texel_size)
        tmp_texel_size = opengl::texel_size (desc.format);

      xtl::uninitialized_storage<char> temp_buffer (level_desc.width * level_desc.height * desc.layers * tmp_texel_size);

      if (opengl::texel_size (desc.format) == 1 || opengl::texel_size (desc.format) == 2)
        tmp_texel_size = opengl::texel_size (desc.format);

      glGetTexImage (layer_desc.target, mip_level, gl_tex_format, gl_tex_type, temp_buffer.data ());

        //копирование части образа в пользовательсий буфер

      size_t line_size    = level_desc.width * texel_size,
             layer_size   = level_desc.width * level_desc.height * tmp_texel_size,
             block_size   = width * texel_size,
             start_offset = layer_desc.new_index * layer_size + y * line_size + x * texel_size;

               //отсечка и width!!!

      switch (target_format)
      {
        default:
        {
          const char* src = temp_buffer.data () + start_offset;          
          char*       dst = reinterpret_cast<char*> (buffer);          

          for (size_t i=0; i<height; i++, src += line_size, dst += block_size)
            memcpy (dst, src, block_size);

          break;          
        }
        case PixelFormat_D24X8:
        {
            //преобразование данных буфера глубины

          const size_t* src = reinterpret_cast<const size_t*> (temp_buffer.data () + start_offset);
          size_t*       dst = reinterpret_cast<size_t*> (buffer);

          block_size /= sizeof (size_t);
          line_size  /= sizeof (size_t);

          for (size_t i=0; i<height; i++, src += line_size - block_size)
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

/*
   Получение параметров текстуры
*/

namespace render
{

namespace low_level
{

namespace opengl
{
  
size_t texel_size (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_S8:    return 1;
    case PixelFormat_LA8:
    case PixelFormat_D16:   return 2;
    case PixelFormat_RGB8:  return 3;
    case PixelFormat_RGBA8:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8: return 4;  
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::texel_size", "format"); return 1;
  }
}

GLint gl_internal_format (PixelFormat format)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::gl_internal_format";

  switch (format)
  {
    case PixelFormat_L8:    return GL_LUMINANCE8;
    case PixelFormat_A8:    return GL_ALPHA8;
    case PixelFormat_LA8:   return GL_LUMINANCE8_ALPHA8;
    case PixelFormat_RGB8:  return GL_RGB8;
    case PixelFormat_RGBA8: return GL_RGBA8;
    case PixelFormat_DXT1:  return GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
    case PixelFormat_DXT3:  return GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
    case PixelFormat_DXT5:  return GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
    case PixelFormat_D16:   return GL_DEPTH_COMPONENT16_ARB;
    case PixelFormat_D24X8: return GL_DEPTH_COMPONENT24_ARB;
    case PixelFormat_D24S8: return GL_DEPTH24_STENCIL8_EXT;  
    case PixelFormat_S8:    common::RaiseNotSupported (METHOD_NAME, "Stencil textures not supported.");
    default: common::RaiseInvalidArgument (METHOD_NAME, "format"); return GL_ALPHA;
  }
}

GLenum gl_format (PixelFormat format)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::gl_format";

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
    case PixelFormat_S8:    common::RaiseNotSupported (METHOD_NAME, "Stencil textures not supported.");
    default: common::RaiseInvalidArgument (METHOD_NAME, "format"); return GL_ALPHA;
  }
}

GLenum gl_type (PixelFormat format)
{
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
    case PixelFormat_D24X8: return GL_UNSIGNED_INT;
    case PixelFormat_D24S8: return GL_UNSIGNED_INT_24_8_EXT;
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::gl_type", "format"); return GL_UNSIGNED_BYTE;
  }
}

bool is_compressed_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:  return true;
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8: 
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:   
    default: return false;
  }
}

bool is_depth_format (PixelFormat format)
{
  switch (format)
  {
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8: return true;   
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8: 
    default: return false;
  }
}

size_t compressed_quad_size (PixelFormat format)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::compressed_quad_size";

  switch (format)
  {
    case PixelFormat_DXT1: return 8;
    case PixelFormat_DXT3: return 16;
    case PixelFormat_DXT5: return 16;
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8:
      common::RaiseInvalidArgument (METHOD_NAME, "format", get_name (format));
      break;
    default:
      common::RaiseInvalidArgument (METHOD_NAME, "format", format);
      break;
  }
  
  return 1;  
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
    case GL_DEPTH_STENCIL_EXT:             return PixelFormat_D24S8;
    default:
      RaiseNotSupported ("render::low_level::get_pixel_format", "Unknown gl_format=%04x", gl_format);
      return (PixelFormat)0;
  }
}

//получение ближайшей сверху степени двойки
size_t next_higher_power_of_two (size_t k) 
{
  k--;

  for (int i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

//получение размера образа сжатой текстуры
size_t get_compressed_image_size (PixelFormat format, size_t width, size_t height)
{
  return width * height / 16 * compressed_quad_size (format);
}

}

}

}
