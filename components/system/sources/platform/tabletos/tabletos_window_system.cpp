#include "shared.h"

using namespace syslib;
using namespace syslib::tabletos;

/*
    Генерация исключения: работа с окнами невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No window support for this platform");
}

/*
    Window registry implmentation
*/

struct WindowRegistryImpl
{
  public:
    //Конструктор/деструктор
    WindowRegistryImpl ()
    {
    }

    ~WindowRegistryImpl ()
    {
    }
  
    //Регистрация окна
    void RegisterWindow (screen_window_t window, IWindowImpl* impl)
    {
      if (!window)
        throw xtl::make_null_argument_exception ("", "window");
      
      if (!impl)
        throw xtl::make_null_argument_exception ("", "impl");
      
      windows.insert_pair (window, impl);
    }
    
    //Отмена регистрации
    void UnregisterWindow (screen_window_t window)
    {
      if (!window)
        throw xtl::make_null_argument_exception ("", "window");

      windows.erase (window);
    }
    
    IWindowImpl* FindWindow (screen_window_t window)
    {
      try
      {
        WindowMap::iterator iter = windows.find (window);

        if (iter == windows.end ())
          return 0;

        return iter->second;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::tabletos::WindowRegistryImpl::FindWindow (window)");
        throw;
      }
    }
    
  private:
    WindowRegistryImpl (const WindowRegistryImpl&);             //no impl
    WindowRegistryImpl& operator = (const WindowRegistryImpl&); //no impl

  private:
    typedef stl::hash_map<screen_window_t, IWindowImpl*> WindowMap;

  private:
    WindowMap windows;
};

typedef common::Singleton<WindowRegistryImpl> WindowRegistrySingleton;

/*
    Window implementation
*/

struct WindowImpl: public IWindowImpl
{
  screen_context_t     screen_context;   // A context encapsulates the connection to the windowing system
  screen_window_t      screen_window;    // The window is the most basic drawing surface.
  screen_display_t     screen_display;   // A display represents the physical display hardware such as a touch screen display.
  WindowMessageHandler message_handler;                  //обработчик сообщений
  void*                user_data;                        //пользовательские данные окна

  bool                 background_state;
  bool                 is_multitouch_enabled;

  Rect                 window_rect;  
///Constructor
  WindowImpl ()
    : screen_window (0),
      message_handler (),
      user_data (), 
      background_state (false),
      is_multitouch_enabled (false),
      window_rect (0, 0, 0, 0)
  {
    try
    {
      platform_initialize ();
      
        //Create a screen context that will be used to create an EGL surface to to receive libscreen events.
        
      if (screen_create_context (&screen_context, SCREEN_APPLICATION_CONTEXT) != BPS_SUCCESS)
        raise_error ("::screen_create_context");
        
        //Signal BPS library that screen events will be requested

      if (screen_request_events (screen_context) != BPS_SUCCESS)
        raise_error ("::screen_request_events");

        //Creates a window that can be used to make graphical content visible on a display
        
      if (screen_create_window (&screen_window, screen_context) != BPS_SUCCESS)
        raise_error ("::screen_create_window");
        
        //Get display        

      if (screen_get_window_property_pv (screen_window, SCREEN_PROPERTY_DISPLAY, (void **)&screen_display))
        raise_error ("::screen_get_window_property_pv SCREEN_PROPERTY_DISPLAY");
        
      screen_display_mode_t screen_mode;

      if (screen_get_display_property_pv (screen_display, SCREEN_PROPERTY_MODE, (void**)&screen_mode))
        raise_error ("::screen_get_display_property_pv SCREEN_PROPERTY_MODE");

      int max_size = screen_mode.width > screen_mode.height ? screen_mode.width : screen_mode.height, buffer_size [2] = {max_size, max_size};

      if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size))
        raise_error ("::screen_set_window_property_iv SCREEN_PROPERTY_BUFFER_SIZE");

      if (screen_create_window_buffers (screen_window, 2)) //buffers count configuration???
        raise_error ("::screen_create_window_buffers"); 

      WindowRegistry::RegisterWindow (screen_window, this);
    }
    catch (xtl::exception& e)
    {
      e.touch ("syslib::tabletos::WindowImpl::WindowImpl");
      throw;
    }
  }
  
///Destructor
  ~WindowImpl ()
  {
    WindowRegistry::UnregisterWindow (screen_window);
    
      //destroys a window and free associated resources
      
    screen_destroy_window (screen_window);

      //destroys screen context
      
    screen_destroy_context (screen_context);
  }

