#include "shared.h"

using namespace common;
using namespace syslib;
using namespace render::low_level;
using namespace render::mid_level;
using namespace render::mid_level::window_driver;

/*
    Константы
*/

namespace
{

const char* DRIVER_NAME = "WindowDriver"; //имя драйвера

void test_registry_variable (VarRegistry& var_registry, const char* variable_name)
{
  if (!var_registry.HasVariable (variable_name))
    throw xtl::format_operation_exception ("", "There is no '%s' variable in the configuration registry branch '%s'",
      variable_name, var_registry.BranchName ());
}

void test_registry_variables (VarRegistry& var_registry)
{
  test_registry_variable (var_registry, "DriverMask");
  test_registry_variable (var_registry, "AdapterMask");
  test_registry_variable (var_registry, "ColorBits");
  test_registry_variable (var_registry, "AlphaBits");
  test_registry_variable (var_registry, "DepthBits");
  test_registry_variable (var_registry, "StencilBits");
  test_registry_variable (var_registry, "SamplesCount");
  test_registry_variable (var_registry, "BuffersCount");
  test_registry_variable (var_registry, "SwapMethod");
  test_registry_variable (var_registry, "VSync");
  test_registry_variable (var_registry, "FullScreen");
}

stl::string get_string (VarRegistry& registry, const char* name)
{
  return registry.GetValue (name).cast<stl::string> ();
}

SwapMethod get_swap_method (VarRegistry& registry, const char* name)
{
  stl::string swap_method (get_string (registry, name));

  if (!xtl::xstricmp ("Discard", swap_method.c_str ()))
    return SwapMethod_Discard;
  if (!xtl::xstricmp ("Flip", swap_method.c_str ()))
    return SwapMethod_Flip;
  if (!xtl::xstricmp ("Copy", swap_method.c_str ()))
    return SwapMethod_Copy;

  throw xtl::format_operation_exception ("", "Unknown '%s' value", name);
}

size_t get_size_t (VarRegistry& registry, const char* name)
{
  return atoi (get_string (registry, name).c_str ());
}

stl::string get (VarRegistry& registry, const char* name)
{
  return get_string (registry, name);
}

stl::string get (VarRegistry& registry, const char* name, const char* default_value)
{
  if (registry.HasVariable (name))
    return get_string (registry, name);

  return stl::string ();
}

void fill_swap_chain_desc (SwapChainDesc& swap_chain_desc, const void* window_handle, VarRegistry& var_registry)
{
  test_registry_variables (var_registry);

  memset (&swap_chain_desc, 0, sizeof (swap_chain_desc));

  swap_chain_desc.frame_buffer.width        = 0;
  swap_chain_desc.frame_buffer.height       = 0;
  swap_chain_desc.frame_buffer.color_bits   = get_size_t (var_registry, "ColorBits");
  swap_chain_desc.frame_buffer.alpha_bits   = get_size_t (var_registry, "AlphaBits");
  swap_chain_desc.frame_buffer.depth_bits   = get_size_t (var_registry, "DepthBits");
  swap_chain_desc.frame_buffer.stencil_bits = get_size_t (var_registry, "StencilBits");
  swap_chain_desc.samples_count             = get_size_t (var_registry, "SamplesCount");
  swap_chain_desc.buffers_count             = get_size_t (var_registry, "BuffersCount");
  swap_chain_desc.swap_method               = get_swap_method (var_registry, "SwapMethod");
  swap_chain_desc.vsync                     = get_size_t (var_registry, "VSync") ? 1 : 0;
  swap_chain_desc.fullscreen                = get_size_t (var_registry, "FullScreen") ? 1 : 0;
  swap_chain_desc.window_handle             = window_handle;
}

/*
    Вхождение системы рендеринга в список драйвера
*/

typedef xtl::com_ptr<FrameBuffer> FrameBufferPtr;

struct WindowFrameBuffer : public xtl::trackable
{
  syslib::Window*           window;
  FrameBufferPtr            frame_buffer;
  Driver*                   driver;
  Driver::RendererEntry*    renderer_entry;
  stl::string               configuration_branch;

  WindowFrameBuffer (syslib::Window* in_window, FrameBuffer* in_frame_buffer, Driver* in_driver, 
                     Driver::RendererEntry* in_renderer_entry, const char* in_configuration_branch)
    : window (in_window), frame_buffer (in_frame_buffer, false), driver (in_driver), 
      renderer_entry (in_renderer_entry), configuration_branch (in_configuration_branch)
  {
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnDestroy, xtl::bind (&WindowFrameBuffer::DestroyWindowHandler, this)));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnChangeHandle, xtl::bind (&WindowFrameBuffer::ChangeWindowHandleHandler, this)));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnSize, xtl::bind (&WindowFrameBuffer::ResizeWindowHandler, this)));
    connect_tracker (window->RegisterEventHandler (WindowEvent_OnPaint, xtl::bind (&WindowFrameBuffer::UpdateWindowHandler, this)));

    syslib::Rect client_rect = window->ClientRect ();

    size_t width = client_rect.right - client_rect.left;
    size_t height = client_rect.bottom - client_rect.top;

    frame_buffer->SetSize (width, height);
  }

  void DestroyWindowHandler ();
  void ChangeWindowHandleHandler ();
  void ResizeWindowHandler ();
  void UpdateWindowHandler ();
};

}

