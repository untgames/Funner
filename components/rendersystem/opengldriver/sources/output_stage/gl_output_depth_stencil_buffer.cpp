#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

DepthStencilBuffer::DepthStencilBuffer (const ContextManager& manager)
  : ContextObject (manager)
{
  desc.dimension            = TextureDimension_2D;
  desc.width                = 0;
  desc.height               = 0;
  desc.layers               = 1;
  desc.format               = PixelFormat_D24S8;
  desc.generate_mips_enable = false;
  desc.access_flags         = AccessFlag_Read | AccessFlag_Write;  
  desc.bind_flags           = BindFlag_DepthStencil;
  desc.usage_mode           = UsageMode_Static;
}

/*
    Получение дескриптора
*/

void DepthStencilBuffer::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
  
  GetSize (out_desc.width, out_desc.height);
}

/*
    Работа с данными
*/

namespace
{

//////сделать через битовые поля!!!!

//выделение компоненты глубины
inline float get_depth_component (size_t pixel)
{
  return float (pixel & 0xFFFFFF) / float (0xFFFFFF);
}

//выделение компоненты трафарета
inline unsigned char get_stencil_index (size_t pixel)
{
  return unsigned char (pixel >> 24);
}

//получение упакованного пикселя по компонентам глубины и трафарета
inline size_t get_depth_stencil_pixel (float depth, unsigned char stencil)
{
  return (size_t (depth * 0xFFFFFF) & 0xFFFFFF) + (size_t (stencil) << 24);
}

}

void DepthStencilBuffer::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::DepthStencilBuffer::SetData";

  if (layer)
    RaiseOutOfRange (METHOD_NAME, "layer", layer, 1);
    
  if (mip_level)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, 1);

  if (!width || !height)
    return;
    
  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");
    
    //установка буфера в контекст OpenGL

  Bind ();

    //проверка наличия расширения GL_ARB_window_pos либо версии OpenGL не ниже 1.4
    
  static Extension ARB_window_pos = "GL_ARB_window_pos",
                   Version_1_4    = "GL_VERSION_1_4";
  
  if (!IsSupported (ARB_window_pos) && !IsSupported (Version_1_4))
    RaiseNotSupported (METHOD_NAME, "Can not set image at position (%u;%u) (GL_ARB_window_pos not supported)", x, y);

  if      (glWindowPos2iARB) glWindowPos2iARB (x, y);
  else if (glWindowPos2i)    glWindowPos2i    (x, y);
  else                       return;
  
    //копирование
    
  switch (source_format)
  {
    case PixelFormat_D16:
      glDrawPixels (width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, buffer);
      break;
    case PixelFormat_D24X8:
    {
      xtl::uninitialized_storage<size_t> depth_buffer (width * height);
      
      size_t*       dst_pixel = depth_buffer.data ();
      const size_t* src_pixel = static_cast<const size_t*> (buffer);
      
      for (size_t count=width*height; count--; src_pixel++, dst_pixel++)
        *dst_pixel = *src_pixel << 8;
      
      glDrawPixels (width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, depth_buffer.data ());

      break;
    }
    case PixelFormat_D24S8:
    {
        //копирование может быть произведено двумя способами: при помощи расширения EXT_packed_depth_stencil,
        //либо посредством разделения переданного буфера на 2: буфер глубины и буфер трафарета

      if (GLEW_EXT_packed_depth_stencil)
      {
        glDrawPixels (width, height, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, buffer);
      }
      else
      {
          //разделение данных буфера на компоненты глубины и трафарета

        xtl::uninitialized_storage<float>         depth_buffer (width * height);
        xtl::uninitialized_storage<unsigned char> stencil_buffer (width * height);    
        
        const size_t*  src_pixel         = static_cast<const size_t*> (buffer);
        float*         dst_depth_pixel   = depth_buffer.data ();
        unsigned char* dst_stencil_pixel = stencil_buffer.data ();
        
        for (size_t count=width*height; count--; src_pixel++, dst_depth_pixel++, dst_stencil_pixel++)
        {
          *dst_depth_pixel   = get_depth_component (*src_pixel);
          *dst_stencil_pixel = get_stencil_index (*src_pixel);
        }

          //копирование    

        glDrawPixels (width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer.data ());
        glDrawPixels (width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, stencil_buffer.data ());
      }

      break;
    }
    case PixelFormat_S8:
      glDrawPixels (width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, buffer);
      break;
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      RaiseNotSupported (METHOD_NAME, "Unsupported %s=%s", "source_format", get_name (source_format));
      return;
    default:
      RaiseInvalidArgument (METHOD_NAME, "source_format", source_format);
      return;
  }    

    //проверка состояния OpenGL

  CheckErrors (METHOD_NAME);
}

void DepthStencilBuffer::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::DepthStencilBuffer::GetData";

  if (layer)
    RaiseOutOfRange (METHOD_NAME, "layer", layer, 1);
    
  if (mip_level)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, 1);    

  if (!width || !height)
    return;
    
  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");
    
    //установка буфера в контекст OpenGL

  Bind ();
  
    //копирование
    
  switch (target_format)
  {
    case PixelFormat_D16:
      glReadPixels (x, y, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT, buffer);
      break;
    case PixelFormat_D24X8:
    {
      glReadPixels (x, y, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, buffer);
      
      size_t* pixel = static_cast<size_t*> (buffer);

      for (size_t count=width*height; count--; pixel++)
        *pixel >>= 8;

      break;
    }
    case PixelFormat_D24S8:
    {
        //копирование может быть произведено двумя способами: при помощи расширения EXT_packed_depth_stencil,
        //либо посредством разделения переданного буфера на 2: буфер глубины и буфер трафарета

      if (GLEW_EXT_packed_depth_stencil)
      {
        glReadPixels (x, y, width, height, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, buffer);
      }
      else
      {
          //создание буферов для хранения разделенных данных глубины и трафарета

        xtl::uninitialized_storage<float>         depth_buffer (width * height);
        xtl::uninitialized_storage<unsigned char> stencil_buffer (width * height);

          //копирование

        glReadPixels (x, y, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth_buffer.data ());
        glReadPixels (x, y, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, stencil_buffer.data ());

          //упаковка прочитанных данных

        size_t*              dst_pixel         = static_cast<size_t*> (buffer);
        const float*         src_depth_pixel   = depth_buffer.data ();
        const unsigned char* src_stencil_pixel = stencil_buffer.data ();

        for (size_t count=width*height; count--; dst_pixel++, src_depth_pixel++, src_stencil_pixel++)
          *dst_pixel = get_depth_stencil_pixel (*src_depth_pixel, *src_stencil_pixel);
      }

      break;
    }
    case PixelFormat_S8:
      glDrawPixels (width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, buffer);
      break;
    case PixelFormat_RGB8:
    case PixelFormat_RGBA8:
    case PixelFormat_L8:
    case PixelFormat_A8:
    case PixelFormat_LA8:
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:
      RaiseNotSupported (METHOD_NAME, "Unsupported %s=%s", "target_format", get_name (target_format));
      return;
    default:
      RaiseInvalidArgument (METHOD_NAME, "target_format", target_format);
      return;
  }
    
    //проверка состояния OpenGL

  CheckErrors (METHOD_NAME);  
}
