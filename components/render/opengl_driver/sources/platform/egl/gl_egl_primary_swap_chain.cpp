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

typedef xtl::com_ptr<Adapter>     AdapterPtr;
typedef Output::Pointer           OutputPtr;
typedef stl::auto_ptr<EglSurface> EglSurfacePtr;

struct PrimarySwapChain::Impl
{
  AdapterPtr    adapter;            //адаптер, которому принадлежит устройство
  OutputPtr     output;             //целевое устройство вывода
  EglSurfacePtr egl_surface_holder; //хранилище поверхности отрисовки
  Log           log;                //протокол
  EGLDisplay    egl_display;        //целевое устройство вывода
  EGLConfig     egl_config;         //целевая конфигурация
  EGLSurface    egl_surface;        //целевая поверхность отрисовки
  SwapChainDesc desc;               //дескриптор цепочки обмена
  PropertyList  properties;         //свойства цепочки обмена

///Конструктор
  Impl (Adapter* in_adapter, const SwapChainDesc& in_desc)
    : adapter (in_adapter)
    , output (adapter->GetOutput (in_desc.window_handle))
    , egl_display (output->GetEglDisplay ())
    , egl_config (0)
    , egl_surface (0)    
  {
    DisplayLock lock (output->GetDisplay ());
    
      //выбор конфигурации

    desc = in_desc;

#ifdef TABLETOS
    desc.frame_buffer.color_bits   = 24;
    desc.frame_buffer.depth_bits   = 24;
//      desc.frame_buffer.alpha_bits   = 8;
//      desc.frame_buffer.stencil_bits = 8;
#endif

    egl_config = ChooseConfig (desc);

    if (!egl_config)
      throw xtl::format_operation_exception ("", "Bad EGL configuration (RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
        in_desc.frame_buffer.color_bits, in_desc.frame_buffer.alpha_bits, in_desc.frame_buffer.depth_bits,
        in_desc.frame_buffer.stencil_bits, in_desc.samples_count);

    desc.frame_buffer.alpha_bits   = GetConfigAttribute (EGL_ALPHA_SIZE);
    desc.frame_buffer.color_bits   = GetConfigAttribute (EGL_BUFFER_SIZE) - desc.frame_buffer.alpha_bits;
    desc.frame_buffer.depth_bits   = GetConfigAttribute (EGL_DEPTH_SIZE);
    desc.frame_buffer.stencil_bits = GetConfigAttribute (EGL_STENCIL_SIZE);
    desc.samples_count             = GetConfigAttribute (EGL_SAMPLES);
    desc.buffers_count             = 2;
    desc.fullscreen                = false;
      
    EGLint format = 0;
        
    eglGetConfigAttrib (egl_display, egl_config, EGL_NATIVE_VISUAL_ID, &format);                
      
    log.Printf ("...choose configuration #%u (VisualId: %d, RGB/A: %u/%u, D/S: %u/%u, Samples: %u)",
      egl_config, format, desc.frame_buffer.color_bits, desc.frame_buffer.alpha_bits, desc.frame_buffer.depth_bits,
      desc.frame_buffer.stencil_bits, desc.samples_count);

#ifdef TABLETOS
    log.Printf ("...setup window");              

    setup_window (output->GetWindowHandle (), desc, log);
#endif
      
    log.Printf ("...create window surface");              

    egl_surface_holder.reset (new EglSurface (egl_display, egl_config, output->GetWindowHandle ()));

    egl_surface = egl_surface_holder->GetSurface ();      

      //сохранение дескриптора устройства        

    desc.frame_buffer.width  = GetSurfaceAttribute (EGL_WIDTH);
    desc.frame_buffer.height = GetSurfaceAttribute (EGL_HEIGHT);

      //установка свойств цепочки обмена
      
    try
    {
      properties.AddProperty ("egl_vendor",      GetEglString (EGL_VENDOR));
      properties.AddProperty ("egl_version",     GetEglString (EGL_VERSION));
      properties.AddProperty ("egl_extensions",  GetEglString (EGL_EXTENSIONS));
      properties.AddProperty ("egl_client_apis", GetEglString (EGL_CLIENT_APIS));
    }
    catch (...)
    {
      //исключения при взятии свойств EGL не являются критичными для работы
      //(обход бага egl для bada)
    }
  }
  
///Деструктор
  ~Impl ()
  {
    try
    {    
      DisplayLock lock (output->GetDisplay ());

      eglWaitGL ();
    }
    catch (...)
    {
    }
  }

///Сравнение двух количественных показателей (0 - первый формат более подходит, 1 - второй формат более подходит)
  static int CompareFormatCounts (size_t source1, size_t source2, size_t require)
  {
    if (source1 == require)
      return 0;

    if (source2 == require)
      return 1;

    if (source1 < require)
    {
       if (source2 < require) return source1 < source2;
       else                   return 1;
    }
    else //source1 > require
    {
      if (source2 > require) return source1 > source2;
      else                   return 0;
    }
  }

///Сравнение двух форматов (0 - первый формат более подходит, 1 - второй формат более подходит)
  static int CompareFormats (const PixelFormatDesc& fmt0, const PixelFormatDesc& fmt1, const SwapChainDesc& swap_chain_desc)
  {
      //упорядочивание по типу ускорения

    if (fmt0.acceleration != fmt1.acceleration)
      return fmt0.acceleration < fmt1.acceleration;

      //упорядочивание по количеству битов цвета
      
    if (fmt0.color_bits != fmt1.color_bits)
      return CompareFormatCounts (fmt0.color_bits, fmt1.color_bits, swap_chain_desc.frame_buffer.color_bits);

      //упорядочивание по количеству битов альфы

    if (fmt0.alpha_bits != fmt1.alpha_bits)
      return CompareFormatCounts (fmt0.alpha_bits, fmt1.alpha_bits, swap_chain_desc.frame_buffer.alpha_bits);

      //упорядочивание по количеству битов глубины

    if (fmt0.depth_bits != fmt1.depth_bits)
      return CompareFormatCounts (fmt0.depth_bits, fmt1.depth_bits, swap_chain_desc.frame_buffer.depth_bits);

      //упорядочивание по количеству битов трафарета

    if (fmt0.stencil_bits != fmt1.stencil_bits)
      return CompareFormatCounts (fmt0.stencil_bits, fmt1.stencil_bits, swap_chain_desc.frame_buffer.stencil_bits);

      //упорядочивание по количеству сэмплов

    if (fmt0.samples_count != fmt1.samples_count)
      return CompareFormatCounts (fmt0.samples_count, fmt1.samples_count, swap_chain_desc.samples_count);

      //при прочих равных первый формат более подходит

    return 1;
  }

///Выбор конфигурации
  EGLConfig ChooseConfig (const SwapChainDesc& swap_chain_desc)
  {
    try
    {
        //перечисление доступных форматов

      Adapter::PixelFormatArray formats;

      adapter->EnumPixelFormats (egl_display, formats);

        //выбор формата

      if (formats.empty ())
      {
        log.Printf ("...no pixel formats found");
        return (EGLConfig)0;
      }

      log.Printf ("...found %u pixel formats", formats.size ());

          //поиск формата

      const PixelFormatDesc* best = &formats [0];
        
      for (Adapter::PixelFormatArray::const_iterator iter=formats.begin ()+1, end=formats.end (); iter!=end; ++iter)
        if (CompareFormats (*best, *iter, swap_chain_desc))
          best = &*iter;

      return best->egl_config;
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::low_level::opengl::egl::PrimarySwapChain::Impl::ChooseConfig");
      throw;
    }
  }
  
///Получение атрибута
  EGLint GetConfigAttribute (EGLint attribute)
  {
    EGLint value = 0;
    
    if (!eglGetConfigAttrib (egl_display, egl_config, attribute, &value))
      raise_error ("::eglGetConfigAttrib");
      
    return value;
  }
  
///Получение значения атрибута поверхности отрисовки
  EGLint GetSurfaceAttribute (EGLint attribute)
  {
    EGLint value = 0;
    
    if (!eglQuerySurface (egl_display, egl_surface, attribute, &value))
      raise_error ("::eglQuerySurface");
      
    return value;
  }
  
///Получение строкового свойства EGL
  const char* GetEglString (EGLint name)
  {
    const char* value = eglQueryString (egl_display, name);
    
    if (!value)
      raise_error ("::eglQueryString");
      
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
    exception.touch ("render::low_level::opengl::egl::PrimarySwapChain::PrimarySwapChain");
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
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::SetFullscreenState");
}

bool PrimarySwapChain::GetFullscreenState ()
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::egl::PrimarySwapChain::GetFullscreenState");
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void PrimarySwapChain::Present ()
{
  try
  {
    DisplayLock lock (impl->output->GetDisplay ());        
    
#ifdef ANDROID    
    eglWaitGL ();    
#endif
    
    if (!eglSwapBuffers (impl->egl_display, impl->egl_surface))
      raise_error ("::eglSwapBuffers");                  
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::PrimarySwapChain::Present");
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
    Получение EGL параметров цепочки обмена
*/

EGLDisplay PrimarySwapChain::GetEglDisplay ()
{
  return impl->output->GetEglDisplay ();
}

EGLConfig PrimarySwapChain::GetEglConfig ()
{
  return impl->egl_config;
}

EGLSurface PrimarySwapChain::GetEglSurface ()
{
  return impl->egl_surface;
}

NativeDisplayType PrimarySwapChain::GetDisplay ()
{
  return impl->output->GetDisplay ();
}
