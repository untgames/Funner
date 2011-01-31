#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

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
        
      glx_fb_config = pixel_format.config;
            
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
  
///Получение устройства вывода с максимальным размером области перекрытия
  IOutput* GetContainingOutput ()
  {
    return adapter->GetOutput (window).get ();
  }
  
  void SetFullscreenState (bool state)
  {
    static const char* METHOD_NAME = "render::low_level::opengl::glx::PrimarySwapChain::impl::SetFullscreenState";

    if (GetFullscreenState () == state)
      return;
      
    OutputPtr output = adapter->GetOutput (window);
    
    if (!output)
      return;
      
    if (state)
    {
      DisplayLock lock (display);
      
      Window root_return = 0;
      int x_return = 0, y_return = 0;
      unsigned int width_return = 0, height_return = 0, border_width_return = 0, depth_return = 0;
                
      if (!XGetGeometry (display, window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return))
        throw xtl::format_operation_exception (METHOD_NAME, "XGetGeometry failed");
                                
      OutputModeDesc mode_desc;
      
      output->GetCurrentMode (mode_desc);
      
      mode_desc.width  = width_return;
      mode_desc.height = height_return;
      
      output->SetCurrentMode (mode_desc);
    }
    else
    {    
      output->RestoreDefaultMode ();      
    }
  }
  
  bool GetFullscreenState ()
  {
    static const char* METHOD_NAME = "render::low_level::opengl::glx::PrimarySwapChain::impl::GetFullscreenState";
    
    OutputPtr output = adapter->GetOutput (window);

    if (!output)
      return false;

    OutputModeDesc mode_desc;
    
    output->GetCurrentMode (mode_desc);
    
    Window root_return = 0;
    int x_return = 0, y_return = 0;
    unsigned int width_return = 0, height_return = 0, border_width_return = 0, depth_return = 0;
                
    if (!XGetGeometry (display, window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return))
      throw xtl::format_operation_exception (METHOD_NAME, "XGetGeometry failed");    
      
    return x_return == 0 && y_return == 0 && width_return == mode_desc.width && height_return == mode_desc.height;
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