struct Driver::RendererEntry
{
  typedef xtl::com_ptr<RendererDispatch> RendererDispatchPtr;

  stl::string         renderer_name;
  RendererDispatchPtr renderer;

  RendererEntry (const char* in_renderer_name, syslib::Window& window, const char* configuration_branch, Driver* in_driver)
    : renderer_name (in_renderer_name),
      driver        (in_driver)
  {
    frame_buffers.push_back (WindowFrameBufferPtr (new WindowFrameBuffer (&window, CreateFrameBuffer (window.Handle (), configuration_branch), driver, this, configuration_branch)));
  
    renderer = RendererDispatchPtr (new RendererDispatch (*device), false);

    renderer->AddFrameBuffer (frame_buffers.back ()->frame_buffer.get ());
  }

  void RegisterWindow (syslib::Window& window, const char* configuration_branch)
  {
    for (WindowFrameBufferArray::iterator iter = frame_buffers.begin (), end = frame_buffers.end (); iter != end; ++iter)
      if ((*iter)->window == &window)
        throw xtl::format_operation_exception ("render::mid_level::window_driver::Driver::RendererEntry::RegisterWindow", "Window already registered");

    frame_buffers.push_back (WindowFrameBufferPtr (new WindowFrameBuffer (&window, CreateFrameBuffer (window.Handle (), configuration_branch), driver, this, configuration_branch)));

    renderer->AddFrameBuffer (frame_buffers.back ()->frame_buffer.get ());
  }

  void UnregisterWindow (syslib::Window& window)
  {
    for (WindowFrameBufferArray::iterator iter = frame_buffers.begin (), end = frame_buffers.end (); iter != end; ++iter)
      if ((*iter)->window == &window)
      {
        frame_buffers.erase (iter);
        
        break;
      }
  }

  FrameBuffer* CreateFrameBuffer (const void* window_handle, const char* configuration_branch)
  {
    try
    {
      if (!window_handle)
       throw xtl::make_null_argument_exception ("", "window_handle");

      if (!configuration_branch)
        throw xtl::make_null_argument_exception ("", "configuration_branch");

      SwapChainDesc swap_chain_desc;

      VarRegistry var_registry (configuration_branch);

      fill_swap_chain_desc (swap_chain_desc, window_handle, var_registry);

      SwapChainPtr swap_chain;

      if (device)
        swap_chain = SwapChainPtr (render::low_level::DriverManager::CreateSwapChain (get (var_registry, "DriverMask").c_str (), 
                                                                                      get (var_registry, "AdapterMask").c_str (), swap_chain_desc), false);
      else
        render::low_level::DriverManager::CreateSwapChainAndDevice (get (var_registry, "DriverMask").c_str (), get (var_registry, "AdapterMask").c_str (), 
                                                                    swap_chain_desc, get (var_registry, "InitString", "").c_str (), swap_chain, device);

      return new FrameBuffer (*device, *swap_chain);
    }
    catch (xtl::exception& exception)
    {
      exception.touch ("render::mid_level::window_driver::Driver::RendererEntry::CreateFrameBuffer");
      throw;
    }
  }

  private:
    typedef xtl::com_ptr<render::low_level::IDevice> DevicePtr;  
    typedef xtl::com_ptr<ISwapChain>                 SwapChainPtr;
    typedef xtl::shared_ptr<WindowFrameBuffer>       WindowFrameBufferPtr;
    typedef stl::vector<WindowFrameBufferPtr>        WindowFrameBufferArray;

  private:
    Driver*                driver;
    DevicePtr              device;
    WindowFrameBufferArray frame_buffers;
};

