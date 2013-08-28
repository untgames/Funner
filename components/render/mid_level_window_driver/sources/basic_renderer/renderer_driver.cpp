#include "shared.h"

using namespace common;
using namespace syslib;
using namespace render::low_level;
using namespace render::mid_level;
using namespace render::mid_level::window_driver;

namespace
{

/*
    Константы
*/

const char* DRIVER_NAME = "WindowDriver"; //имя драйвера

/*
    Вхождение окна в систему рендеринга
*/

typedef xtl::com_ptr<FrameBuffer> FrameBufferPtr;

struct WindowEntry: public xtl::trackable, public xtl::reference_counter
{
  Window&        window;              //окно
  const void*    saved_window_handle; //сохраненный дескриптор окна
  FrameBufferPtr frame_buffer;        //буфера кадров
  SwapChainDesc  swap_chain_desc;     //дескриптор цепочки обмена

///Конструктор
    WindowEntry (syslib::Window& in_window, const ParseNode& cfg_node)
      : window (in_window)
      , saved_window_handle (window.Handle ())
    {
        //чтение параметров инициализации устройства отрисовки

      memset (&swap_chain_desc, 0, sizeof (swap_chain_desc));

      swap_chain_desc.frame_buffer.color_bits   = get<size_t> (cfg_node, "ColorBits", 0);
      swap_chain_desc.frame_buffer.alpha_bits   = get<size_t> (cfg_node, "AlphaBits", 0);
      swap_chain_desc.frame_buffer.depth_bits   = get<size_t> (cfg_node, "DepthBits", 0);
      swap_chain_desc.frame_buffer.stencil_bits = get<size_t> (cfg_node, "StencilBits", 0);
      swap_chain_desc.samples_count             = get<size_t> (cfg_node, "SamplesCount", 0);
      swap_chain_desc.buffers_count             = get<size_t> (cfg_node, "BuffersCount", 1);
      swap_chain_desc.swap_method               = SwapMethod_Discard;
      swap_chain_desc.vsync                     = get<bool> (cfg_node, "VSync", false);
      swap_chain_desc.fullscreen                = get<bool> (cfg_node, "FullScreen", false);

      const char* swap_method_name = get<const char*> (cfg_node, "SwapMethod", (const char*)0);

      if (swap_method_name)
      {
        if      (!xtl::xstricmp ("Flip",    swap_method_name)) swap_chain_desc.swap_method = SwapMethod_Flip;
        else if (!xtl::xstricmp ("Copy",    swap_method_name)) swap_chain_desc.swap_method = SwapMethod_Copy;
        else if (!xtl::xstricmp ("Discard", swap_method_name)) swap_chain_desc.swap_method = SwapMethod_Discard;
        else
        {
          cfg_node.Log ().Error (cfg_node, "Attribute 'SwapMethod' has wrong value '%s'", swap_method_name);
        }
      }
    }
};

}

/*
    Вхождение системы рендеринга в драйвер
*/

