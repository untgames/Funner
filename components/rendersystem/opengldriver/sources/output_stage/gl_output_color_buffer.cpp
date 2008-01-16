#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace common;

/*
    Конструктор
*/

ColorBuffer::ColorBuffer (const ContextManager& manager)
  : ContextObject (manager)
{
  desc.dimension            = TextureDimension_2D;
  desc.width                = 0;
  desc.height               = 0;
  desc.layers               = 1;
  desc.format               = PixelFormat_RGBA8;
  desc.generate_mips_enable = false;
  desc.access_flags         = AccessFlag_Read | AccessFlag_Write;  
  desc.bind_flags           = BindFlag_RenderTarget;
  desc.usage_mode           = UsageMode_Static;
}

/*
    Получение дескриптора
*/

void ColorBuffer::GetDesc (TextureDesc& out_desc)
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

/*
    Временное хранилище изменяемых параметров OpenGL
*/

const GLenum MODE_NAMES [] = {GL_BLEND, GL_DEPTH_TEST, GL_STENCIL_TEST, GL_SCISSOR_TEST, GL_FOG, GL_TEXTURE_1D, GL_TEXTURE_2D,
                              GL_TEXTURE_3D, GL_TEXTURE_CUBE_MAP, GL_TEXTURE_RECTANGLE_EXT};
const size_t MODE_NAMES_COUNT = sizeof (MODE_NAMES) / sizeof (*MODE_NAMES);

struct ColorBufferTempState
{
  public:
    ColorBufferTempState ()
    {
        //сохранение состояния параметров пофрагментных операций          
      
      for (size_t i=0; i<MODE_NAMES_COUNT; i++)
        mode_states [i] = glIsEnabled (MODE_NAMES [i]) != 0;                

      glGetIntegerv (GL_COLOR_WRITEMASK, color_write_mask);

        //отключение пофрагментных операций      

      for (size_t i=0; i<MODE_NAMES_COUNT; i++)
        glDisable (MODE_NAMES [i]);

      glColorMask (1, 1, 1, 1);
      
      ClearErrors ();      
    }
    
    ~ColorBufferTempState ()
    {
        //восстановление состояния параметров пофрагментных операций
        
      for (size_t i=0; i<MODE_NAMES_COUNT; i++)
        if (mode_states [i]) glEnable (MODE_NAMES [i]);
        else                 glDisable (MODE_NAMES [i]);

      glColorMask (color_write_mask [0], color_write_mask [1], color_write_mask [2], color_write_mask [3]);

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
};

}

void ColorBuffer::SetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat source_format, const void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ColorBuffer::SetData";
  
  if (layer)
    RaiseOutOfRange (METHOD_NAME, "layer", layer, 1);

  if (mip_level)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, 1);

  if (!width || !height)
    return;
    
  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");

    //преобразование формата пикселей

  GLenum format = get_glformat (source_format, METHOD_NAME, "source_format");

    //установка буфера в контекст OpenGL

  Bind ();
  
  ColorBufferTempState temp_state;

    //проверка наличия расширения GL_ARB_window_pos либо версии OpenGL не ниже 1.4

  static Extension ARB_window_pos = "GL_ARB_window_pos",
                   Version_1_4    = "GL_VERSION_1_4";

  if (!IsSupported (ARB_window_pos) && !IsSupported (Version_1_4))
    RaiseNotSupported (METHOD_NAME, "Can not set image at position (%u;%u) (GL_ARB_window_pos not supported)", x, y);

  if      (glWindowPos2iARB) glWindowPos2iARB (x, y);
  else if (glWindowPos2i)    glWindowPos2i    (x, y);
  else                       return;  

    //копирование    

  glDrawPixels (width, height, format, GL_UNSIGNED_BYTE, buffer);  

    //проверка состояния OpenGL

  CheckErrors (METHOD_NAME);
}

void ColorBuffer::GetData (size_t layer, size_t mip_level, size_t x, size_t y, size_t width, size_t height, PixelFormat target_format, void* buffer)
{
  static const char* METHOD_NAME = "render::low_level::opengl::ColorBuffer::GetData";

  if (layer)
    RaiseOutOfRange (METHOD_NAME, "layer", layer, 1);
    
  if (mip_level)
    RaiseOutOfRange (METHOD_NAME, "mip_level", mip_level, 1);        

  if (!width || !height)
    return;

  if (!buffer)
    RaiseNullArgument (METHOD_NAME, "buffer");    
    
    //преобразование формата пикселей

  GLenum format = get_glformat (target_format, METHOD_NAME, "target_format");

    //установка буфера в контекст OpenGL

  Bind ();

    //копирование

  glReadPixels (x, y, width, height, format, GL_UNSIGNED_BYTE, buffer);

    //проверка состояния OpenGL

  CheckErrors (METHOD_NAME);
}