namespace
{

void WindowFrameBuffer::DestroyWindowHandler ()
{
  driver->UnregisterWindow (renderer_entry->renderer_name.c_str (), *window);
}

void WindowFrameBuffer::ChangeWindowHandleHandler ()
{
  if (!window->Handle ())
  {
    renderer_entry->renderer->RemoveFrameBuffer (frame_buffer.get ());
    frame_buffer = 0;
    return;
  }

  frame_buffer = FrameBufferPtr (renderer_entry->CreateFrameBuffer (window->Handle (), configuration_branch.c_str ()), false);

  renderer_entry->renderer->AddFrameBuffer (frame_buffer.get ());
}

void WindowFrameBuffer::ResizeWindowHandler ()
{
  syslib::Rect client_rect = window->ClientRect ();

  size_t width = client_rect.right - client_rect.left;
  size_t height = client_rect.bottom - client_rect.top;

  frame_buffer->SetSize (width, height);

  renderer_entry->renderer->FrameBufferResizeNotify (frame_buffer.get (), width, height);
}

void WindowFrameBuffer::UpdateWindowHandler ()
{
  renderer_entry->renderer->FrameBufferUpdateNotify (frame_buffer.get ());
}

}

/*
    Конструктор / деструктор
*/

Driver::Driver ()
{
}

Driver::~Driver ()
{
  UnregisterDriver ();
}

/*
    Описание драйвера
*/

const char* Driver::GetDescription ()
{
  return "render::mid_level::window_driver::Driver";
}

/*
    Перечисление доступных систем визуализации
*/

size_t Driver::GetRenderersCount ()
{
  return renderer_entries.size ();
}

const char* Driver::GetRendererName (size_t index)
{
  if (index >= GetRenderersCount ())
    throw xtl::make_range_exception ("render::mid_level::window_driver::Driver::GetRendererName", "index", index, 0u, GetRenderersCount ());

  return renderer_entries[index]->renderer_name.c_str ();
}

/*
    Создание системы визуализации
*/

IRenderer* Driver::CreateRenderer (const char* name)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::Driver::CreateRenderer";

  if (!name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "name");
    
  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), name))
    {
      (*iter)->renderer->AddRef ();

      return (*iter)->renderer.get ();
    }

  throw xtl::make_argument_exception (METHOD_NAME, "name", name);
}

/*
   Регистрация систем рендернинга
*/

void Driver::RegisterWindow (const char* renderer_name, syslib::Window& window, const char* configuration_branch)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::Driver::RegisterRenderer";

  if (!renderer_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "renderer_name");

  if (!configuration_branch)
    throw xtl::make_null_argument_exception (METHOD_NAME, "configuration_branch");

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), renderer_name))
    {
      (*iter)->RegisterWindow (window, configuration_branch);
      return;
    }

  renderer_entries.push_back (RendererEntryPtr (new RendererEntry (renderer_name, window, configuration_branch, this)));

  if (renderer_entries.size () == 1)
    DriverManager::RegisterDriver (DRIVER_NAME, this);
}

void Driver::UnregisterWindow (const char* renderer_name, syslib::Window& window)
{
  if (!renderer_name)
    return;

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), renderer_name))
    {
      (*iter)->UnregisterWindow (window);

      if (!(*iter)->renderer->GetFrameBuffersCount ())
      {
        renderer_entries.erase (iter);

        if (renderer_entries.empty ())
          UnregisterDriver ();
      }

      return;
    }
}

void Driver::UnregisterAllWindows (const char* renderer_name)
{
  if (!renderer_name)
    return;

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->renderer_name.c_str (), renderer_name))
    {
      renderer_entries.erase (iter);

      if (renderer_entries.empty ())
        UnregisterDriver ();

      return;
    }
}

void Driver::UnregisterAllWindows ()
{
  renderer_entries.clear ();
  UnregisterDriver ();
}

void Driver::UnregisterDriver ()
{
  DriverManager::UnregisterDriver (DRIVER_NAME);
}

namespace
{

/*
   Синглтон драйвера низкоуровневой системы рендернинга
*/

typedef common::Singleton<Driver> WindowDriverSingleton;

}

/*
   Драйвер оконной системы рендеринга
*/

/*
   Имя драйвера
*/

const char* WindowDriver::Name ()
{
  return DRIVER_NAME;
}
    
/*
   Получение драйвера
*/

render::mid_level::IDriver* WindowDriver::Driver ()
{
  return WindowDriverSingleton::InstancePtr ();
}

/*
   Регистрация окон
*/

void WindowDriver::RegisterWindow (const char* renderer_name, syslib::Window& window, const char* configuration_branch)
{
  WindowDriverSingleton::Instance ().RegisterWindow (renderer_name, window, configuration_branch);
}

void WindowDriver::UnregisterWindow (const char* renderer_name, syslib::Window& window)
{
  WindowDriverSingleton::Instance ().UnregisterWindow (renderer_name, window);
}

void WindowDriver::UnregisterAllWindows (const char* renderer_name)
{
  WindowDriverSingleton::Instance ().UnregisterAllWindows (renderer_name);
}

void WindowDriver::UnregisterAllWindows ()
{
  WindowDriverSingleton::Instance ().UnregisterAllWindows ();
}
