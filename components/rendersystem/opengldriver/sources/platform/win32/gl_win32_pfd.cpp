#include "shared.h"

using namespace common;

namespace render
{

namespace low_level
{

namespace opengl
{

namespace
{

//установка атрибута в таблице формата пикселей
inline void set_attribute (int*& iter, int name, int value)
{
  iter [0]  = name;
  iter [1]  = value;
  iter     += 2;
}

}

//выбор формата пикселей
int choose_pixel_format (HDC device_context, const SwapChainDesc& swap_chain_desc)
{
  try
  {
    int pixel_format = 0;
    
      //заполнение стандартной структуры дескриптора формата пикселей
    
    PIXELFORMATDESCRIPTOR pfd;

    memset (&pfd, 0, sizeof (pfd));

    pfd.nSize        = sizeof (pfd);
    pfd.nVersion     = 1;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | (swap_chain_desc.buffers_count > 1 ? PFD_DOUBLEBUFFER : 0);
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cColorBits   = swap_chain_desc.frame_buffer.color_bits;
    pfd.cDepthBits   = swap_chain_desc.frame_buffer.depth_bits;
    pfd.cStencilBits = swap_chain_desc.frame_buffer.stencil_bits;
    pfd.cAccumBits   = 0;
    pfd.cAlphaBits   = swap_chain_desc.frame_buffer.alpha_bits;
    pfd.iLayerType   = PFD_MAIN_PLANE;    

    if (wglewGetContext () && wglChoosePixelFormatARB)
    {
        //заполнение таблицы формата пикселей
        
      const size_t INTEGER_ATTRIBUTES_TABLE_SIZE = 64;      

      int integer_attributes [INTEGER_ATTRIBUTES_TABLE_SIZE], *iter = integer_attributes;
     
      set_attribute (iter, WGL_PIXEL_TYPE_ARB,      WGL_TYPE_RGBA_ARB);
      set_attribute (iter, WGL_DOUBLE_BUFFER_ARB,   swap_chain_desc.buffers_count > 1);
      set_attribute (iter, WGL_SUPPORT_OPENGL_ARB,  true);
      set_attribute (iter, WGL_DRAW_TO_WINDOW_ARB,  true);
      set_attribute (iter, WGL_COLOR_BITS_ARB,      swap_chain_desc.frame_buffer.color_bits);
      set_attribute (iter, WGL_ALPHA_BITS_ARB,      swap_chain_desc.frame_buffer.alpha_bits);
      set_attribute (iter, WGL_DEPTH_BITS_ARB,      swap_chain_desc.frame_buffer.depth_bits);
      set_attribute (iter, WGL_STENCIL_BITS_ARB,    swap_chain_desc.frame_buffer.stencil_bits);
      set_attribute (iter, WGL_ACCELERATION_ARB,    WGL_FULL_ACCELERATION_ARB);
      
      switch (swap_chain_desc.swap_method)
      {
        case SwapMethod_Copy:
          set_attribute (iter, WGL_SWAP_METHOD_ARB, WGL_SWAP_COPY_ARB);
          break;
        case SwapMethod_Flip:
          set_attribute (iter, WGL_SWAP_METHOD_ARB, WGL_SWAP_EXCHANGE_ARB);
          break;
        default:
        case SwapMethod_Discard:
          set_attribute (iter, WGL_SWAP_METHOD_ARB, WGL_SWAP_UNDEFINED_ARB);
          break;
      }

      if (WGLEW_ARB_pbuffer)
        set_attribute (iter, WGL_DRAW_TO_PBUFFER_ARB, true);

      if (WGLEW_ARB_multisample && swap_chain_desc.samples_count)
      {
        set_attribute (iter, WGL_SAMPLE_BUFFERS_ARB, true);
        set_attribute (iter, WGL_SAMPLES_ARB,        swap_chain_desc.samples_count);
      }

      set_attribute (iter, 0, 0); //end of attribute list

      size_t formats_count = 0;

      float float_attributes [] = {0, 0};

      if (!wglChoosePixelFormatARB (device_context, integer_attributes, float_attributes, 1, &pixel_format, &formats_count))
        raise_error ("wglChoosePixelFormatARB");
    }
    
    if (!pixel_format)
    {
        //попытка инициализации стандартными средствами      
      
      pixel_format = ChoosePixelFormat (device_context, &pfd);

      if (!pixel_format)
        raise_error ("ChoosePixelFormat");
    }
    
    return pixel_format;
  }
  catch (common::Exception& exception)
  {
    const char* swap_method_name = "unknown";
    
    switch (swap_chain_desc.swap_method)
    {
      case SwapMethod_Discard: swap_method_name = "discard"; break;
      case SwapMethod_Flip:    swap_method_name = "flip"; break;
      case SwapMethod_Copy:    swap_method_name = "copy"; break;
    }

    exception.Touch ("render::low_level::opengl::choose_pixel_format(color=%u, alpha=%u, depth=%u, stencil=%u, "
      "samples=%u, buffers=%u, swap=%s%s%s)", swap_chain_desc.frame_buffer.color_bits, swap_chain_desc.frame_buffer.alpha_bits,
      swap_chain_desc.frame_buffer.depth_bits, swap_chain_desc.frame_buffer.stencil_bits, swap_chain_desc.samples_count,
      swap_chain_desc.buffers_count, swap_method_name, swap_chain_desc.fullscreen ? ", fullscreen" : "",
      swap_chain_desc.vsync ? ", vsync" : "");

    throw;
  }
}

//получение формата пикселей
void get_pixel_format (HDC device_context, int pixel_format, SwapChainDesc& swap_chain_desc)
{
    //очистка структуры swap_chain_desc

  memset (&swap_chain_desc, 0, sizeof (swap_chain_desc));  

    //получение номера формата пикселей

  if (wglewGetContext () && wglGetPixelFormatAttribivARB) //WGLEW already init
  {
      //получение формата пикселей с использованием расширений

    static int basic_names [] = {
      WGL_COLOR_BITS_ARB,
      WGL_ALPHA_BITS_ARB,
      WGL_DEPTH_BITS_ARB,
      WGL_STENCIL_BITS_ARB,
      WGL_DOUBLE_BUFFER_ARB,
      WGL_SWAP_METHOD_ARB
    };

    static const int BASIC_NAMES_COUNT = sizeof (basic_names) / sizeof (GLenum);

    int value_buffer [BASIC_NAMES_COUNT], *iter = value_buffer;

    if (!wglGetPixelFormatAttribivARB (device_context, pixel_format, 0, BASIC_NAMES_COUNT, basic_names, value_buffer))
      raise_error ("wglGetPixelFormatAttribivARB");

    swap_chain_desc.frame_buffer.color_bits   = *iter++;
    swap_chain_desc.frame_buffer.alpha_bits   = *iter++;
    swap_chain_desc.frame_buffer.depth_bits   = *iter++;
    swap_chain_desc.frame_buffer.stencil_bits = *iter++;
    swap_chain_desc.buffers_count             = *iter++ ? 2 : 1;
    
    switch (*iter++)
    {
      case WGL_SWAP_EXCHANGE_ARB:   swap_chain_desc.swap_method = SwapMethod_Flip; break;
      case WGL_SWAP_COPY_ARB:       swap_chain_desc.swap_method = SwapMethod_Copy; break;
      default:
      case WGL_SWAP_UNDEFINED_ARB:  swap_chain_desc.swap_method = SwapMethod_Discard; break;
    }

    if (WGLEW_ARB_multisample)
    {
      static int multisample_name = WGL_SAMPLES_ARB;

      int samples_count = 0;

      if (!wglGetPixelFormatAttribivARB (device_context, pixel_format, 0, 1, &multisample_name, &samples_count))
        raise_error ("wglGetPixelFormatAttribivARB");

      swap_chain_desc.samples_count = samples_count;
    }
  }
  else
  {
      //получение формата пикселей стандартным способом
    
    PIXELFORMATDESCRIPTOR pfd;

    if (!DescribePixelFormat (device_context, pixel_format, sizeof (pfd), &pfd))
      raise_error ("DescribePixelFormat");

    swap_chain_desc.frame_buffer.color_bits   = pfd.cColorBits;
    swap_chain_desc.frame_buffer.alpha_bits   = pfd.cAlphaBits;
    swap_chain_desc.frame_buffer.depth_bits   = pfd.cDepthBits;
    swap_chain_desc.frame_buffer.stencil_bits = pfd.cStencilBits;
    swap_chain_desc.buffers_count             = pfd.dwFlags & PFD_DOUBLEBUFFER ? 2 : 1;
    swap_chain_desc.swap_method               = SwapMethod_Discard;
  }
}

}

}

}
