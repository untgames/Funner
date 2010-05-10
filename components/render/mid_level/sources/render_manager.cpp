#include "shared.h"

using namespace render::mid_level;
using namespace render::low_level;

/*
    Константы
*/

const size_t WINDOW_ARRAY_RESERVE_SIZE = 8; //резервируемое число окон

/*
    Описание реализации менеджера рендеринга
*/

typedef xtl::signal<void (RenderManager&, Window&)> WindowSignal;
typedef stl::vector<WindowImpl*>                    WindowArray;

struct RenderManagerImpl::Impl: public xtl::trackable
{
  RenderManagerImpl* owner;                                         //владелец
  DeviceManagerPtr   device_manager;                                //менеджер устройства отрисовки
  WindowSignal       window_signals [RenderManagerWindowEvent_Num]; //оконные сигналы
  WindowArray        windows;                                       //окна

///Конструктор
  Impl (RenderManagerImpl* in_owner)
    : owner (in_owner)
  {
    windows.reserve (WINDOW_ARRAY_RESERVE_SIZE);
  }
  
///Получение менеджера устройства
  mid_level::DeviceManager& DeviceManager ()
  {
    if (!device_manager)
      throw xtl::format_operation_exception ("render::mid_level::RenderManagerImpl::Impl::DeviceManager", "Device not initialized. Create rendering window first");
      
    return *device_manager;
  }
  
///Оповещение об удалении окна
  void OnWindowDestroy (WindowImpl* window)
  {
    for (WindowArray::iterator iter=windows.begin (), end=windows.end (); iter!=end; ++iter)
      if (*iter == window)
      {
        windows.erase (iter);
        
        WindowEventNotify (RenderManagerWindowEvent_OnRemoved, Wrappers::Wrap<mid_level::Window> (window));
        
        return;
      }
  }
  
///Оконные оповещения
  void WindowEventNotify (RenderManagerWindowEvent event, mid_level::Window& window)
  {
    if (window_signals [event].empty ())
      return;
      
    try
    {
      window_signals [event](Wrappers::Wrap<RenderManager> (owner), window);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
    Конструктор / деструктор
*/

RenderManagerImpl::RenderManagerImpl ()
{
  try
  {
    impl = new Impl (this);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::RenderManagerImpl::RenderManagerImpl");
    throw;
  }
}

RenderManagerImpl::~RenderManagerImpl ()
{
}

/*
    Строка описания устройства рендеринга
*/

const char* RenderManagerImpl::Description ()
{
  return impl->DeviceManager ().Device ().GetName ();
}

/*
    Создание окна рендеринга
*/

WindowPtr RenderManagerImpl::CreateWindow (syslib::Window& sys_window, common::PropertyMap& properties)
{
  try
  {
    WindowPtr window (new WindowImpl (impl->device_manager, sys_window, properties), false);
    
    window->connect_tracker (xtl::bind (&Impl::OnWindowDestroy, &*impl, &*window), *impl);

    impl->windows.push_back (&*window);
    
    if (!impl->device_manager)
      impl->device_manager = window->DeviceManager ();
    
    impl->WindowEventNotify (RenderManagerWindowEvent_OnAdded, Wrappers::Wrap<mid_level::Window> (window));
    
    return window;
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::RenderManagerImpl::CreateWindow");
    throw;
  }
}

/*
    Перебор окон рендеринга
*/

size_t RenderManagerImpl::WindowsCount ()
{
  return impl->windows.size ();
}

WindowPtr RenderManagerImpl::Window (size_t index)
{
  if (index >= impl->windows.size ())    
    throw xtl::make_range_exception ("render::mid_level::RenderManagerImpl::Window", "index", index, impl->windows.size ());
    
  return impl->windows [index];
}

/*
    Создание целей рендеринга
*/

RenderTargetPtr RenderManagerImpl::CreateRenderBuffer (size_t width, size_t height, render::mid_level::PixelFormat format)
{
  try
  {
    low_level::TextureDesc texture_desc;
    
    memset (&texture_desc, 0, sizeof texture_desc);
    
    texture_desc.dimension            = low_level::TextureDimension_2D;
    texture_desc.width                = width;
    texture_desc.height               = height;
    texture_desc.layers               = 1;
    texture_desc.generate_mips_enable = false;
    texture_desc.access_flags         = low_level::AccessFlag_ReadWrite;
    texture_desc.bind_flags           = low_level::BindFlag_RenderTarget;
    
    switch (format)
    {
      case PixelFormat_RGB8:
        texture_desc.format = low_level::PixelFormat_RGB8;
        break;
      case PixelFormat_RGBA8:
        texture_desc.format = low_level::PixelFormat_RGBA8;
        break;
      case PixelFormat_L8:
        texture_desc.format = low_level::PixelFormat_L8;
        break;
      case PixelFormat_A8:
        texture_desc.format = low_level::PixelFormat_A8;
        break;
      case PixelFormat_LA8:
        texture_desc.format = low_level::PixelFormat_LA8;
        break;
      default:
        throw xtl::make_argument_exception ("", "format", format);
    }
    
    LowLevelTexturePtr texture (impl->DeviceManager ().Device ().CreateTexture (texture_desc), false);
    
    return RenderTargetPtr (new RenderTargetImpl (&impl->DeviceManager (), &*texture), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::RenderManagerImpl::CreateRenderBuffer");
    throw;
  }
}

RenderTargetPtr RenderManagerImpl::CreateDepthStencilBuffer (size_t width, size_t height)
{
  try
  {
    low_level::TextureDesc texture_desc;
    
    memset (&texture_desc, 0, sizeof texture_desc);
    
    texture_desc.dimension            = low_level::TextureDimension_2D;
    texture_desc.width                = width;
    texture_desc.height               = height;
    texture_desc.layers               = 1;
    texture_desc.format               = low_level::PixelFormat_D24S8;
    texture_desc.generate_mips_enable = false;
    texture_desc.access_flags         = low_level::AccessFlag_ReadWrite;
    texture_desc.bind_flags           = low_level::BindFlag_DepthStencil;
    
    LowLevelTexturePtr texture (impl->DeviceManager ().Device ().CreateTexture (texture_desc), false);
 
    return RenderTargetPtr (new RenderTargetImpl (&impl->DeviceManager (), &*texture), false);
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::mid_level::RenderManagerImpl::CreateDepthStencilBuffer");
    throw;
  }
}

/*
    Создание примитивов
*/

PrimitivePtr RenderManagerImpl::CreatePrimitive ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreatePrimitive()");
}

PrimitivePtr RenderManagerImpl::CreatePrimitive (const char* name, ResourceInstanceMode mode)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreatePrimitive(const char*,ResourceInstanceMode)");
}

FramePtr RenderManagerImpl::CreateFrame ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateFrame");
}

TexturePtr RenderManagerImpl::CreateTexture (const media::Image& image)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateTexture(const media::Image&)");
}

