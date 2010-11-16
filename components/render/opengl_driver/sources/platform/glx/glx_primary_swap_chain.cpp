#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

namespace
{

/*
    Константы
*/

const size_t CONFIG_MAX_ATTRIBUTES = 128; //максимальное количество атрибутов в конфигурации

}

/*
    Описание реализации первичной цепочки обмена
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef Output::Pointer       OutputPtr;

struct PrimarySwapChain::Impl
{
  AdapterPtr    adapter;     //адаптер, которому принадлежит устройство
  Log           log;         //протокол
  Display*      display;     //соединение с дисплеем
  SwapChainDesc desc;        //дескриптор цепочки обмена
  PropertyList  properties;  //свойства цепочки обмена

///Конструктор
  Impl (Adapter* in_adapter, const SwapChainDesc& in_desc)
    : adapter (in_adapter)
    , display (DisplayManager::DisplayHandle ())
  {
    try
    {
        //проверка возможности работы GLX
        
      int error_base, event_base;        
        
      if (!glXQueryExtension (display, &error_base, &event_base ) )
      {
          fprintf(stderr, "glxsimple: %s\n", "X server has no OpenGL GLX extension");
          exit(1);
      }
        
      
        //выбор конфигурации

      glxint config_attributes [CONFIG_MAX_ATTRIBUTES], *attr = config_attributes;
      
      *attr++ = glx_BUFFER_SIZE;
      *attr++ = in_desc.frame_buffer.color_bits;
      *attr++ = glx_ALPHA_SIZE;
      *attr++ = in_desc.frame_buffer.alpha_bits;      
      *attr++ = glx_DEPTH_SIZE;
      *attr++ = in_desc.frame_buffer.depth_bits;
//      *attr++ = glx_STENCIL_SIZE; //for tests only!!!!!!!
//      *attr++ = in_desc.frame_buffer.stencil_bits;
      *attr++ = glx_SAMPLES;
      *attr++ = in_desc.samples_count;
      
      switch (in_desc.swap_method)
      {
        case SwapMethod_Discard:
        case SwapMethod_Flip:
        case SwapMethod_Copy:
          break;
        default:
          throw xtl::make_argument_exception ("", "desc.swap_method", desc.swap_method);
      }            
      
      *attr++ = glx_SURFACE_TYPE;
      *attr++ = glx_WINDOW_BIT;
      *attr++ = glx_NONE;      

      glxint configs_count = 0;

      if (!glxChooseConfig (glx_display, config_attributes, &glx_config, 1, &configs_count))
        raise_error ("::glxChooseConfig");

      if (!configs_count)
        throw xtl::format_operation_exception ("", "Bad glx configuration (RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
          in_desc.frame_buffer.color_bits, in_desc.frame_buffer.alpha_bits, in_desc.frame_buffer.depth_bits,
          in_desc.frame_buffer.stencil_bits, in_desc.samples_count);
        
      log.Printf ("...choose configuration #%u (RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
        glx_config, in_desc.frame_buffer.color_bits, in_desc.frame_buffer.alpha_bits, in_desc.frame_buffer.depth_bits,
        in_desc.frame_buffer.stencil_bits, in_desc.samples_count);
        
      log.Printf ("...create window surface");
        
        //создание поверхности отрисовки

      glx_surface = glxCreateWindowSurface (glx_display, glx_config, (NativeWindowType)output->GetWindowHandle (), 0);
      
      if (!glx_surface)
        raise_error ("::glxCreateWindowSurface");

        //сохранение дескриптора устройства        

      desc = in_desc;

      desc.frame_buffer.width        = GetSurfaceAttribute (glx_WIDTH);
      desc.frame_buffer.height       = GetSurfaceAttribute (glx_HEIGHT);
      desc.frame_buffer.color_bits   = GetConfigAttribute (glx_BUFFER_SIZE);
      desc.frame_buffer.alpha_bits   = GetConfigAttribute (glx_ALPHA_SIZE);
      desc.frame_buffer.depth_bits   = GetConfigAttribute (glx_DEPTH_SIZE);
      desc.frame_buffer.stencil_bits = GetConfigAttribute (glx_STENCIL_SIZE);
      desc.samples_count             = GetConfigAttribute (glx_SAMPLES);
      desc.buffers_count             = 1;
      desc.fullscreen                = false;
      
        //установка свойств цепочки обмена
        
      try
      {
        properties.AddProperty ("glx_vendor",      GetglxString (glx_VENDOR));
        properties.AddProperty ("glx_version",     GetglxString (glx_VERSION));
        properties.AddProperty ("glx_extensions",  GetglxString (glx_EXTENSIONS));
        properties.AddProperty ("glx_client_apis", GetglxString (glx_CLIENT_APIS));
      }
      catch (...)
      {
        //исключения при взятии свойств glx не являются критичными для работы
        //(обход бага glx для bada)
      }
    }
    catch (...)
    {
      if (glx_surface)
        glxDestroySurface (glx_display, glx_surface);

      throw;
    }
  }
  
///Деструктор
  ~Impl ()
  {
    glxDestroySurface (glx_display, glx_surface);
  }
  
///Получение атрибута
  glxint GetConfigAttribute (glxint attribute)
  {
    glxint value = 0;
    
    if (!glxGetConfigAttrib (glx_display, glx_config, attribute, &value))
      raise_error ("::glxGetConfigAttrib");
      
    return value;
  }
  
///Получение значения атрибута поверхности отрисовки
  glxint GetSurfaceAttribute (glxint attribute)
  {
    glxint value = 0;
    
    if (!glxQuerySurface (glx_display, glx_surface, attribute, &value))
      raise_error ("::glxQuerySurface");
      
    return value;
  }
  
///Получение строкового свойства glx
  const char* GetglxString (glxint name)
  {
    const char* value = glxQueryString (glx_display, name);
    
    if (!value)
      raise_error ("::glxQueryString");
      
    return value;
  }
};

/*
    Конструктор / деструктор
*/

PrimarySwapChain::PrimarySwapChain (Adapter* adapter, const SwapChainDesc& desc)
{
  try
  {
    if (!adapter)
      throw xtl::make_null_argument_exception ("", "adapter");
    
    impl = new Impl (adapter, desc);        

    impl->log.Printf ("...swap chain successfully created");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::PrimarySwapChain");
    throw;
  }
}

PrimarySwapChain::~PrimarySwapChain ()
{
}

/*
    Получение адаптера
*/

IAdapter* PrimarySwapChain::GetAdapter ()
{
  return impl->adapter.get ();
}

/*
    Получение дескриптора
*/

void PrimarySwapChain::GetDesc (SwapChainDesc& out_desc)
{
  out_desc = impl->desc;
}

/*
    Получение устройства вывода с максимальным размером области перекрытия
*/

IOutput* PrimarySwapChain::GetContainingOutput ()
{
  return impl->output.get ();
}

/*
    Установка / взятие состояния full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::SetFullscreenState");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::GetFullscreenState");
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void PrimarySwapChain::Present ()
{
  try
  {
    if (!glxSwapBuffers (impl->glx_display, impl->glx_surface))
      raise_error ("::glxSwapBuffers");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::Present");
    throw;
  }
}

/*
    Список свойств устройства вывода
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}

/*
    Получение glx параметров цепочки обмена
*/

glxDisplay PrimarySwapChain::GetglxDisplay ()
{
  return impl->output->GetglxDisplay ();
}

glxConfig PrimarySwapChain::GetglxConfig ()
{
  return impl->glx_config;
}

glxSurface PrimarySwapChain::GetglxSurface ()
{
  return impl->glx_surface;
}
