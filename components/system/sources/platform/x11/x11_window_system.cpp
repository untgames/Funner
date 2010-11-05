#include "shared.h"

using namespace syslib;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_WINDOW_X      = 0;
const size_t DEFAULT_WINDOW_Y      = 0;
const size_t DEFAULT_WINDOW_WIDTH  = 100;
const size_t DEFAULT_WINDOW_HEIGHT = 100;

}

/*
    Описание реализации окна
*/

typedef ::Window XWindow;

struct Platform::window_handle
{
  Display*  display;          //display for this window
  XWindow   window;           //window handle
  bool      background_state; //background state of this window (fake property)
  
  window_handle ()
    : display (0)
    , window (0)
    , background_state (true)
  {
  }
};

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle style, WindowMessageHandler handler, window_t parent, const char* init_string, void* user_data)
{
  try
  {
      //определение стиля окна
      
    switch (style)
    {
      case WindowStyle_Overlapped:
      case WindowStyle_PopUp: //not implemented
        break;
      default:
        throw xtl::make_argument_exception ("", "style", style);
    }
    
      //создание дескриптора окна

    stl::auto_ptr<window_handle> impl (new window_handle);

      //открытие дисплея

    common::PropertyMap properties = common::parse_init_string (init_string);

    const char* display_name = properties.IsPresent ("display") ? properties.GetString ("display") : 0;

    impl->display = XOpenDisplay (display_name);

    if (!impl->display)
      throw xtl::format_operation_exception ("", "Can't open display '%s'", XDisplayName (display_name));

    bool synchronize = properties.IsPresent ("synchronize") && properties.GetInteger ("synchronize") != 0;
    
    if (synchronize)
      XSynchronize (impl->display, true);          

    int blackColor = BlackPixel (impl->display, DefaultScreen (impl->display));
    
    XWindow parent_window = 0;
    
    if (parent) parent_window = parent->window;
    else        parent_window = DefaultRootWindow (impl->display);
    
    impl->window = XCreateSimpleWindow (impl->display, parent_window, DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y,
      DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, blackColor, blackColor);

    if (!impl->window)
      throw xtl::format_operation_exception ("", "Can't create window for display '%s'", XDisplayName (display_name));

    XSelectInput (impl->display, impl->window, StructureNotifyMask | ExposureMask | ButtonPressMask | KeyPressMask | KeyReleaseMask);  
    
    XFlush (impl->display);    
    
      //event processing!!!!
    
    return impl.release (); 
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::CreateWindow");
    throw;  
  }
}

void Platform::CloseWindow (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
    
    XEvent e;
     
    memset (&e, 0, sizeof (e));
     
    e.xclient.type         = ClientMessage;
    e.xclient.window       = handle->window;
    e.xclient.message_type = XInternAtom (handle->display, "WM_PROTOCOLS", true);
    e.xclient.format       = 32;
    e.xclient.data.l [0]   = XInternAtom (handle->display, "WM_DELETE_WINDOW", false);
    e.xclient.data.l [1]   = CurrentTime;

    if (!XSendEvent (handle->display, handle->window, False, NoEventMask, &e))
      throw xtl::format_operation_exception ("", "XSendEvent failed");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::CloseWindow");
    throw;  
  }
}

void Platform::DestroyWindow (window_t handle)
{
  try
  {
    if (!XDestroyWindow (handle->display, handle->window))
      throw xtl::format_operation_exception ("", "XDestroyWindow failed");
      
    if (!XCloseDisplay (handle->display))
      throw xtl::format_operation_exception ("", "XCloseDisplay failed");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::DestroyWindow");
    throw;  
  }
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* Platform::GetNativeWindowHandle (window_t handle)
{
  return reinterpret_cast<const void*> (handle->window);
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t handle, const wchar_t* title)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
    
    XStoreName (handle->display, handle->window, common::tostring (title).c_str ());
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetWindowTitle");
    throw;  
  }
}

void Platform::GetWindowTitle (window_t handle, size_t buffer_size_in_chars, wchar_t* buffer)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
    
    char* window_name = 0;
    
    if (!XFetchName (handle->display, handle->window, &window_name) || !window_name)
      throw xtl::format_operation_exception ("", "XFetchName failed");
      
    stl::wstring result = common::towstring (window_name);
    
    xtl::xstrncpy (buffer, result.c_str (), buffer_size_in_chars);
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetWindowTitle");
    throw;  
  }
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t handle, const Rect& rect)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
    
    XWindowChanges changes;
    
    memset (&changes, 0, sizeof (XWindowChanges));        
    
    changes.x      = (int)rect.left;
    changes.y      = (int)rect.top;
    changes.width  = rect.right - rect.left;
    changes.height = rect.bottom - rect.top;
    
    if (!XConfigureWindow (handle->display, handle->window, CWX | CWY | CWWidth | CWHeight, &changes))
      throw xtl::format_operation_exception ("", "XConfigureWindow failed");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetWindowRect");
    throw;  
  }
}

