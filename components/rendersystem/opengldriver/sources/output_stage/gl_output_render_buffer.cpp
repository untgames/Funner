#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

namespace
{

/*
    Временное хранилище изменяемых параметров OpenGL
*/

const GLenum MODE_NAMES [] = {GL_BLEND, GL_DEPTH_TEST, GL_STENCIL_TEST, GL_SCISSOR_TEST, GL_FOG, GL_TEXTURE_1D, GL_TEXTURE_2D,
                              GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_RECTANGLE_EXT};
const size_t MODE_NAMES_COUNT = sizeof (MODE_NAMES) / sizeof (*MODE_NAMES);

struct RenderBufferTempState
{
  public:
    RenderBufferTempState ()
    {
        //сохранение состояния параметров пофрагментных операций          
      
      for (size_t i=0; i<MODE_NAMES_COUNT; i++)
        mode_states [i] = glIsEnabled (MODE_NAMES [i]) != 0;                

      glGetIntegerv (GL_COLOR_WRITEMASK, color_write_mask);
      glGetIntegerv (GL_STENCIL_WRITEMASK, &stencil_write_mask);
      glGetIntegerv (GL_DEPTH_WRITEMASK, &depth_write_mask);

        //отключение пофрагментных операций      

      for (size_t i=0; i<MODE_NAMES_COUNT; i++)
        glDisable (MODE_NAMES [i]);

      glColorMask   (1, 1, 1, 1);
      glDepthMask   (GL_TRUE);
      glStencilMask (~0);
      
      ClearErrors ();      
    }
    
    ~RenderBufferTempState ()
    {
        //восстановление состояния параметров пофрагментных операций
        
      for (size_t i=0; i<MODE_NAMES_COUNT; i++)
        if (mode_states [i]) glEnable (MODE_NAMES [i]);
        else                 glDisable (MODE_NAMES [i]);

      glColorMask   (color_write_mask [0], color_write_mask [1], color_write_mask [2], color_write_mask [3]);
      glDepthMask   (depth_write_mask);
      glStencilMask (stencil_write_mask);

      ClearErrors ();      
    }
    
  private:
      //очистка ошибки, связанной с использованием неподдерживаемых расширений  
    void ClearErrors ()
    {
      while (glGetError () != GL_NO_ERROR);      
    }
    
  private:
    bool mode_states [MODE_NAMES_COUNT];
    int  color_write_mask [4];
    int  stencil_write_mask;
    int  depth_write_mask;    
};

/*
    Пиксель с упакованными компонентой глубины и индексом трафарета
*/

struct Depth24Stencil8
{
  size_t depth_component : 24;
  size_t stencil_index : 8;
  
  void Set (size_t in_depth_component, unsigned char in_stencil_index)
  {
    depth_component = in_depth_component >> 8;
    stencil_index   = in_stencil_index;
  }
};

}

/*
    Конструктор
*/

RenderBuffer::RenderBuffer (const ContextManager& manager, RenderBufferType in_target)
  : ContextObject (manager), target (in_target)
{
  PixelFormat format;
  size_t      bind_flags;

  switch (target)
  {
    case RenderBufferType_Color:
      format     = PixelFormat_RGBA8;
      bind_flags = BindFlag_RenderTarget;
      break;
    case RenderBufferType_DepthStencil:
      format     = PixelFormat_D24S8;
      bind_flags = BindFlag_DepthStencil;
      break;
    default:
      RaiseInvalidArgument ("render::low_level::opengl::RenderBuffer::RenderBuffer", "target", target);
      break;
  }

  desc.dimension            = TextureDimension_2D;
  desc.width                = 0;
  desc.height               = 0;
  desc.layers               = 1;
  desc.format               = format;
  desc.generate_mips_enable = false;
  desc.access_flags         = AccessFlag_Read | AccessFlag_Write;  
  desc.bind_flags           = bind_flags;
  desc.usage_mode           = UsageMode_Static;
}

/*
    Получение дескриптора
*/

void RenderBuffer::GetDesc (TextureDesc& out_desc)
{
  out_desc = desc;
  
  GetSize (out_desc.width, out_desc.height);
}

/*
    Работа с данными
*/

namespace
{

GLenum get_glformat (PixelFormat format, const char* source, const char* param)
{
  switch (format)
  {
    case PixelFormat_RGB8:  return GL_RGB;
    case PixelFormat_RGBA8: return GL_RGBA;
    case PixelFormat_L8:    return GL_LUMINANCE;
    case PixelFormat_A8:    return GL_ALPHA;
    case PixelFormat_LA8:   return GL_LUMINANCE_ALPHA;
    case PixelFormat_DXT1:
    case PixelFormat_DXT3:
    case PixelFormat_DXT5:    
    case PixelFormat_D16:
    case PixelFormat_D24X8:
    case PixelFormat_D24S8:
    case PixelFormat_S8:
      RaiseNotSupported (source, "Unsupported %s=%s", param, get_name (format));
      return 0;
    default:
      RaiseInvalidArgument (source, param, format);
      return 0;
  }
}

}

