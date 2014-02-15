#include "shared.h"

using namespace render::manager;
using namespace render::low_level;

/*
    Описание реализации окна рендеринга
*/

typedef xtl::signal<void (Window&)> WindowSignal;
typedef xtl::com_ptr<INativeWindow> INativeWindowPtr;

struct WindowImpl::Impl: public xtl::trackable, public INativeWindowListener
{
  WindowImpl*              owner;                     //окно-владелец
  INativeWindowPtr         native_window;             //нативное окно
  stl::string              name;                      //имя окна
  DeviceManagerPtr         device_manager;            //менеджер устройства отрисовки  
  low_level::SwapChainDesc swap_chain_desc;           //параметры цепочки обмена
  LowLevelSwapChainPtr     swap_chain;                //цепочка обмена
  LowLevelAdapterPtr       adapter;                   //адаптер отрисовки
  RenderTargetPtr          color_buffer;              //буфер цвета окна
  RenderTargetPtr          depth_stencil_buffer;      //буфер глубины окна
  size_t                   width;                     //ширина окна
  size_t                   height;                    //высота окна
  WindowSignal             signals [WindowEvent_Num]; //сигналы окна
  Log                      log;                       //протокол
  SettingsPtr              settings;                  //настройки рендеринга
  
///Конструктор
  Impl (WindowImpl* in_owner, const SettingsPtr& in_settings)
    : owner (in_owner)
    , width (0)
    , height (0)
  {
    memset (&swap_chain_desc, 0, sizeof swap_chain_desc);    
  }

///Деструктор
  ~Impl ()
  {
    try
    {
      if (native_window)
        native_window->DetachListener (this);
    }
    catch (...)
    {
    }
  }
  
///Создание цепочки обмена
  void CreateSwapChain (void* handle, const char* adapter_mask)
  {
    try
    {
      swap_chain = 0;
      
      if (!device_manager)
        throw xtl::format_operation_exception ("", "Null device manager");

      swap_chain_desc.window_handle = handle;      
         
      swap_chain = low_level::DriverManager::CreateSwapChain (&device_manager->Driver (), adapter_mask, swap_chain_desc);

      if (!swap_chain)
        throw xtl::format_operation_exception ("", "Null swap chain after render::manager::low_level::IDriver::CreateSwapChain");
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::WindowImpl::Impl::CreateSwapChain(void*,const char*)");
      throw;
    }
  }

  void CreateSwapChain (void* handle)
  {
    try
    {
      swap_chain = 0;
      
      if (!device_manager)
        throw xtl::format_operation_exception ("", "Null device manager");

      swap_chain_desc.window_handle = handle;
      
      low_level::IAdapter* adapter_ptr = &*adapter;
          
      swap_chain = device_manager->Driver ().CreateSwapChain (1, &adapter_ptr, swap_chain_desc);

      if (!swap_chain)
        throw xtl::format_operation_exception ("", "Null swap chain after render::manager::low_level::IDriver::CreateSwapChain");
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::WindowImpl::Impl::CreateSwapChain(void*)");
      throw;
    }
  }
  
///Создание целей рендеринга
  void CreateRenderTargets ()
  {
    try
    {
      if (!swap_chain)
        throw xtl::format_operation_exception ("", "Null swap chain");
        
      log.Printf ("Initialize render targets for swap chain");
      
      LowLevelTexturePtr color_texture (device_manager->Device ().CreateRenderTargetTexture (&*swap_chain, swap_chain_desc.buffers_count - 1), false),
                         depth_stencil_texture (device_manager->Device ().CreateDepthStencilTexture (&*swap_chain), false);
 
      if (!color_buffer)
      {
        RenderTargetPtr new_color_buffer (new RenderTargetImpl (device_manager, &*color_texture), false),
                        new_depth_stencil_buffer (new RenderTargetImpl (device_manager, &*depth_stencil_texture), false);
        
        color_buffer         = new_color_buffer;
        depth_stencil_buffer = new_depth_stencil_buffer;
      }
      else
      {
        try
        {
          color_buffer->SetTarget (&*color_texture);
          depth_stencil_buffer->SetTarget (&*depth_stencil_texture);
        }
        catch (...)
        {
          color_buffer->SetTarget (0);
          depth_stencil_buffer->SetTarget (0);

          throw;
        }
      }
      
      color_buffer->Resize (width, height);
      depth_stencil_buffer->Resize (width, height);
      
      log.Printf ("...render targets for swap chain created");
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::manager::WindowImpl::Impl::CreateRenderTargets");
      throw;
    }
  }
  
///Обновление размеров окна
  void UpdateSizes (size_t in_width, size_t in_height)
  {
    width  = in_width;
    height = in_height;
  }

///Обработка события перерисовки окна
  void OnPaint ()
  {
    try
    {
      if (signals [WindowEvent_OnUpdate].empty () || !swap_chain)
        return;
        
      Window window = Wrappers::Wrap<Window> (owner);
        
      signals [WindowEvent_OnUpdate] (window);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::manager::WindowImpl::Impl::OnUpdate", e.what ());      
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::manager::WindowImpl::Impl::OnUpdate");
    }
  }

///Обработка события изменения размеров окна
  void OnSizeChanged (size_t width, size_t height)
  {
    try
    {
      UpdateSizes (width, height);  
      
      if (signals [WindowEvent_OnResize].empty ())
        return;
      
      if (color_buffer)
      {
        color_buffer->Resize (width, height);
        depth_stencil_buffer->Resize (width, height);
      }
      
      Window window = Wrappers::Wrap<Window> (owner);
        
      signals [WindowEvent_OnResize] (window);
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::manager::WindowImpl::Impl::OnResize", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::manager::WindowImpl::Impl::OnResize");
    }
  }
  