///  
  void GetEventContext (WindowEventContext& context)
  {
    memset (&context, 0, sizeof (context));

    context.handle = screen_window;
  }


  void OnMtouch (int event_type, screen_event_t event)
  {
    try
    {
      WindowEventContext context;
    
      GetEventContext (context);

      int pos[2];
      if (screen_get_event_property_iv (event, SCREEN_PROPERTY_SOURCE_POSITION, (int*)&pos))
        raise_error ("::screen_get_event_property_iv SCREEN_PROPERTY_SOURCE_POSITION");

      mtouch_event_t mtouch_event;
      if (screen_get_mtouch_event(event, &mtouch_event, 0))
        raise_error ("::screen_get_mtouch_event");

      context.touches_count = 1;
      context.touches[0].position.x = mtouch_event.x;
      context.touches[0].position.y = mtouch_event.y;

      int id;
      if (screen_get_event_property_iv (event, SCREEN_PROPERTY_TOUCH_ID, &id))
        raise_error ("::screen_get_event_property_iv SCREEN_PROPERTY_TOUCH_ID");

      context.touches[0].touch_id = mtouch_event.contact_id;
      switch (event_type)
      {
        case SCREEN_EVENT_MTOUCH_TOUCH:   // Dispatched when a multi-touch event is detected.
          Notify (WindowEvent_OnTouchesBegan, context);
          break;
        case SCREEN_EVENT_MTOUCH_MOVE:    // Dispatched when a multi-touch move event is detected, for example when the user moves his or her fingers to make an input gesture. 
          Notify (WindowEvent_OnTouchesMoved ,context);
          break;
        case SCREEN_EVENT_MTOUCH_RELEASE: // Dispatched when a multi-touch release event occurs, or when the user completes the multi-touch gesture.
          Notify (WindowEvent_OnTouchesEnded, context);
          break;
      }
    } 
    catch (xtl::exception& exception)
    {
      exception.touch ("syslib::TabletOsWindowManager::OnMtouch");
      throw;
    }
  }

  void OnKeyboard (int event_type, screen_event_t event)
  {
    try
    {
      WindowEventContext context;
    
      GetEventContext (context);

      int flags;
      if (screen_get_event_property_iv (event, SCREEN_PROPERTY_KEY_FLAGS, &flags))
        raise_error ("::screen_get_event_property_iv SCREEN_PROPERTY_KEY_FLAGS");

      int key_sym;
      if (screen_get_event_property_iv (event, SCREEN_PROPERTY_KEY_SYM, &key_sym))
        raise_error ("::screen_get_event_property_iv SCREEN_PROPERTY_KEY_SYM");

      int key_mod;
      if (screen_get_event_property_iv (event, SCREEN_PROPERTY_KEY_MODIFIERS, &key_mod))
        raise_error ("::screen_get_event_property_iv SCREEN_PROPERTY_KEY_MODIFIERS");

      if (key_mod & KEYMOD_SHIFT) context.keyboard_shift_pressed = true;
      if (key_mod & KEYMOD_CTRL)  context.keyboard_control_pressed = true;
      if (key_mod & KEYMOD_ALT)   context.keyboard_alt_pressed = true;

      if (flags & KEY_SYM_VALID)  context.char_code = key_sym;
 
      if (flags & KEY_DOWN)
        Notify (WindowEvent_OnChar, context);
    } 
    catch (xtl::exception& exception)
    {
      exception.touch ("syslib::TabletOsWindowManager::OnKeyboard");
      throw;
    }
  }
 
  void OnProperty (int event_type, screen_event_t event)
  {
    try
    {
      WindowEventContext context;
    
      GetEventContext (context);

      int visible;
      int property;
  
      if (screen_get_event_property_iv (event, SCREEN_PROPERTY_NAME, &property))
        raise_error ("::screen_get_event_property_iv SCREEN_PROPERTY_NAME");
      
      switch (property)
      {
        case SCREEN_PROPERTY_POSITION:
          Notify (WindowEvent_OnMove, context);
          break; 

        case SCREEN_PROPERTY_SIZE:
          Notify (WindowEvent_OnSize, context);
          break; 

        case SCREEN_PROPERTY_SOURCE_SIZE:
          Notify (WindowEvent_OnSize, context);
          Notify (WindowEvent_OnChangeViewport, context);
          break;

        case SCREEN_PROPERTY_SOURCE_POSITION:
          Notify (WindowEvent_OnMove, context);
          Notify (WindowEvent_OnChangeViewport, context);
          break;
 
        case SCREEN_PROPERTY_VISIBLE:
          if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_VISIBLE, &visible))
            raise_error ("::screen_set_window_property_iv SCREEN_PROPERTY_VISIBLE");
   
          if (visible)
            Notify (WindowEvent_OnShow, context);
          else
            Notify (WindowEvent_OnHide, context);
          break;

        case SCREEN_PROPERTY_ZORDER:
          if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_ZORDER, &visible))
            raise_error ("::screen_set_window_property_iv SCREEN_PROPERTY_ZORDER");
   
          if (visible == 0)
            Notify (WindowEvent_OnActivate, context);
          else
            Notify (WindowEvent_OnDeactivate, context);
          break;
      }
    } 
    catch (xtl::exception& exception)
    {
      exception.touch ("syslib::TabletOsWindowManager::OnProperty");
      throw;
    }
  }