class Driver::RendererEntry: public xtl::reference_counter
{
  public:
///Конструктор
    RendererEntry (const char* in_renderer_name, Driver& in_driver)
      : driver        (in_driver),
        renderer_name (in_renderer_name),
        low_level_driver_mask ("*"),
        low_level_adapter_mask ("*")
    {
    }

///Имя системы рендеринга
    const char* Name () { return renderer_name.c_str (); }

///Чтение настроек
    void ReadConfiguration (const ParseNode& cfg_node)
    {
      low_level_driver_mask        = get<const char*> (cfg_node, "DriverMask", "*");
      low_level_adapter_mask       = get<const char*> (cfg_node, "AdapterMask", "*");
      low_level_device_init_string = get<const char*> (cfg_node, "InitString", "");
    }

///Регистрация нового окна в системе рендеринга
    void RegisterWindow (syslib::Window& window, const ParseNode& cfg_node)
    {
      try
      {
          //проверка корректности аргументов

        for (WindowEntryArray::iterator iter = windows.begin (), end = windows.end (); iter != end; ++iter)
        {
          WindowEntry& window_entry = **iter;

          if (&window_entry.window == &window)
            throw xtl::format_operation_exception ("", "Window already registered");
        }
        
          //создание вхождения окна в систему рендеринга
          
        WindowEntryPtr window_entry (new WindowEntry (window, cfg_node), false);
        DevicePtr      window_device = device;
        
          //создание буфера кадра

        CreateFrameBuffer (*window_entry, window_device);
        
          //регистрация обработчиков событий окна

        window_entry->connect_tracker (window.RegisterEventHandler (WindowEvent_OnDestroy, xtl::bind (&RendererEntry::UnregisterWindow,
          this, xtl::ref (window))));

        window_entry->connect_tracker (window.RegisterEventHandler (WindowEvent_OnPaint, xtl::bind (&RendererEntry::OnUpdateWindow,
          this, xtl::ref (*window_entry))));

        window_entry->connect_tracker (window.RegisterEventHandler (WindowEvent_OnChangeViewport, xtl::bind (&RendererEntry::OnChangeViewport,
          this, xtl::ref (*window_entry))));
          
        window_entry->connect_tracker (window.RegisterEventHandler (WindowEvent_OnChangeHandle, xtl::bind (&RendererEntry::OnChangeWindowHandle,
          this, xtl::ref (*window_entry))));

          //регистрация буфера кадра

        windows.push_back (window_entry);
        
        try
        {
            //регистрация нового буфера кадров

          if (renderer)
            renderer->AddFrameBuffer (window_entry->frame_buffer.get (), reinterpret_cast<size_t> (&*window_entry));

            //сохранение параметров

          device = window_device;
        }
        catch (...)
        {
          windows.pop_back ();
          throw;
        }
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::mid_level::window_driver::Driver::RendererEntry::RegisterWindow");
        throw;
      }
    }

///Отмена регистрации окна в системе рендеринга
    void UnregisterWindow (syslib::Window& window)
    {
        //удаление буфера кадра, соответствующего окну
      
      for (WindowEntryArray::iterator iter = windows.begin (), end = windows.end (); iter != end; ++iter)
      {
        WindowEntry& window_entry = **iter;

        if (&window_entry.window == &window)
        {
          windows.erase (iter);
          break;
        }
      }

        //в случае отсутствия буферов кадра - удаление вхождения системы рендеринга в драйвер

      if (windows.empty ())
        driver.UnregisterAllWindows (renderer_name.c_str ());
    }

///Создание системы рендеринга
    IRenderer* CreateRenderer ()
    {
      try
      {
          //если система уже создана - возвращаем её
          
        if (renderer)
          return renderer.get ();

          //создание системы рендеринга

        if (!device)
          throw xtl::format_operation_exception ("", "No windows attached for renderer");            

        xtl::com_ptr<RendererDispatch> new_renderer (new RendererDispatch (*device), false);
        
          //добавление буферов кадра
          
        for (WindowEntryArray::iterator iter=windows.begin (), end=windows.end (); iter!=end; ++iter)
          new_renderer->AddFrameBuffer ((*iter)->frame_buffer.get (), reinterpret_cast<size_t> (&**iter));
          
          //возвращение результата

        renderer = new_renderer.get ();

        new_renderer->AddRef ();

        return renderer.get ();
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::mid_level::window_driver::Driver::RendererEntry::CreateRenderer");
        throw;
      }
    }

  private:
    typedef xtl::com_ptr<render::low_level::IDevice> DevicePtr;

