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
    Window implementation
*/

struct WindowImpl
{
  screen_context_t screen_context; // A context encapsulates the connection to the windowing system
  screen_window_t  screen_window;  // The window is the most basic drawing surface.
  screen_display_t screen_display; // A display represents the physical display hardware such as a touch screen display.
  int              usage;          //
  int              format;         //
  int              nbuffers;       // count of buffers
  
///Constructor
  WindowImpl ()
    : screen_window (0)
    , format (SCREEN_FORMAT_RGBX8888)
    , nbuffers (2)
  {
    try
    {
      platform_initialize ();
      
        //create a screen context that will be used to create an EGL surface to to receive libscreen events.
        
      if (screen_create_context (&screen_context, SCREEN_APPLICATION_CONTEXT) != BPS_SUCCESS)
        raise_error ("::screen_create_context");

        //creates a window that can be used to make graphical content visible on a display
        
      if (screen_create_window (&screen_window, screen_context) != BPS_SUCCESS)
        raise_error ("::screen_create_window");
       
        //
        
      if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_FORMAT, &format))
        raise_error ("::screen_set_window_property_iv(SCREEN_PROPERTY_FORMAT)");
        
        //

      if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_USAGE, &usage))
        raise_error ("screen_set_window_property_iv(SCREEN_PROPERTY_USAGE)");

        //get display

      if (screen_get_window_property_pv (screen_window, SCREEN_PROPERTY_DISPLAY, (void**)&screen_display) != BPS_SUCCESS)
        raise_error ("::screen_get_window_property_pv(SCREEN_PROPERTY_DISPLAY)");
        
        //

      if (screen_create_window_buffers (screen_window, nbuffers))
        raise_error ("::screen_create_window_buffers");
        
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
      //destroys a window and free associated resources
      
    screen_destroy_window (screen_window);

      //destroys screen context
      
    screen_destroy_context (screen_context);
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
        break;
      case WindowStyle_PopUp:
        throw xtl::format_not_supported_exception ("", "WindowStyle_Popup not supported");
      default:
        throw xtl::make_argument_exception ("", "style", style);
    }
    
    if (parent_handle)
      throw xtl::format_not_supported_exception ("", "Child windows are not supported");
      
      //создание и инициализация окна

    stl::auto_ptr<WindowImpl> handle (new WindowImpl);
    
    return (window_t)handle.release ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::TabletOsWindowManager::CreateWindow");
    throw;
  }
}

void TabletOsWindowManager::CloseWindow (window_t)
{
  raise ("syslib::TabletOsWindowManager::CloseWindow");
}

void TabletOsWindowManager::DestroyWindow (window_t)
{
  raise ("syslib::TabletOsWindowManager::DestroyWindow");
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

    if (screen_get_window_property_cv (screen_window, SCREEN_PROPERTY_ID_STRING, title.length (), &title[0]) != BPS_SUCCESS)
      raise_error ("::screen_get_window_property_cv(SCREEN_PROPERTY_ID_STRING)");
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

    char title[buffer_size];

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
    
    int buffer_size[2] = {rect.right, rect.bottom};
    
    if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size) != BPS_SUCCESS)
      raise_error ("::screen_set_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE)");
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
    screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);
    
    int buffer_size[2] = {rect.right, rect.bottom};
    
    if (screen_set_window_property_iv (screen_window, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size) != BPS_SUCCESS)
      raise_error ("::screen_set_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE)");
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
    
    int buffer_size[2] = {0, 0};
    
    if (screen_get_window_property_iv (screen_window, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size) != BPS_SUCCESS)
      raise_error ("::screen_get_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE)");

    rect.left   = 0;
    rect.right  = buffer_size[0];
    rect.top    = 0;
    rect.bottom = buffer_size[1];
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
    screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);
    
    int buffer_size[2] = {0, 0};
    
    if (screen_get_window_property_iv (screen_window, SCREEN_PROPERTY_BUFFER_SIZE, buffer_size) != BPS_SUCCESS)
      raise_error ("::screen_get_window_property_iv(SCREEN_PROPERTY_BUFFER_SIZE)");

    rect.left   = 0;
    rect.right  = buffer_size[0];
    rect.top    = 0;
    rect.bottom = buffer_size[1];
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

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
        break;
      case WindowFlag_Active:
        break;
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
}

bool TabletOsWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  screen_window_t screen_window = (screen_window_t) GetNativeWindowHandle (handle);

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
        break;
      case WindowFlag_Active:
        break;
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
//  if (enabled)
//    raise ("syslib::TabletOsWindowManager::SetMultitouchEnabled");      
}

bool TabletOsWindowManager::IsMultitouchEnabled (window_t handle)
{
  return false;
}

/*
    Обновление окна
*/

void TabletOsWindowManager::InvalidateWindow (window_t)
{
  raise ("syslib::TabletOsWindowManager::InvalidateWindow");
}

/*
    Положение курсора
*/

void TabletOsWindowManager::SetCursorPosition (const Point&)
{
  //ignore
//  raise ("syslib::TabletOsWindowManager::SetCursorPosition");
}

Point TabletOsWindowManager::GetCursorPosition ()
{
//  raise ("syslib::TabletOsWindowManager::GetCursorPosition");

  return Point ();
}

void TabletOsWindowManager::SetCursorPosition (window_t, const Point& client_position)
{
  //ignore
//  throw xtl::make_not_implemented_exception ("syslib::TabletOsWindowManager::SetCursorPosition (window_t, const Point&)");
}

Point TabletOsWindowManager::GetCursorPosition (window_t)
{
//  throw xtl::make_not_implemented_exception ("syslib::TabletOsWindowManager::GetCursorPosition (window_t)");

  return Point ();
}

/*
    Видимость курсора
*/

void TabletOsWindowManager::SetCursorVisible (window_t, bool state)
{
  if (state)
    raise ("syslib::TabletOsWindowManager::SetCursorVisible");  
}

bool TabletOsWindowManager::GetCursorVisible (window_t)
{
  return false;
}

/*
    Изображение курсора
*/

cursor_t TabletOsWindowManager::CreateCursor (const char*, int, int)
{
  raise ("syslib::TabletOsWindowManager::CreateCursor");
  
  return 0;
}

void TabletOsWindowManager::DestroyCursor (cursor_t)
{
  raise ("syslib::TabletOsWindowManager::DestroyCursor");
}

void TabletOsWindowManager::SetCursor (window_t, cursor_t)
{
  raise ("syslib::TabletOsWindowManager::SetCursor");
}

/*
    Цвет фона
*/

void TabletOsWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  raise ("syslib::TabletOsWindowManager::SetBackgroundColor");
}

void TabletOsWindowManager::SetBackgroundState (window_t window, bool state)
{
  raise ("syslib::TabletOsWindowManager::SetBackgroundState");
}

Color TabletOsWindowManager::GetBackgroundColor (window_t window)
{
  raise ("syslib::TabletOsWindowManager::GetBackgroundColor");

  return Color (0, 0, 0);
}

bool TabletOsWindowManager::GetBackgroundState (window_t window)
{
  raise ("syslib::TabletOsWindowManager::GetBackgroundState");
  
  return false;
}

/*
    Получение имени клавиши
*/

//возвращается длина строки без учёта '\0'
size_t TabletOsWindowManager::GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer)
{
  static const char* METHOD_NAME = "syslib::TabletOsWindowManager::GetKeyName";

  if (scan_code < 0 || scan_code >= ScanCode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "scan_code", scan_code);

  if (!buffer && buffer_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (!buffer_size)
    return 0;
    
  strncpy (buffer, "Unknown", buffer_size);

  return strlen (buffer);
}