///Screen window event
  void OnWindowEvent (int event_type, screen_event_t event)
  {
    WindowEventContext context;
   
    GetEventContext (context);
 
    switch (event_type)
    {
      case SCREEN_EVENT_MTOUCH_TOUCH:   // Dispatched when a multi-touch event is detected.
      case SCREEN_EVENT_MTOUCH_MOVE:    // Dispatched when a multi-touch move event is detected, for example when the user moves his or her fingers to make an input gesture. 
      case SCREEN_EVENT_MTOUCH_RELEASE: // Dispatched when a multi-touch release event occurs, or when the user completes the multi-touch gesture.
        OnMtouch (event_type, event);
        break;

      case SCREEN_EVENT_KEYBOARD:
        OnKeyboard (event_type, event);
        break;
   
      case SCREEN_EVENT_PROPERTY:
        OnProperty (event_type, event);
        break;

      case SCREEN_EVENT_CREATE:         // Dispatched when a child window is created.
        break;

      case SCREEN_EVENT_CLOSE:          // Dispatched when a child window is destroyed.
        Notify (WindowEvent_OnClose, context);
        break;

      case SCREEN_EVENT_POINTER:        // Dispatched when a pointer input event occurs.
        break;
      default:
        break;
    }
  }

  void Notify (WindowEvent event, const WindowEventContext& context)
  {
    try
    {
      message_handler ((window_t)this, event, context, user_data);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }

};

}

/*
    Создание/закрытие/уничтожение окна
*/

window_t TabletOsWindowManager::CreateWindow (WindowStyle style, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  try
  {
    switch (style)
    {
      case WindowStyle_Overlapped:
      case WindowStyle_PopUp:
        break;
      default:
        throw xtl::make_argument_exception ("", "style", style);
    }
    
    if (parent_handle)
      throw xtl::format_not_supported_exception ("", "Child windows are not supported");
      
      //создание и инициализация окна

    stl::auto_ptr<WindowImpl> handle (new WindowImpl);
    
    handle->message_handler = handler;
    handle->user_data = user_data;

    return (window_t)handle.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::CreateWindow");
    throw;
  }
}

void TabletOsWindowManager::CloseWindow (window_t handle)
{
  try
  {
    WindowImpl *window = (WindowImpl*)handle;

    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);

    if (screen_leave_window_group (screen_window))
      raise_error ("::screen_leave_window_group");

    WindowEventContext context;

    memset (&context, 0, sizeof (context));          

    window->Notify (WindowEvent_OnClose, context);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::CloseWindow");
    throw;
  }
}

void TabletOsWindowManager::DestroyWindow (window_t handle)
{
  try
  {
    WindowImpl *window = (WindowImpl*)handle;

    if (!window)
      throw xtl::make_null_argument_exception ("", "window");      

    WindowEventContext context;

    memset (&context, 0, sizeof (context));          

    window->Notify (WindowEvent_OnDestroy, context);

    delete window;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::DestroyWindow");
    throw;
  }
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* TabletOsWindowManager::GetNativeWindowHandle (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    WindowImpl* window = (WindowImpl*)handle;
      
    return window->screen_window;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::GetNativeWindowHandle");
    throw;
  }
}

const void* TabletOsWindowManager::GetNativeDisplayHandle (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    WindowImpl* window = (WindowImpl*)handle;
    
    return window->screen_display;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::GetNativeWindowHandle");
    throw;
  }
}

/*
    Заголовок окна
*/

void TabletOsWindowManager::SetWindowTitle (window_t handle, const wchar_t* buffer)
{
  try
  {
    screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);
    
    stl::string title = common::tostring (buffer);

    if (screen_set_window_property_cv (screen_window, SCREEN_PROPERTY_ID_STRING, title.length (), &title[0]) != BPS_SUCCESS)
      raise_error ("::screen_set_window_property_cv(SCREEN_PROPERTY_ID_STRING)");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::TabletOsWindowManager::SetWindowTitle");
    throw;
  }
}

