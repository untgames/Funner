#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
   Конструктор / деструктор
*/

Texture::Texture  (const ContextManager& manager, const TextureDesc& tex_desc, GLenum in_target)
  : ContextObject (manager), target (in_target), desc (tex_desc)
{
  static const char* METHOD_NAME = "render::low_level::opengl::Texture::Texture";

    //расчёт числа mip-уровней    

  size_t max_edge_size = desc.width > desc.height ? desc.width : desc.height;            

  mips_count = (size_t)(log ((float)max_edge_size) / log (2.f)) + 1;

  switch (desc.format)
  {
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
    {
        //проверка корректности начальных размеров

      if (mips_count < 3)
      {
        if (desc.width % 4)  RaiseInvalidArgument (METHOD_NAME, "desc.width", desc.width, "Reason: width is not multiple of 4");
        if (desc.height % 4) RaiseInvalidArgument (METHOD_NAME, "desc.height", desc.height, "Reason: height is not multiple of 4");
      }

      mips_count -= 2;

      break;
    }
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_L8:    
    case PixelFormat_A8:    
    case PixelFormat_S8:
    case PixelFormat_LA8:   
    case PixelFormat_RGB8:  
    case PixelFormat_RGBA8:     
      break;
    default:
      RaiseInvalidArgument (METHOD_NAME, "desc.format", desc.format);
      break;
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

void Texture::GetDesc (TextureDesc& target_desc)
{
  target_desc.dimension    = desc.dimension;
  target_desc.width        = desc.width;
  target_desc.height       = desc.height;
  target_desc.layers       = desc.layers;
  target_desc.format       = desc.format;
  target_desc.generate_mips_enable = desc.generate_mips_enable;
  target_desc.bind_flags   = BindFlag_Texture;
  target_desc.access_flags = AccessFlag_Read | AccessFlag_Write;

  if ((target_desc.usage_mode != UsageMode_Default) && (target_desc.usage_mode != UsageMode_Static) && 
      (target_desc.usage_mode != UsageMode_Dynamic) && (target_desc.usage_mode != UsageMode_Stream))
    target_desc.usage_mode = UsageMode_Default;
}

void Texture::GetDesc (BindableTextureDesc& desc)
{
  desc.id     = texture_id;
  desc.target = target;
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

      break;
    default:
      break;
  }

  if (!level_width)  level_width  = 1;
  if (!level_height) level_height = 1;

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
   Работа с данными
*/

void Texture::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  static Extension EXT_packed_depth_stencil     = "GL_EXT_packed_depth_stencil",
                   EXT_framebuffer_object       = "GL_EXT_framebuffer_object";
    
  bool has_ext_packed_depth_stencil = GetContextManager().IsSupported (EXT_packed_depth_stencil) && GetContextManager().IsSupported (EXT_framebuffer_object);  

  if (!buffer)
    common::RaiseNullArgument ("render::low_level::opengl::Texture::SetData", "buffer");

  if ((source_format == PixelFormat_D24S8)  && (!has_ext_packed_depth_stencil))
    common::RaiseNotSupported ("render::low_level::opengl::Texture::SetData", 
                               "Can't set depth-stencil data. Reason: GL_EXT_packed_depth_stencil extension not supported");

  if (is_depth_format (desc.format) && !is_depth_format (source_format))
    common::RaiseInvalidArgument ("render::low_level::opengl::Texture::SetData", "source_format");
  if (!is_depth_format (desc.format) && is_depth_format (source_format))
    common::RaiseInvalidArgument ("render::low_level::opengl::Texture::SetData", "source_format");
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

  if (layer >= desc.layers)
    common::RaiseOutOfRange (METHOD_NAME, "layer", layer, desc.layers);

  if (mip_level >= mips_count)
    common::RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, mips_count);
    
    //получение дескриптора mip-уровня
    
  MipLevelDesc level_desc;

  GetMipLevelDesc (mip_level, level_desc);
    
    //отсечение

  if (x > level_desc.width)
    return;
    
  if (y > level_desc.height)
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
  
    //получение информации о достпных расширениях

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
      glGetCompressedTexImage (layer_desc.target, mip_level, buffer);      
    }
    else
    {
      static const size_t DXT_EDGE_SIZE  = 4,
                          DXT_BLOCK_SIZE = DXT_EDGE_SIZE * DXT_EDGE_SIZE;

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

      glGetCompressedTexImage (layer_desc.target, mip_level, temp_buffer.data ());      

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
      //копирование несжатого образа
      
    GLenum gl_tex_format = gl_format (target_format),
           gl_tex_type   = gl_type (target_format);

    if (is_full_image)
    {
//      printf ("target=%04x error=%04x\n", layer_desc.target, glGetError ());
      
      glGetTexImage (layer_desc.target, mip_level, gl_tex_format, gl_tex_type, buffer);
      
//      printf ("error=%04x\n", glGetError ());            
    }
    else
    {
//      printf ("fuck!\n");
      
        //копирование полного образа текстуры во временный буфер
        
      size_t texel_size     = opengl::texel_size (target_format),
             tmp_texel_size = texel_size != 3 || target != GL_TEXTURE_3D_EXT ? texel_size : 4; ////обход бага (?) OpenGL!!

      xtl::uninitialized_storage<char> temp_buffer (level_desc.width * level_desc.height * desc.layers * tmp_texel_size);

      glGetTexImage (layer_desc.target, mip_level, gl_tex_format, gl_tex_type, temp_buffer.data ());

        //копирование части образа в пользовательсий буфер

      size_t line_size    = level_desc.width * texel_size,
             layer_size   = line_size * level_desc.height,
             block_size   = width * texel_size,
             start_offset = layer_desc.new_index * layer_size + y * line_size + x * texel_size;

      const char* src = temp_buffer.data () + start_offset;
      char*       dst = reinterpret_cast<char*> (buffer);
      
      for (size_t i=0; i<height; i++)
      {
        memcpy (dst, src, block_size);

        src += line_size;
        dst += block_size;
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
    case PixelFormat_S8:    common::RaiseNotSupported ("render::low_level::opengl::Texture::gl_format", "Stencil textures not supported.");
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::gl_internal_format", "format"); return GL_ALPHA;
  }
}

GLenum gl_format (PixelFormat format)
{
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
    case PixelFormat_S8:    common::RaiseNotSupported ("render::low_level::opengl::Texture::gl_format", "Stencil textures not supported.");
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::gl_format", "format"); return GL_ALPHA;
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
      common::RaiseInvalidArgument ("render::low_level::opengl::Texture::compressed_quad_size", "format", get_name (format));
      break;
    default:
      common::RaiseInvalidArgument ("render::low_level::opengl::Texture::compressed_quad_size", "format", format);
      break;
  }
  
  return 1;  
}

//получение ближайшей сверху степени двойки
size_t next_higher_power_of_two (size_t k) 
{
  k--;

  for (int i=1; i < sizeof (size_t) * 8; i *= 2)
          k |= k >> i;

  return k + 1;
}

}

}

}