///Обработка события смены оконного дескриптора
  void OnHandleChanged (void* handle)
  {
    try
    {
      log.Printf ("Swap chain handle changed (handle=%p)", handle);
      
      swap_chain = 0;
      
      if (!handle)
        return;

      CreateSwapChain (handle);
      
      try
      {        
        CreateRenderTargets ();
      }
      catch (...)
      {
        swap_chain = 0;
        
        if (color_buffer)         color_buffer->SetTarget (0);
        if (depth_stencil_buffer) depth_stencil_buffer->SetTarget (0);
        
        throw;
      }
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::manager::WindowImpl::Impl::OnChangeHandle", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::manager::WindowImpl::Impl::OnChangeHandle");
    }
  }
  
///Обработчик события изменения области вывода
  void OnViewportChanged (const Rect& viewport)
  {
    try
    {
      log.Printf ("Window viewport changed: x=%d, y=%d, width=%u, height=%u", viewport.x, viewport.y, viewport.width, viewport.height);
      
      if (color_buffer)
      {
        color_buffer->Resize (viewport.width, viewport.height);
        color_buffer->SetViewportOffset (math::vec2ui (viewport.x, viewport.y));
      }
      
      if (depth_stencil_buffer)
      {
        depth_stencil_buffer->Resize (viewport.width, viewport.height);
        depth_stencil_buffer->SetViewportOffset (math::vec2ui (viewport.x, viewport.y));
      }
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::manager::WindowImpl::Impl::OnChangeViewport", e.what ());      
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::manager::WindowImpl::Impl::OnChangeViewport");
    }
  }  
};

/*
    Конструктор / деструктор
*/

namespace
{

size_t get_uint_property (const common::PropertyMap& properties, const char* name, size_t default_value)
{
  if (!properties.IsPresent (name))
    return default_value;
    
  return (size_t)properties.GetInteger (name);
}

const char* get_string_property (const common::PropertyMap& properties, const char* name, const char* default_value)
{
  if (!properties.IsPresent (name))
    return default_value;
    
  return properties.GetString (name);
}

}

