#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;

/*
   Конструктор / деструктор
*/

Texture::Texture  (const ContextManager& manager, const TextureDesc& tex_desc, GLenum in_target)
  : ContextObject (manager), target (in_target), desc (tex_desc)
{
  mips_count = get_mips_count (tex_desc.width, tex_desc.height);

  MakeContextCurrent ();
  glGenTextures      (1, &texture_id);
  CheckErrors        ("render::low_level::opengl::Texture::Texture");
}

Texture::~Texture ()
{
  MakeContextCurrent ();
  glDeleteTextures (1, &texture_id);
  CheckErrors ("render::low_level::opengl::Texture::~Texture");
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

    //отсечение

  size_t level_width  = desc.width >> mip_level,
         level_height = desc.height >> mip_level;

  if (x > level_width)
    return;
    
  if (y > level_height)
    return;
    
  if (x + width > level_width)
    width = level_width - x;

  if (y + height > level_height)
    height = level_height - y;

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
  
    //получение информации о текстуре
    
  GLenum layer_target  = GetLayerTarget (layer);
  bool   is_full_image = width == level_width && height == level_height && desc.layers == 1 && !x && !y;
         
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
      glGetCompressedTexImage (layer_target, mip_level, buffer);      
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

        //Копирование полного образа во временный буфер


      size_t quad_size  = compressed_quad_size (target_format),
             layer_size = level_width * level_height / DXT_BLOCK_SIZE * quad_size;
      
      xtl::uninitialized_storage<char> temp_buffer (layer_size * desc.layers);

      glGetCompressedTexImage (layer_target, mip_level, temp_buffer.data ());      

        //копирование части образа в пользовательсий буфер
               
      size_t quad_line_size = level_width / DXT_EDGE_SIZE * quad_size,
             start_offset   = layer * layer_size + y * quad_line_size + x * quad_size,
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
      
    //!!! Учесть случай RGB 3D текстур
    
    int pack_skip_images = -1, pack_image_height, unpack_skip_images = -1, unpack_image_height = -1,
        pack_row_length = -1, pack_alignment = -1, pack_skip_rows = -1, pack_skip_pixels = -1,
        unpack_row_length = -1, unpack_alignment = -1, unpack_skip_rows = -1, unpack_skip_pixels = -1;            
    
    glGetIntegerv (GL_PACK_SKIP_IMAGES_EXT, &pack_skip_images);
    glGetIntegerv (GL_PACK_IMAGE_HEIGHT_EXT, &pack_image_height);
    glGetIntegerv (GL_UNPACK_SKIP_IMAGES_EXT, &unpack_skip_images);
    glGetIntegerv (GL_UNPACK_IMAGE_HEIGHT_EXT, &unpack_image_height);    
    glGetIntegerv (GL_PACK_ROW_LENGTH, &pack_row_length);
    glGetIntegerv (GL_PACK_ALIGNMENT, &pack_alignment);
    glGetIntegerv (GL_PACK_SKIP_ROWS, &pack_skip_rows);
    glGetIntegerv (GL_PACK_SKIP_PIXELS, &pack_skip_pixels);    
    glGetIntegerv (GL_UNPACK_ROW_LENGTH, &unpack_row_length);
    glGetIntegerv (GL_UNPACK_ALIGNMENT, &unpack_alignment);
    glGetIntegerv (GL_UNPACK_SKIP_ROWS, &unpack_skip_rows);
    glGetIntegerv (GL_UNPACK_SKIP_PIXELS, &unpack_skip_pixels);    
    
    printf ("#pack_skip_images=%d\n", pack_skip_images);
    printf ("#pack_image_height=%d\n", pack_image_height);
    printf ("#unpack_skip_images=%d\n", unpack_skip_images);
    printf ("#unpack_image_height=%d\n", unpack_image_height);
    printf ("#pack_row_length=%d\n", pack_row_length);
    printf ("#pack_alignment=%d\n", pack_alignment);
    printf ("#pack_skip_rows=%d\n", pack_skip_rows);
    printf ("#pack_skip_pixels=%d\n", pack_skip_pixels);
    printf ("#unpack_row_length=%d\n", unpack_row_length);
    printf ("#unpack_alignment=%d\n", unpack_alignment);
    printf ("#unpack_skip_rows=%d\n", unpack_skip_rows);
    printf ("#unpack_skip_pixels=%d\n", unpack_skip_pixels);

    if (is_full_image)
    {
      glGetTexImage (layer_target, mip_level, gl_tex_format, gl_tex_type, buffer);
    }
    else
    {      
        //копирование полного образа текстуры во временный буфер
        
      size_t texel_size = opengl::texel_size (target_format);
        
      xtl::uninitialized_storage<char> temp_buffer (level_width * level_height * desc.layers * texel_size * 2);
      
      memset (temp_buffer.data (), 0xfe, temp_buffer.size ());
      printf ("in\n buffer size = %u, width = %u, height = %u, layers = %u, texel_size = %u\n", 
              temp_buffer.size (), level_width, level_height, desc.layers, texel_size);
      glGetTexImage (layer_target, mip_level, gl_tex_format, gl_tex_type, temp_buffer.data ());

      printf ("byte 0 = %02x\n", temp_buffer.data()[0]);
      for (size_t i = 0; i < temp_buffer.size (); i++)
        if (temp_buffer.data ()[i] == 0xfffffffe)
        {
          printf ("data ends at byte %u\n", i);
          printf ("after data:\n");

          while (temp_buffer.data ()[i] == 0xfffffffe)
          {
//          for (size_t j = 0; j < 128*128; j++)
//            printf ("%02x ", (size_t)temp_buffer.data ()[i+j] & 0xff);
            i++;
          }
          printf ("i = %u", i);
          printf ("\n");

          break;
        }

      printf ("out\n");
      
        //копирование части образа в пользовательсий буфер
               
      size_t line_size    = level_width * texel_size,
             layer_size   = line_size * level_height,
             start_offset = layer * layer_size + y * line_size + x * texel_size,
             block_size   = width * texel_size;
      
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
    default: common::RaiseInvalidArgument ("render::low_level::opengl::Texture::compressed_texel_size", "format"); return 1;
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

//получение количества mip-уровней
size_t get_mips_count (size_t size) //оптимизировать
{
  return (size_t)(log ((float)next_higher_power_of_two (size)) / log (2.f)) + 1;
}

//получение количества mip-уровней
size_t get_mips_count (size_t width, size_t height)
{
  return width > height ? get_mips_count (width) : get_mips_count (height);
}

}

}

}