void Platform::SetClientRect (window_t handle, const Rect& rect)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetClientRect");
    throw;  
  }
}

void Platform::GetWindowRect (window_t handle, Rect& rect)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    XWindow root_return = 0;
    int x_return = 0, y_return = 0;
    unsigned int width_return = 0, height_return = 0, border_width_return = 0, depth_return = 0;
    
    if (!XGetGeometry (handle->display, handle->window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return))
      throw xtl::format_operation_exception ("", "XGetGeometry failed");
      
    rect.left   = (size_t)x_return;
    rect.top    = (size_t)y_return;
    rect.right  = rect.left + width_return;
    rect.bottom = rect.top + height_return;
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetWindowRect");
    throw;  
  }
}

void Platform::GetClientRect (window_t handle, Rect& rect)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetClientRect");
    throw;  
  }
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
    
    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
      {
        if (!XMapWindow (handle->display, handle->window))
          throw xtl::format_operation_exception ("", "XMapWindow failed");
          
        XFlush (handle->display);

        break;
      }
      case WindowFlag_Active: //активность окна
        break;
      case WindowFlag_Focus: //фокус ввода
        break;
      case WindowFlag_Maximized:
        break;
      case WindowFlag_Minimized:            
        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetWindowFlag");
    throw;  
  }
}

bool Platform::GetWindowFlag (window_t handle, WindowFlag flag)
{
  try
  {
    return false;    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetWindowFlag");
    throw;  
  }
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindow (window_t child, window_t parent)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetParentWindow");
    throw;  
  }
}

Platform::window_t Platform::GetParentWindow (window_t child)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
    
    if (!child)
      throw xtl::make_null_argument_exception ("", "child");
      
    XWindow root_return = 0, parent_return = 0, *children_return = 0;
    unsigned int nchildren_return = 0;
    
    if (!XQueryTree (child->display, child->window, &root_return, &parent_return, &children_return, &nchildren_return))
      throw xtl::format_operation_exception ("", "XQueryTree failed");
    
//    return ????; 
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetParentWindow");
    throw;  
  }
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t handle)
{
  try
  {    
    //????
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::InvalidateWindow");
    throw;  
  }
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point& position)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursorPosition(const Point&)");
    throw;  
  }
}

syslib::Point Platform::GetCursorPosition ()
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetCursorPosition()");
    throw;  
  }
}

void Platform::SetCursorPosition (window_t handle, const Point& client_position)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursorPosition(window_t,const Point&)");
    throw;  
  }
}

syslib::Point Platform::GetCursorPosition (window_t handle)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetCursorPosition(window_t)");
    throw;  
  }
}

/*
    Видимость курсора
*/

void Platform::SetCursorVisible (window_t, bool state)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursorVisible");
    throw;  
  }
}

bool Platform::GetCursorVisible (window_t)
{
  try
  {
    throw xtl::make_not_implemented_exception (""); 
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetCursorVisible");
    throw;  
  }  
}

/*
    Изображение курсора
*/

Platform::cursor_t Platform::CreateCursor (const char* file_name, int hotspot_x, int hotspot_y)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::X11Platform::CreateCursor");
    throw;
  }
}

void Platform::DestroyCursor (cursor_t cursor)
{
  try
  {
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::X11Platform::DestroyCursor");
    throw;
  }
}

void Platform::SetCursor (window_t window, cursor_t cursor)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursor");
    throw;
  }
}

/*
    Цвет фона
*/

void Platform::SetBackgroundColor (window_t handle, const Color& color)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetBackgroundColor");
    throw;
  }
}

void Platform::SetBackgroundState (window_t handle, bool state)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");    

    handle->background_state = state;
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetBackgroundState");
    throw;
  }
}

Color Platform::GetBackgroundColor (window_t handle)
{
  try
  {
    return Color (0, 0, 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetBackgroundColor");
    throw;
  }
}

bool Platform::GetBackgroundState (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");    
    
    return handle->background_state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetBackgroundState");
    throw;
  }
}