void TabletOsWindowManager::GetWindowTitle (window_t handle, size_t buffer_size, wchar_t* buffer)
{
  try
  {
    screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);

    char title [buffer_size];

    if (screen_get_window_property_cv (screen_window, SCREEN_PROPERTY_ID_STRING, buffer_size, title) != BPS_SUCCESS)
      raise_error ("::screen_get_window_property_cv(SCREEN_PROPERTY_ID_STRING)");

    stl::wstring result = common::towstring (title, buffer_size);

    xtl::xstrncpy (buffer, result.c_str(), buffer_size);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::TabletOsWindowManager::GetWindowTitle");
    throw;
  }

}

/*
    Область окна / клиентская область
*/

void TabletOsWindowManager::SetWindowRect (window_t handle, const Rect& rect)
{
  try
  {
    screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);

    int size [2] = {rect.right - rect.left, rect.bottom - rect.top}, position [2] = {rect.left, rect.top};

/*     (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_SIZE, size) != BPS_SUCCESS)
      raise_error ("::screen_set_window_property_iv(SCREEN_PROPERTY_SIZE)");

    if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_POSITION, position) != BPS_SUCCESS)
      raise_error ("::screen_set_window_property_iv(SCREEN_PROPERTY_POSITION)");
*/
    if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_SOURCE_POSITION, position) != BPS_SUCCESS)
      raise_error ("::screen_set_window_property_iv(SCREEN_PROPERTY_SOURCE_POSITION)");

    if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_SOURCE_SIZE, size) != BPS_SUCCESS)
      raise_error ("::screen_set_window_property_iv(SCREEN_PROPERTY_SOURCE_SIZE)");    

    WindowImpl *window = (WindowImpl*)handle;
    window->window_rect.left   = rect.left;
    window->window_rect.right  = rect.right; 
    window->window_rect.top    = rect.top;   
    window->window_rect.bottom = rect.bottom;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::TabletOsWindowManager::SetWindowRect");
    throw;
  }
}

void TabletOsWindowManager::SetClientRect (window_t handle, const Rect& rect)
{
  try
  {
    SetWindowRect (handle, rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::TabletOsWindowManager::SetClientRect");
    throw;
  }
}

void TabletOsWindowManager::GetWindowRect (window_t handle, Rect& rect)
{
  try
  {
    screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);
    WindowImpl *window = (WindowImpl*)handle;
    
    int size [2] = {0, 0}, position [2] = {0, 0};
    
    if (screen_get_window_property_iv (screen_window, SCREEN_PROPERTY_SOURCE_SIZE, size) != BPS_SUCCESS)
    {
      size [0] = window->window_rect.right - window->window_rect.left;
      size [1] = window->window_rect.bottom - window->window_rect.top;
    }

    if (screen_get_window_property_iv (screen_window, SCREEN_PROPERTY_SOURCE_POSITION, position) != BPS_SUCCESS)
    {
      position [0] = window->window_rect.left;
      position [1] = window->window_rect.top;
    }

    rect.left   = position [0];
    rect.right  = position [0] + size [0];
    rect.top    = position [1];
    rect.bottom = position [1] + size [1];

    window->window_rect.left   = rect.left;
    window->window_rect.right  = rect.right; 
    window->window_rect.top    = rect.top;   
    window->window_rect.bottom = rect.bottom;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::TabletOsWindowManager::GetWindowRect");
    throw;
  }
}

void TabletOsWindowManager::GetClientRect (window_t handle, Rect& rect)
{
  try
  {
    GetWindowRect (handle, rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::TabletOsWindowManager::GetClientRect");
    throw;
  }
}

/*
    Установка флагов окна
*/

void TabletOsWindowManager::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);
  WindowImpl*     window        = (WindowImpl*)handle;  

  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "handle");    
    
    switch (flag)
    {
      case WindowFlag_Visible:    
      {
        int value = state;
      
        if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_VISIBLE, &value) != BPS_SUCCESS)
          raise_error ("::screen_set_window_property_i(SCREEN_PROPERTY_VISIBLE)");
          
        InvalidateWindow (handle);

        break;
      }
      case WindowFlag_Active:
        break;
      case WindowFlag_Focus:
        break;
      case WindowFlag_Maximized:
      {
        int value = 1;
      
        if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_VISIBLE, &value) != BPS_SUCCESS)
          raise_error ("::screen_set_window_property_i(SCREEN_PROPERTY_VISIBLE)");        
        
        screen_display_mode_t screen_mode;

        if (screen_get_display_property_pv (window->screen_display, SCREEN_PROPERTY_MODE, (void**)&screen_mode))
          raise_error ("::screen_get_display_property_pv SCREEN_PROPERTY_MODE");                
          
        Rect rect (0, 0, screen_mode.width, screen_mode.height);

        SetWindowRect (handle, rect);

        break;
      }
      case WindowFlag_Minimized:
        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetWindowFlag");
    throw;
  }
}