WindowImpl::WindowImpl (const DeviceManagerPtr& device_manager, INativeWindow& window, const common::PropertyMap& properties, const SettingsPtr& settings, const CacheManagerPtr& cache_manager)
{
  try
  {
    Log log;
    
    log.Printf ("Creating render window");
    
      //заполнение параметров цепочки обмена
      
    low_level::SwapChainDesc swap_chain_desc;
    
    memset (&swap_chain_desc, 0, sizeof swap_chain_desc);
    
    swap_chain_desc.frame_buffer.color_bits   = get_uint_property (properties, "ColorBits", 0);
    swap_chain_desc.frame_buffer.alpha_bits   = get_uint_property (properties, "AlphaBits", 0);
    swap_chain_desc.frame_buffer.depth_bits   = get_uint_property (properties, "DepthBits", 0);
    swap_chain_desc.frame_buffer.stencil_bits = get_uint_property (properties, "StencilBits", 0);
    swap_chain_desc.samples_count             = get_uint_property (properties, "SamplesCount", 0);
    swap_chain_desc.buffers_count             = get_uint_property (properties, "BuffersCount", 2);
    swap_chain_desc.swap_method               = SwapMethod_Discard;
    swap_chain_desc.vsync                     = get_uint_property (properties, "VSync", 0) != 0;
    swap_chain_desc.fullscreen                = get_uint_property (properties, "FullScreen", 0) != 0;

    const char* swap_method_name = get_string_property (properties, "SwapMethod", (const char*)0);

    if (swap_method_name)
    {
      if      (!xtl::xstricmp ("Flip",    swap_method_name)) swap_chain_desc.swap_method = SwapMethod_Flip;
      else if (!xtl::xstricmp ("Copy",    swap_method_name)) swap_chain_desc.swap_method = SwapMethod_Copy;
      else if (!xtl::xstricmp ("Discard", swap_method_name)) swap_chain_desc.swap_method = SwapMethod_Discard;
      else
      {
        throw xtl::make_argument_exception ("", "properties ['SwapMethod']", swap_method_name);
      }
    }
    
      //создание цепочки обмена
    
    impl = new Impl (this, settings);
    
    impl->native_window   = &window;
    impl->swap_chain_desc = swap_chain_desc;
    
    if (!device_manager)
    {
      const char *driver_mask  = get_string_property (properties, "DriverMask", "*"),
                 *adapter_mask = get_string_property (properties, "AdapterMask", "*"),
                 *init_string  = get_string_property (properties, "InitString", "");
                 
      log.Printf ("Create device manager (driver='%s', adapter='%s', init_string='%s')", driver_mask, adapter_mask, init_string);                 
                 
      LowLevelDevicePtr device;
      LowLevelDriverPtr driver;
      
      swap_chain_desc.window_handle = window.GetHandle ();

      low_level::DriverManager::CreateSwapChainAndDevice (driver_mask, adapter_mask, swap_chain_desc, init_string, impl->swap_chain, device, driver);
      
      if (!device)
        throw xtl::format_operation_exception ("", "Null device after render::low_level::DriverManager::CreateSwapChainAndDevice");
        
      if (!driver)
        throw xtl::format_operation_exception ("", "Null driver after render::low_level::DriverManager::CreateSwapChainAndDevice");
        
      if (!impl->swap_chain)
        throw xtl::format_operation_exception ("", "Null swap chain after render::low_level::DriverManager::CreateSwapChainAndDevice");      
        
      LowLevelAdapterPtr adapter = impl->swap_chain->GetAdapter ();
        
      if (!adapter)
        throw xtl::format_operation_exception ("", "Null adapter after render::low_level::DriverManager::CreateSwapChainAndDevice");

      impl->device_manager = DeviceManagerPtr (new render::manager::DeviceManager (device, driver, settings, cache_manager), false);
      impl->adapter        = impl->swap_chain->GetAdapter ();

      log.Printf ("...device manager and swap chain have been successfully created");
    }
    else
    {
      const char* adapter_mask = get_string_property (properties, "AdapterMask", "*");
                                  
      impl->device_manager = device_manager;
      
      log.Printf ("Creating swap chain (adapter='%s')", adapter_mask);
      
      impl->CreateSwapChain (window.GetHandle (), adapter_mask);
    }
    
      //обновление размеров
      
    impl->UpdateSizes (window.GetWidth (), window.GetHeight ());
    
      //подписка на события окна

    window.AttachListener (&*impl);
    
      //создание целей рендеринга
      
    impl->CreateRenderTargets ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::WindowImpl::WindowImpl");
    throw;
  }
}

WindowImpl::~WindowImpl ()
{
}

/*
    Менеджер устройства
*/

DeviceManagerPtr& WindowImpl::DeviceManager ()
{
  return impl->device_manager;
}

/*
    Идентификатор окна
*/

void WindowImpl::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::manager::WindowImpl::SetName", "name");
    
  impl->name = name;
}

const char* WindowImpl::Name ()
{
  return impl->name.c_str ();
}

/*
    Цели рендеринга
*/

const RenderTargetPtr& WindowImpl::ColorBuffer ()
{
  if (!impl->color_buffer)
    throw xtl::format_operation_exception ("render::manager::WindowImpl::ColorBuffer", "No color buffer binded");
    
  return impl->color_buffer;
}

const RenderTargetPtr& WindowImpl::DepthStencilBuffer ()
{
  if (!impl->depth_stencil_buffer)
    throw xtl::format_operation_exception ("render::manager::WindowImpl::DepthStencilBuffer", "No depth-stencil buffer binded");
    
  return impl->depth_stencil_buffer;
}

/*
    Размеры целей рендеринга
*/

size_t WindowImpl::Width ()
{
  return impl->width;
}

size_t WindowImpl::Height ()
{
  return impl->height;
}

/*
    Принудительное обновление содержимого окна (обмен буферов)
*/

void WindowImpl::SwapBuffers ()
{
  try
  {
    impl->swap_chain->Present ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::manager::Window::SwapBuffers");
    throw;
  }
}

/*
    Регистрация обработчиков событий
*/

xtl::connection WindowImpl::RegisterEventHandler (WindowEvent event, const EventHandler& handler) const
{
  switch (event)
  {
    case WindowEvent_OnUpdate:
    case WindowEvent_OnResize:
      return impl->signals [event].connect (handler);
    default:
      throw xtl::make_argument_exception ("render::manager::WindowImpl::RegisterEventHandler", "event", event);
  }
}
