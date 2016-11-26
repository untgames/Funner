#include "shared.h"

using namespace Windows::UI::Core;
using namespace Windows::ApplicationModel::Core;
using namespace Windows::ApplicationModel::Activation;
using namespace Windows::Foundation;

using namespace syslib::win8;

#ifdef CreateWindow
#undef CreateWindow
#endif

namespace syslib
{

namespace
{

/// Получение контекста по умолчанию
inline WindowEventContext get_event_context ()
{
  WindowEventContext context;

  memset (&context, 0, sizeof (context));

  return context;
}

/// Реализация окна
struct WindowImpl: public xtl::reference_counter
{
  Platform::Agile<CoreWindow> window;          //окно
  WindowMessageHandler        message_handler; //обработчик событий окна
  void*                       user_data;       //пользовательские данные обработчика

/// Конструктор
  WindowImpl (CoreWindow^ in_window, WindowMessageHandler in_message_handler, void* in_user_data)
    : window (in_window)
    , message_handler (in_message_handler)
    , user_data (in_user_data)
  {
    window->Closed          += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^> (xtl::bind (&WindowImpl::OnClosed, this), Platform::CallbackContext::Same);
    window->KeyDown         += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^> (xtl::bind (&WindowImpl::OnKeyDown, this, _2), Platform::CallbackContext::Same);
    window->KeyUp           += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^> (xtl::bind (&WindowImpl::OnKeyUp, this, _2), Platform::CallbackContext::Same);
    window->SizeChanged     += ref new TypedEventHandler<CoreWindow^, WindowSizeChangedEventArgs^> (xtl::bind (&WindowImpl::OnSizeChanged, this, _2), Platform::CallbackContext::Same);
    window->PointerPressed  += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^> (xtl::bind (&WindowImpl::OnPointerPressed, this, _2), Platform::CallbackContext::Same);
    window->PointerReleased += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^> (xtl::bind (&WindowImpl::OnPointerReleased, this, _2), Platform::CallbackContext::Same);
    window->PointerMoved    += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^> (xtl::bind (&WindowImpl::OnPointerMoved, this, _2), Platform::CallbackContext::Same);
    window->PointerEntered  += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^> (xtl::bind (&WindowImpl::OnPointerEntered, this, _2), Platform::CallbackContext::Same);
    window->PointerExited   += ref new TypedEventHandler<CoreWindow^, PointerEventArgs^> (xtl::bind (&WindowImpl::OnPointerExited, this, _2), Platform::CallbackContext::Same);
  }

/// Деструктор
  ~WindowImpl ()
  {
  }

/// События
  void Notify (WindowEvent event, const WindowEventContext& context = get_event_context ())
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

  void OnClosed ()
  {
    Notify (WindowEvent_OnClose);
  }

  void OnKeyDown (KeyEventArgs^ args)
  {
    WindowEventContext context = get_event_context ();

    context.key           = get_key_code (args->VirtualKey);
    context.key_scan_code = (ScanCode)args->VirtualKey;

    Notify (WindowEvent_OnKeyDown, context);
  }

  void OnKeyUp (KeyEventArgs^ args)
  {
    WindowEventContext context = get_event_context ();

    context.key           = get_key_code (args->VirtualKey);
    context.key_scan_code = (ScanCode)args->VirtualKey;

    Notify (WindowEvent_OnKeyUp, context);
  }

  WindowEventContext GetPointerContext (PointerEventArgs^ args)
  {
    WindowEventContext context = get_event_context ();

#ifndef WP8
    Windows::System::VirtualKeyModifiers mods = args->KeyModifiers;

    context.keyboard_alt_pressed     = (mods & Windows::System::VirtualKeyModifiers::Menu) != Windows::System::VirtualKeyModifiers::None;
    context.keyboard_control_pressed = (mods & Windows::System::VirtualKeyModifiers::Control) != Windows::System::VirtualKeyModifiers::None;
    context.keyboard_shift_pressed   = (mods & Windows::System::VirtualKeyModifiers::Shift) != Windows::System::VirtualKeyModifiers::None;
#endif

    Windows::Foundation::Point p = args->CurrentPoint->Position;

    context.touches_count        = 1;
    context.touches [0].touch_id = args->CurrentPoint->PointerId;
    context.touches [0].position = Point ((size_t)p.X, (size_t)p.Y);
    
    return context;
  }

  void OnPointerEntered (PointerEventArgs^ args)
  {
  }

  void OnPointerExited (PointerEventArgs^ args)
  {
    Notify (WindowEvent_OnMouseLeave, GetPointerContext (args));
  }

  void OnPointerMoved (PointerEventArgs^ args)
  {
    Notify (WindowEvent_OnTouchesMoved, GetPointerContext (args));
  }