void RenderBuffer::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::RenderBuffer::SetData";
  
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
  
  RenderBufferTempState temp_state;

    //проверка наличия расширения GL_ARB_window_pos либо версии OpenGL не ниже 1.4

  static Extension ARB_window_pos = "GL_ARB_window_pos",
                   Version_1_4    = "GL_VERSION_1_4";

  if (!IsSupported (ARB_window_pos) && !IsSupported (Version_1_4))
    RaiseNotSupported (METHOD_NAME, "Can not set image at position (%u;%u) (GL_ARB_window_pos not supported)", x, y);

  if      (glWindowPos2iARB) glWindowPos2iARB (x, y);
  else if (glWindowPos2i)    glWindowPos2i    (x, y);
  else                       return;  
  
  switch (target)
  {
    case RenderBufferType_Color:
    {
        //преобразование формата пикселей

      GLenum format = get_glformat (source_format, METHOD_NAME, "source_format"); 

        //копирование      

      glDrawPixels (width, height, format, GL_UNSIGNED_BYTE, buffer);
      
      break;
    }
    case RenderBufferType_DepthStencil:
    {
      switch (source_format)
      {
        case PixelFormat_D16:
        {
          xtl::uninitialized_storage<size_t> depth_buffer (width * height);

          size_t*               dst_pixel = depth_buffer.data ();
          const unsigned short* src_pixel = static_cast<const unsigned short*> (buffer);

          for (size_t count=width*height; count--; src_pixel++, dst_pixel++)
            *dst_pixel = size_t (*src_pixel) << 16;

          glDrawPixels (width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, depth_buffer.data ());

          break;
        }
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
            
          static Extension EXT_packed_depth_stencil = "GL_EXT_packed_depth_stencil";        

          if (IsSupported (EXT_packed_depth_stencil))
          {
            glDrawPixels (width, height, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, buffer);
          }
          else
          {
              //разделение данных буфера на компоненты глубины и трафарета

            xtl::uninitialized_storage<size_t>        depth_buffer (width * height);
            xtl::uninitialized_storage<unsigned char> stencil_buffer (width * height);    
            
            const Depth24Stencil8* src_pixel         = static_cast<const Depth24Stencil8*> (buffer);
            size_t*                dst_depth_pixel   = depth_buffer.data ();
            unsigned char*         dst_stencil_pixel = stencil_buffer.data ();
      
            for (size_t count=width*height; count--; src_pixel++, dst_depth_pixel++, dst_stencil_pixel++)
            {
              *dst_depth_pixel   = (size_t)(src_pixel->depth_component) << 8;
              *dst_stencil_pixel = src_pixel->stencil_index;
            }

              //копирование

            glDrawPixels (width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, depth_buffer.data ());
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
          RaiseNotSupported (METHOD_NAME, "Unsupported source_format=%s", get_name (source_format));
          return;
        default:
          RaiseInvalidArgument (METHOD_NAME, "source_format", source_format);
          return;
      }      

      break;
    }
    default: break;
  }    

    //проверка состояния OpenGL

  CheckErrors (METHOD_NAME);
}

void RenderBuffer::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::RenderBuffer::GetData";

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
  
  switch (target)
  {
    case RenderBufferType_Color:
    {
        //преобразование формата пикселей

      GLenum format = get_glformat (target_format, METHOD_NAME, "target_format");  

        //копирование

      glReadPixels (x, y, width, height, format, GL_UNSIGNED_BYTE, buffer);
      
      break;
    }
    case RenderBufferType_DepthStencil:
    {
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
            
          static Extension EXT_packed_depth_stencil = "GL_EXT_packed_depth_stencil";

          if (IsSupported (EXT_packed_depth_stencil))
          {
            glReadPixels (x, y, width, height, GL_DEPTH_STENCIL_EXT, GL_UNSIGNED_INT_24_8_EXT, buffer);
          }
          else
          {
              //создание буферов для хранения разделенных данных глубины и трафарета

            xtl::uninitialized_storage<size_t>        depth_buffer (width * height);
            xtl::uninitialized_storage<unsigned char> stencil_buffer (width * height);

              //копирование

            glReadPixels (x, y, width, height, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, depth_buffer.data ());
            glReadPixels (x, y, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, stencil_buffer.data ());

              //упаковка прочитанных данных

            Depth24Stencil8*     dst_pixel         = static_cast<Depth24Stencil8*> (buffer);
            const size_t*        src_depth_pixel   = depth_buffer.data ();
            const unsigned char* src_stencil_pixel = stencil_buffer.data ();

            for (size_t count=width*height; count--; dst_pixel++, src_depth_pixel++, src_stencil_pixel++)
              dst_pixel->Set (*src_depth_pixel, *src_stencil_pixel);
          }

          break;
        }
        case PixelFormat_S8:
          glReadPixels (x, y, width, height, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, buffer);
          break;
        case PixelFormat_RGB8:
        case PixelFormat_RGBA8:
        case PixelFormat_L8:
        case PixelFormat_A8:
        case PixelFormat_LA8:
        case PixelFormat_DXT1:
        case PixelFormat_DXT3:
        case PixelFormat_DXT5:
          RaiseNotSupported (METHOD_NAME, "Unsupported target_format=%s", get_name (target_format));
          return;
        default:
          RaiseInvalidArgument (METHOD_NAME, "target_format", target_format);
          return;
      }      

      break;
    }
    default: break;
  }  

    //проверка состояния OpenGL

  CheckErrors (METHOD_NAME);
}