  private:
///Создание буфера кадра
    void CreateFrameBuffer (WindowEntry& window_entry, DevicePtr& window_device)
    {
      try
      {
        if (!window_entry.window.Handle ())
          throw xtl::format_operation_exception ("", "Null window handle");

        window_entry.swap_chain_desc.window_handle = window_entry.window.Handle ();

          //создание цепочки обмена и буфера кадров
          
        typedef xtl::com_ptr<ISwapChain> SwapChainPtr;
          
        SwapChainPtr new_swap_chain;
        
        if (window_device)
        {
          new_swap_chain = SwapChainPtr (render::low_level::DriverManager::CreateSwapChain (low_level_driver_mask.c_str (),
            low_level_adapter_mask.c_str (), window_entry.swap_chain_desc), false);
        }
        else
        {
          render::low_level::DriverManager::CreateSwapChainAndDevice (low_level_driver_mask.c_str (), low_level_adapter_mask.c_str (),
            window_entry.swap_chain_desc, low_level_device_init_string.c_str (), new_swap_chain, window_device);
        }

        FrameBufferPtr new_frame_buffer (new FrameBuffer (*window_device, *new_swap_chain), false);

          //обновление размеров буфера кадра

        syslib::Rect viewport = window_entry.window.Viewport ();

        size_t width  = viewport.right - viewport.left,
               height = viewport.bottom - viewport.top;

        new_frame_buffer->SetSize (width, height);
        new_frame_buffer->SetViewportOffset (viewport.left, viewport.top);

          //сохранение состояния

        window_entry.frame_buffer = new_frame_buffer;
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("render::mid_level::window_driver::Driver::RendererEntry::CreateFrameBuffer");
        throw;
      }
    }

///Обработчик события смены оконного дескриптора
    void OnChangeWindowHandle (WindowEntry& window_entry)
    {
      const void* current_window_handle = window_entry.window.Handle ();

      if (window_entry.saved_window_handle == current_window_handle)
        return;

      if (!current_window_handle)
      {
        if (renderer)          
          renderer->RemoveFrameBuffer (window_entry.frame_buffer.get ());

        window_entry.frame_buffer = 0;

        return;
      }
      
        //создание буфера кадра

      CreateFrameBuffer (window_entry, device);

        //регистрация буфера кадра

      if (renderer)
        renderer->AddFrameBuffer (window_entry.frame_buffer.get (), reinterpret_cast<size_t> (&window_entry));

      window_entry.saved_window_handle = current_window_handle;
    }

///Обработчик события изменения области вывода
    void OnChangeViewport (WindowEntry& window_entry)
    {
      if (!renderer || !window_entry.frame_buffer)
        return;
      
      syslib::Rect viewport = window_entry.window.Viewport ();      

      size_t width  = viewport.right - viewport.left,
             height = viewport.bottom - viewport.top;

      window_entry.frame_buffer->SetSize (width, height);
      window_entry.frame_buffer->SetViewportOffset (viewport.left, viewport.top);

      renderer->FrameBufferResizeNotify (window_entry.frame_buffer.get (), width, height);
    }

///Обработчик события обновления окна
    void OnUpdateWindow (WindowEntry& window_entry)
    {
      if (!renderer || !window_entry.frame_buffer)
        return;

      renderer->FrameBufferUpdateNotify (window_entry.frame_buffer.get ());
    }

  private:
    typedef xtl::intrusive_ptr<WindowEntry>      WindowEntryPtr;
    typedef stl::vector<WindowEntryPtr>          WindowEntryArray;
    typedef xtl::trackable_ptr<RendererDispatch> RendererDispatchPtr;

  private:
    Driver&             driver;                       //ссылка на драйвер, хранящий систему рендеринга
    stl::string         renderer_name;                //имя системы рендеринга
    stl::string         low_level_driver_mask;        //маска имени драйвера
    stl::string         low_level_adapter_mask;       //маска имени драйвера
    stl::string         low_level_device_init_string; //строка инициализации устройства отрисовки
    WindowEntryArray    windows;                      //окна системы рендеринга
    DevicePtr           device;                       //устройство отрисовки
    RendererDispatchPtr renderer;                     //система рендеринга
};

/*
    Конструктор / деструктор
*/

Driver::Driver ()
{
  DriverManager::RegisterDriver (DRIVER_NAME, this);
}

Driver::~Driver ()
{
  DriverManager::UnregisterDriver (DRIVER_NAME);
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
  if (index >= renderer_entries.size ())
    throw xtl::make_range_exception ("render::mid_level::window_driver::Driver::GetRendererName", "index", index, renderer_entries.size ());

  return renderer_entries [index]->Name ();
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
  {
    RendererEntry& entry = **iter;

    if (!xtl::xstrcmp (entry.Name (), name))
      return entry.CreateRenderer ();
  }

  throw xtl::make_argument_exception (METHOD_NAME, "name", name, "Renderer not found");
}