bool TabletOsWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
      {
        int state = 0;
        
        if (screen_get_window_property_iv (screen_window, SCREEN_PROPERTY_VISIBLE, &state) != BPS_SUCCESS)
          raise_error ("::screen_get_window_property_iv(SCREEN_PROPERTY_VISIBLE)");        
        
        return state != 0;
      }
      case WindowFlag_Active:
      {
        int z_order = 0;
        
        if (screen_get_window_property_iv (screen_window, SCREEN_PROPERTY_ZORDER, &z_order) != BPS_SUCCESS)
          raise_error ("::screen_get_window_property_iv(SCREEN_PROPERTY_ZORDER)");        
          
        return z_order == 0;
      }
      case WindowFlag_Focus:
        break;
      case WindowFlag_Maximized:
        break;
      case WindowFlag_Minimized:
        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::WindowsWindowManager::SetWindowFlag");
    throw;
  }

  return false;
}

/*
    Установка родительского окна
*/

void TabletOsWindowManager::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  raise ("syslib::TabletOsWindowManager::SetParentWindow");
}

const void* TabletOsWindowManager::GetParentWindowHandle (window_t child)
{
  raise ("syslib::TabletOsWindowManager::GetParentWindow");

  return 0;
}

/*
   Установка multitouch режима для окна
*/

void TabletOsWindowManager::SetMultitouchEnabled (window_t handle, bool enabled)
{
  try
  {
    WindowImpl* window = (WindowImpl*)handle;  

    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    window->is_multitouch_enabled = enabled;    //??????? filter touch events to keep only one touch if multitouch disabled
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::SetMultitouchEnabled");
    throw;
  }
}

bool TabletOsWindowManager::IsMultitouchEnabled (window_t handle)
{
  try
  {
    WindowImpl* window = (WindowImpl*)handle;
  
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    return window->is_multitouch_enabled;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::IsMultitouchEnabled");
    throw;
  }
}

/*
    Обновление окна
*/

void TabletOsWindowManager::InvalidateWindow (window_t handle)
{
  screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);

  screen_buffer_t screen_buffer [2];

  screen_get_window_property_pv (screen_window, SCREEN_PROPERTY_RENDER_BUFFERS, (void **)screen_buffer);

  int rect [4] = {0, 0, 1, 1};

  if (screen_get_window_property_iv (screen_window, SCREEN_PROPERTY_BUFFER_SIZE, rect+2) != BPS_SUCCESS)
    raise_error ("::screen_get_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE)");
    
  if (screen_post_window (screen_window, screen_buffer [0], 1, rect, 0)!= BPS_SUCCESS)
    raise_error ("::screen_post_window");
}

/*
    Цвет фона
*/

void TabletOsWindowManager::SetBackgroundColor (window_t handle, const Color& color)
{
  try
  {
    screen_window_t screen_window = (screen_window_t)GetNativeWindowHandle (handle);

    int native_color = 0xff0000ff;

    if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_COLOR, &native_color))
      raise_error ("::screen_set_window_property_iv (SCREEN_PROPERTY_COLOR)");
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::SetBackgroundColor");
    throw;
  }
}

void TabletOsWindowManager::SetBackgroundState (window_t handle, bool state)
{
  WindowImpl* window = (WindowImpl*)handle;
  
  window->background_state = state; 
}

Color TabletOsWindowManager::GetBackgroundColor (window_t handle)
{
  screen_window_t screen_window = (screen_window_t)GetNativeWindowHandle (handle);

  int color = 0;

  screen_get_window_property_iv (screen_window, SCREEN_PROPERTY_COLOR, &color);
  
  return Color (0, 0, 0);
}

bool TabletOsWindowManager::GetBackgroundState (window_t handle)
{
  raise ("syslib::TabletOsWindowManager::GetBackgroundState");
  
  return false;
}

/*
    Window registry (used for )
*/

void WindowRegistry::RegisterWindow (screen_window_t window, IWindowImpl* impl)
{
  WindowRegistrySingleton::Instance ()->RegisterWindow (window, impl);
}

void WindowRegistry::UnregisterWindow (screen_window_t window)
{
  WindowRegistrySingleton::Instance ()->UnregisterWindow (window);
}

IWindowImpl* WindowRegistry::FindWindow (screen_window_t window)
{
  return WindowRegistrySingleton::Instance ()->FindWindow (window);
}