  void OnPointerPressed (PointerEventArgs^ args)
  {    
    Notify (WindowEvent_OnTouchesBegan, GetPointerContext (args));
  }

  void OnPointerReleased (PointerEventArgs^ args)
  {    
    Notify (WindowEvent_OnTouchesEnded, GetPointerContext (args));
  }

  void OnSizeChanged (WindowSizeChangedEventArgs^ args)
  {
    Notify (WindowEvent_OnSize);
  }

  void OnRedraw ()
  {
    Notify (WindowEvent_OnPaint);
  }
};

}

/*
    Создание/закрытие/уничтожение окна
*/

window_t WindowsWindowManager::CreateWindow (WindowStyle, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data, const wchar_t*)
{
  try
  {
    if (parent_handle)
      throw xtl::format_not_supported_exception ("", "Parent windows is not supported in Windows8");

    xtl::intrusive_ptr<WindowImpl> impl (new WindowImpl (get_context ()->MainWindow (), handler, user_data), false);

    addref (impl.get ());

    return (window_t)impl.get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::CreateWindow");
    throw;
  }
}

void WindowsWindowManager::CloseWindow (window_t handle)
{
  try
  {
    WindowImpl* impl = (WindowImpl*)handle;

    if (!impl)
      throw xtl::make_null_argument_exception ("", "window");  

    impl->window->Close ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::CloseWindow");
    throw;
  }
}

void WindowsWindowManager::DestroyWindow (window_t handle)
{
  WindowImpl* impl = (WindowImpl*)handle;

  if (!impl)
    return;

  release (impl);
}

/*
    Попытка изменения стиля окна (может быть проигнорирована)
*/

bool WindowsWindowManager::ChangeWindowStyle (window_t window, WindowStyle style)
{
  return false;
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* WindowsWindowManager::GetNativeWindowHandle (window_t handle)
{
  try
  {
    WindowImpl* impl = (WindowImpl*)handle;

    if (!impl)
      throw xtl::make_null_argument_exception ("", "window");  
    
    return (void*)impl->window.Get ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GetNativeWindowHandle");
    throw;
  }
}

const void* WindowsWindowManager::GetNativeDisplayHandle (window_t)
{
  return 0;
}

/*
    Заголовок окна
*/

void WindowsWindowManager::SetWindowTitle (window_t, const wchar_t*)
{
}

void WindowsWindowManager::GetWindowTitle (window_t handle, size_t buffer_size, wchar_t* buffer)
{
  try
  {
    if (!buffer_size)
      return;

    static const wchar_t TITLE [] = L"Funner Application";

    if (buffer_size > sizeof (TITLE))
      buffer_size = sizeof (TITLE);

    memcpy (buffer, TITLE, buffer_size);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GetWindowTitle");
    throw;
  }
}

/*
    Область окна / клиентская область
*/

void WindowsWindowManager::SetWindowRect (window_t handle, const Rect&)
{
  try
  {
    WindowImpl* impl = (WindowImpl*)handle;

    if (!impl)
      throw xtl::make_null_argument_exception ("", "window");      

    impl->Notify (WindowEvent_OnSize);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::SetWindowRect");
    throw;
  }
}

void WindowsWindowManager::SetClientRect (window_t handle, const Rect& rect)
{
  try
  {
    SetWindowRect (handle, rect);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::SetClientRect");
    throw;
  }
}

void WindowsWindowManager::GetWindowRect (window_t handle, Rect& out_rect)
{
  try
  {
    WindowImpl* impl = (WindowImpl*)handle;

    if (!impl)
      throw xtl::make_null_argument_exception ("", "window");      

    Windows::Foundation::Rect rect = impl->window->Bounds;

    out_rect.left   = (size_t)rect.X;
    out_rect.top    = (size_t)rect.Y;
    out_rect.right  = (size_t)(rect.X + rect.Width);
    out_rect.bottom = (size_t)(rect.Y + rect.Height);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GetWindowRect");
    throw;
  }
}

void WindowsWindowManager::GetClientRect (window_t handle, Rect& rect)
{
  try
  {
    GetWindowRect (handle, rect);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GetClientRect");
    throw;
  }
}

/*
    Установка флагов окна
*/

void WindowsWindowManager::SetWindowFlag (window_t, WindowFlag, bool)
{
}

bool WindowsWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  try
  {
    WindowImpl* impl = (WindowImpl*)handle;

    if (!impl)
      throw xtl::make_null_argument_exception ("", "window");      

    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
        return impl->window->Visible;
      case WindowFlag_Active: //активность окна
        return true;
      case WindowFlag_Focus: //фокус ввода
        return true;
      case WindowFlag_Maximized:
      case WindowFlag_Minimized:
        throw xtl::format_operation_exception ("", "Can't get window flag %d value", flag);
      default:
        throw xtl::make_argument_exception ("", "flag", flag);  
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::GetWindowFlag");
    throw;
  }
}

/*
    Установка родительского окна
*/

void WindowsWindowManager::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  throw xtl::format_not_supported_exception ("syslib::WindowsWindowManager::SetParentWindow", "Child windows are not supported");
}

