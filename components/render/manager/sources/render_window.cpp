#include "shared.h"

using namespace render;
using namespace render::low_level;

/*
    Описание реализации окна рендеринга
*/

typedef xtl::signal<void (Window&)> WindowSignal;

struct WindowImpl::Impl: public xtl::trackable
{
  WindowImpl*              owner;                     //окно-владелец
  stl::string              name;                      //имя окна
  DeviceManagerPtr         device_manager;            //менеджер устройства отрисовки
  low_level::SwapChainDesc swap_chain_desc;           //параметры цепочки обмена
  LowLevelSwapChainPtr     swap_chain;                //цепочка обмена
  RenderTargetPtr          color_buffer;              //буфер цвета окна
  RenderTargetPtr          depth_stencil_buffer;      //буфер глубины окна
  size_t                   width;                     //ширина окна
  size_t                   height;                    //высота окна
  WindowSignal             signals [WindowEvent_Num]; //сигналы окна
  Log                      log;                       //протокол
  
///Конструктор
  Impl (WindowImpl* in_owner)
    : owner (in_owner)
    , width (0)
    , height (0)
  {
    memset (&swap_chain_desc, 0, sizeof swap_chain_desc);
  }
  
///Создание цепочки обмена
  void CreateSwapChain (syslib::Window& window)
  {
    try
    {
      swap_chain = 0;
      
      if (!device_manager)
        throw xtl::format_operation_exception ("", "Null device manager");
      
      low_level::IDriver* driver = low_level::DriverManager::FindDriver (device_manager->DriverName ());
      
      if (!driver)
        throw xtl::format_operation_exception ("", "Rendering driver '%s' not found", device_manager->DriverName ());
        
      const char* adapter_name = device_manager->AdapterName ();
        
      for (size_t i=0, count=driver->GetAdaptersCount (); i<count; i++)
      {
        low_level::IAdapter* adapter = driver->GetAdapter (i);
        
        if (strcmp (adapter->GetName (), adapter_name))
          continue;
          
        swap_chain_desc.window_handle = window.Handle ();
          
        swap_chain = driver->CreateSwapChain (1, &adapter, swap_chain_desc);
        
        break;
      }
      
      if (!swap_chain)
        throw xtl::format_operation_exception ("", "Rendering adapter '%s' not found in driver '%s'", adapter_name, device_manager->DriverName ());
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::WindowImpl::Impl::CreateSwapChain");
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
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::WindowImpl::Impl::CreateRenderTargets");
      throw;
    }
  }
  
///Обновление размеров окна
  void UpdateSizes (syslib::Window& window)
  {
    try
    {
      width  = window.ClientWidth ();
      height = window.ClientHeight ();
    }
    catch (xtl::exception& e)
    {
      e.touch ("render::WindowImpl::Impl::UpdateSizes");
      throw;
    }
  }

///Обработка события перерисовки окна
  void OnUpdate ()
  {
    try
    {
      if (signals [WindowEvent_OnUpdate].empty () || !swap_chain)
        return;
        
      signals [WindowEvent_OnUpdate] (Wrappers::Wrap<Window> (owner));
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::WindowImpl::Impl::OnUpdate", e.what ());      
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::WindowImpl::Impl::OnUpdate");
    }
  }

///Обработка события изменения размеров окна
  void OnResize (syslib::Window& window)
  {
    try
    {
      UpdateSizes (window);    
      
      if (signals [WindowEvent_OnResize].empty ())
        return;
        
      width  = window.ClientWidth ();
      height = window.ClientHeight ();
      
      if (color_buffer)
      {
        color_buffer->Resize (width, height);
        depth_stencil_buffer->Resize (width, height);
      }
        
      signals [WindowEvent_OnResize] (Wrappers::Wrap<Window> (owner));
    }
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at render::WindowImpl::Impl::OnResize", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::WindowImpl::Impl::OnResize");
    }
  }
  
///Обработка события смены оконного дескриптора
  void OnChangeHandle (syslib::Window& window)
  {
    try
    {
      swap_chain = 0;
      
      if (!window.Handle ())
        return;

      CreateSwapChain (window);
      
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
      log.Printf ("%s\n    at render::WindowImpl::Impl::OnChangeHandle", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at render::WindowImpl::Impl::OnChangeHandle");
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

WindowImpl::WindowImpl (const DeviceManagerPtr& device_manager, syslib::Window& window, const common::PropertyMap& properties)
{
  try
  {
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
    
    impl = new Impl (this);
    
    impl->swap_chain_desc = swap_chain_desc;
    
    if (!device_manager)
    {
      const char *driver_mask  = get_string_property (properties, "DriverMask", "*"),
                 *adapter_mask = get_string_property (properties, "AdapterMask", "*"),
                 *init_string  = get_string_property (properties, "InitString", "");
                 
      LowLevelDevicePtr device;
      
      swap_chain_desc.window_handle = window.Handle ();

      low_level::DriverManager::CreateSwapChainAndDevice (driver_mask, adapter_mask, swap_chain_desc, init_string, impl->swap_chain, device);
      
      impl->device_manager = DeviceManagerPtr (new render::DeviceManager (device, driver_mask, adapter_mask), false);
    }
    else
    {
      impl->device_manager = device_manager;
      
      impl->CreateSwapChain (window);
    }
    
      //обновление размеров
      
    impl->UpdateSizes (window);
    
      //подписка на события окна
      
    impl->connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Impl::OnResize, &*impl, _1)));
    impl->connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnChangeHandle, xtl::bind (&Impl::OnChangeHandle, &*impl, _1)));
    impl->connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Impl::OnUpdate, &*impl)));
    
      //создание целей рендеринга
      
    impl->CreateRenderTargets ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::WindowImpl::WindowImpl");
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
    throw xtl::make_null_argument_exception ("render::WindowImpl::SetName", "name");
    
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
    throw xtl::format_operation_exception ("render::WindowImpl::ColorBuffer", "No color buffer binded");
    
  return impl->color_buffer;
}

const RenderTargetPtr& WindowImpl::DepthStencilBuffer ()
{
  if (!impl->depth_stencil_buffer)
    throw xtl::format_operation_exception ("render::WindowImpl::DepthStencilBuffer", "No depth-stencil buffer binded");
    
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
      throw xtl::make_argument_exception ("render::WindowImpl::RegisterEventHandler", "event", event);
  }
}
