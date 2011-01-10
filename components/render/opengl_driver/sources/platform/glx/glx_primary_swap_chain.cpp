#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

/*
    Получение конигурации буфера кадра по заданному id
*/

namespace
{

GLXFBConfig get_fb_config (AdapterLibrary& library, Display *display, int screen, int fbconfig_id)
{
  DisplayLock lock (display);
  
  int attrib_list[] =
  {
    GLX_FBCONFIG_ID, fbconfig_id,
    None
  };

  int nelements = 0;
        
  GLXFBConfig* config = library->ChooseFBConfig (display, screen, attrib_list, &nelements);
  
  if (!config)
    throw xtl::format_operation_exception ("render::low_level::opengl::glx::get_fb_config",
      "glXChooseFBConfig failed");
      
  GLXFBConfig fb_config = config [0];

  XFree (config);
  
  return fb_config;
}

}

/*
    Описание реализации первичной цепочки обмена
*/

typedef xtl::com_ptr<Adapter> AdapterPtr;
typedef Output::Pointer       OutputPtr;

struct PrimarySwapChain::Impl
{
  Log                 log;                     //протокол
  AdapterPtr          adapter;                 //адаптер, которому принадлежит устройство
  AdapterLibraryPtr   library;                 //библиотека адаптера
  GlxExtensionEntries glx_extension_entries;   //таблица WGL-расширений
  int                 pixel_format_index;      //индекс формата пикселей устройства вывода
  SwapChainDesc       desc;                    //дескриптор цепочки обмена
  Display*            display;                 //соединение с дисплеем
  Window              window;                  //окно
  GLXFBConfig         glx_fb_config;           //конфигурация буфера кадра
  PropertyList        properties;              //свойства цепочки обмена

///Конструктор
  Impl (const SwapChainDesc& in_desc, const PixelFormatDesc& pixel_format)
    : adapter (pixel_format.adapter)
    , library (&adapter->GetLibrary ())
    , pixel_format_index (pixel_format.pixel_format_index)
    , display ((Display*)syslib::x11::DisplayManager::DisplayHandle ())
    , window  ((Window)in_desc.window_handle)
  {
    try
    {
        //инициализация таблицы расширений

      if (pixel_format.glx_extension_entries)
      {
        glx_extension_entries = *pixel_format.glx_extension_entries;
      }
      else
      {
        memset (&glx_extension_entries, 0, sizeof glx_extension_entries);
      }

        //установка состояния FullScreen

      if (in_desc.fullscreen)
      {
        log.Printf ("...set fullscreen mode");
        
        SetFullscreenState (true);
      }
      
        //инициализация конфигурации буфера кадра
        
      glx_fb_config = get_fb_config (library, display, get_screen_number (window), pixel_format_index);
            
        //инициализация дескриптора цепочки обмена
        
      desc.frame_buffer.width        = XWidthOfScreen (get_screen (window));
      desc.frame_buffer.height       = XHeightOfScreen (get_screen (window));
      desc.frame_buffer.color_bits   = pixel_format.color_bits;
      desc.frame_buffer.alpha_bits   = pixel_format.alpha_bits;
      desc.frame_buffer.depth_bits   = pixel_format.depth_bits;
      desc.frame_buffer.stencil_bits = pixel_format.stencil_bits;
      desc.samples_count             = pixel_format.samples_count;
      desc.buffers_count             = pixel_format.buffers_count;
      desc.swap_method               = pixel_format.swap_method;
      desc.fullscreen                = in_desc.fullscreen;
      desc.vsync                     = in_desc.vsync;
      desc.window_handle             = in_desc.window_handle;
    }
    catch (...)
    {
      throw;
    }
  }
  
///Деструктор
  ~Impl ()
  {
    log.Printf ("...release resources");
  }
  
  void SetFullscreenState (bool state)
  {
    throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::impl::SetFullscreenState");
  }
  
  bool GetFullscreenState ()
  {
    throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::impl::GetFullscreenState");
  }
  
  IOutput* GetContainingOutput ()
  {
    throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::PrimarySwapChain::impl::GetContainingOutput");
  }

  void Present ()
  {  
    try
    {
      library->SwapBuffers (display, window);
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::low_level::opengl::glx::PrimarySwapChain::impl::Present");
      throw;
    }
  }
};

/*
    Конструктор / деструктор
*/

PrimarySwapChain::PrimarySwapChain (const SwapChainDesc& sc_desc, const PixelFormatDesc& pf_desc)
{
  try
  {
    impl = new Impl (sc_desc, pf_desc);

    impl->log.Printf ("...primary swap chain (id=%u) successfully created", GetId ());
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

Adapter* PrimarySwapChain::GetAdapterImpl ()
{
  return &*impl->adapter;
}

/*
    Устройство отображения для текущего контекста
*/

Display* PrimarySwapChain::GetDisplay ()
{
  return impl->display;
}

/*
    Окно отрисовки
*/

Window PrimarySwapChain::GetWindow ()
{
  return impl->window;
}

/*
    Получение таблицы GLX-расширений
*/

const GlxExtensionEntries& PrimarySwapChain::GetGlxExtensionEntries ()
{
  return impl->glx_extension_entries;
}

/*
    Конфигурация буфера кадра
*/

GLXFBConfig PrimarySwapChain::GetFBConfig ()
{
  return impl->glx_fb_config;
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
  return impl->GetContainingOutput ();
}

/*
    Установка / взятие состояния full-screen mode
*/

void PrimarySwapChain::SetFullscreenState (bool state)
{
  impl->SetFullscreenState (state);
}

bool PrimarySwapChain::GetFullscreenState ()
{
  return impl->GetFullscreenState ();
}

/*
    Обмен текущего заднего буфера и переднего буфера
*/

void PrimarySwapChain::Present ()
{
  impl->Present ();
}

/*
    Список свойств устройства вывода
*/

IPropertyList* PrimarySwapChain::GetProperties ()
{
  return &impl->properties;
}