/*
    Регистрация систем рендеринга
*/

void Driver::RegisterRenderer (const char* renderer_name, const common::ParseNode& configuration_node)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::Driver::RegisterRenderer";
  
    //проверка корректности аргументов

  if (!renderer_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "renderer_name");  
    
  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
  {
    RendererEntry& entry = **iter;

    if (!xtl::xstrcmp (entry.Name (), renderer_name))
      throw xtl::make_argument_exception (METHOD_NAME, "renderer_name", renderer_name, "Renderer has already registered");
  }
  
    //добавление системы рендеринга
  
  RendererEntryPtr renderer_entry (new RendererEntry (renderer_name, *this), false);

  renderer_entry->ReadConfiguration (configuration_node);

  renderer_entries.push_back (renderer_entry);
}

void Driver::RegisterWindow (const char* renderer_name, syslib::Window& window, const common::ParseNode& configuration_node)
{
  static const char* METHOD_NAME = "render::mid_level::window_driver::Driver::RegisterRenderer";
  
    //проверка корректности аргументов

  if (!renderer_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "renderer_name");
    
    //поиск системы рендеринга среди уже созданных
    
  RendererEntryPtr renderer_entry = 0;

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
  {
    RendererEntry& entry = **iter;

    if (!xtl::xstrcmp (entry.Name (), renderer_name))
    {
      renderer_entry = *iter;
      break;
    }
  }
  
    //если система рендеринга ещё не создана - создаём её
  
  if (!renderer_entry)
  {
    renderer_entry = RendererEntryPtr (new RendererEntry (renderer_name, *this), false);

    renderer_entries.push_back (renderer_entry);
  }

    //регистрация окна в системе рендеринга

  try
  {
    renderer_entry->RegisterWindow (window, configuration_node);
  }
  catch (...)
  {
    renderer_entries.pop_back ();
    throw;
  }
}

void Driver::UnregisterWindow (const char* renderer_name, syslib::Window& window)
{
  if (!renderer_name)
    return;

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->Name (), renderer_name))
    {
      (*iter)->UnregisterWindow (window);

      return;
    }
}

void Driver::UnregisterAllWindows (const char* renderer_name)
{
  if (!renderer_name)
    return;

  for (RendererEntries::iterator iter = renderer_entries.begin (), end = renderer_entries.end (); iter != end; ++iter)
    if (!xtl::xstrcmp ((*iter)->Name (), renderer_name))
    {
      renderer_entries.erase (iter);
      return;
    }
}

void Driver::UnregisterAllWindows ()
{
  renderer_entries.clear ();
}

/*
   Драйвер оконной системы рендеринга
*/

typedef common::Singleton<Driver> WindowDriverSingleton;

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
  return &*WindowDriverSingleton::Instance ();
}

/*
    Предварительная регистрация системы рендеринга (для конфигурирования)
*/

void WindowDriver::RegisterRenderer (const char* renderer_name, const common::ParseNode& configuration_node)
{
  WindowDriverSingleton::Instance ()->RegisterRenderer (renderer_name, configuration_node);
}

/*
   Регистрация окон
*/

void WindowDriver::RegisterWindow (const char* renderer_name, syslib::Window& window, const common::ParseNode& configuration_node)
{
  WindowDriverSingleton::Instance ()->RegisterWindow (renderer_name, window, configuration_node);
}

void WindowDriver::UnregisterWindow (const char* renderer_name, syslib::Window& window)
{
  WindowDriverSingleton::Instance ()->UnregisterWindow (renderer_name, window);
}

void WindowDriver::UnregisterAllWindows (const char* renderer_name)
{
  WindowDriverSingleton::Instance ()->UnregisterAllWindows (renderer_name);
}

void WindowDriver::UnregisterAllWindows ()
{
  WindowDriverSingleton::Instance ()->UnregisterAllWindows ();
}