TexturePtr RenderManagerImpl::CreateTexture (const media::Image& image, render::mid_level::TextureDimension dimension)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateTexture(const media::Image&,TextureDimension)");
}

TexturePtr RenderManagerImpl::CreateTexture (render::mid_level::TextureDimension, size_t width, size_t height, size_t depth, render::mid_level::PixelFormat format)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateTexture(TextureDimension,size_t,size_t,size_t,PixelFormat)");
}

MaterialPtr RenderManagerImpl::CreateMaterial ()
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::CreateMaterial");
}

/*
    Загрузка ресурсов
*/

ResourceLibraryPtr RenderManagerImpl::Load (const char* resource_name)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const char*)");
}

ResourceLibraryPtr RenderManagerImpl::Load (const media::rfx::MaterialLibrary& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const media::rfx::MaterialLibrary&)");
}

ResourceLibraryPtr RenderManagerImpl::Load (const media::rfx::EffectLibrary& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const media::rfx::EffectLibrary&)");
}

ResourceLibraryPtr RenderManagerImpl::Load (const media::geometry::MeshLibrary& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const media::geometry::MeshLibrary&)");
}

ResourceLibraryPtr RenderManagerImpl::Load (const media::rfx::ShaderLibrary& library)
{
  throw xtl::make_not_implemented_exception ("render::mid_level::RenderManagerImpl::Load(const media::rfx::ShaderLibrary&)");
}

/*
    Регистрация на события
*/

xtl::connection RenderManagerImpl::RegisterWindowEventHandler (RenderManagerWindowEvent event, const WindowEventHandler& handler) const
{
  switch (event)
  {
    case RenderManagerWindowEvent_OnAdded:
    case RenderManagerWindowEvent_OnRemoved:
      return impl->window_signals [event].connect (handler);
    default:
        throw xtl::make_argument_exception ("render::mid_level::RenderManagerImpl::RegisterWindowEventHandler", "event", event);
  }
}