const void* WindowsWindowManager::GetParentWindowHandle (window_t child)
{
  return 0;
}

/*
   Установка multitouch режима для окна
*/

void WindowsWindowManager::SetMultitouchEnabled (window_t window, bool enabled)
{
}

bool WindowsWindowManager::IsMultitouchEnabled (window_t window)
{
  return false;
}

/*
    Обновление окна
*/

void WindowsWindowManager::InvalidateWindow (window_t handle)
{
  try
  {
    WindowImpl* impl = (WindowImpl*)handle;

    if (!impl)
      throw xtl::make_null_argument_exception ("", "window");

    common::ActionQueue::PushAction (xtl::bind (&WindowImpl::OnRedraw, xtl::intrusive_ptr<WindowImpl> (impl)), common::ActionThread_Main);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::WindowsWindowManager::InvalidateWindow");
    throw;
  }
}

/*
    Положение курсора
*/

void WindowsWindowManager::SetCursorPosition (const Point&)
{
  //ignore
//  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetCursorPosition");
}

Point WindowsWindowManager::GetCursorPosition ()
{
//  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetCursorPosition");

  return Point ();
}

void WindowsWindowManager::SetCursorPosition (window_t, const Point& client_position)
{
  //ignore
//  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetCursorPosition (window_t, const Point&)");
}

Point WindowsWindowManager::GetCursorPosition (window_t)
{
//  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetCursorPosition (window_t)");

  return Point ();
}

/*
    Видимость курсора
*/

void WindowsWindowManager::SetCursorVisible (window_t, bool state)
{
  if (state)
    throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetCursorVisible");  
}

bool WindowsWindowManager::GetCursorVisible (window_t)
{
  return false;
}

/*
    Изображение курсора
*/

cursor_t WindowsWindowManager::CreateCursor (const char*, int, int)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::CreateCursor");
  
  return 0;
}

void WindowsWindowManager::DestroyCursor (cursor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::DestroyCursor");
}

void WindowsWindowManager::SetCursor (window_t, cursor_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetCursor");
}

/*
    Цвет фона
*/

void WindowsWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
}

void WindowsWindowManager::SetBackgroundState (window_t window, bool state)
{
}

Color WindowsWindowManager::GetBackgroundColor (window_t window)
{
  return Color (0, 0, 0);
}

bool WindowsWindowManager::GetBackgroundState (window_t window)
{
  return true;
}

/*
    Создание/уничтожение web-view
*/

web_view_t WindowsWindowManager::CreateWebView (IWebViewListener*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::CreateWebView");
}

void WindowsWindowManager::DestroyWebView (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::DestroyWebView");
}

/*
    Присоединенное окно
*/

window_t WindowsWindowManager::GetWindow (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetWindow");
}

/*
    Загрузка данных
*/

void WindowsWindowManager::LoadRequest (web_view_t, const char*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::LoadRequest");
}

void WindowsWindowManager::LoadData (web_view_t, const char*, size_t, const char*, const char*, const char*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::LoadData");
}

/*
    Перезагрузка страницы / остановка загрузки / проверка наличия загрузки
*/

void WindowsWindowManager::Reload (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::Reload");
}

void WindowsWindowManager::StopLoading (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::StopLoading");
}

bool WindowsWindowManager::IsLoading (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::IsLoading");
  return false;  
}

/*
    Средства навигации
*/

bool WindowsWindowManager::CanGoBack (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::CanGoBack");
  return false;
}

bool WindowsWindowManager::CanGoForward (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::CanGoForward");
  return false;  
}

void WindowsWindowManager::GoBack (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GoBack");
}

void WindowsWindowManager::GoForward (web_view_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GoForward");
}

/*
    Получение имени клавиши
*/

//возвращается длина строки без учёта '\0'
size_t WindowsWindowManager::GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer)
{
  static const char* METHOD_NAME = "syslib::WindowsWindowManager::GetKeyName";

  if (scan_code < 0 || scan_code >= ScanCode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "scan_code", scan_code);

  if (!buffer && buffer_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (!buffer_size)
    return 0;

  char scan_str [2] = {*(char*)&scan_code, 0};
    
  strncpy (buffer, scan_str, buffer_size);

  return strlen (buffer);
}

}
