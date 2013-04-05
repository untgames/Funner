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
struct WindowImpl
{
  Platform::Agile<CoreWindow> window;          //окно
  WindowMessageHandler        message_handler; //обработчик событий окна
  void*                       user_data;       //пользовательские данные обработчика

/// Конструктор
  WindowImpl (WindowMessageHandler in_message_handler, void* in_user_data)
    : message_handler (in_message_handler)
    , user_data (in_user_data)
  {
      CoreWindow^ window = this->window.Get ();

      window->Closed  += ref new TypedEventHandler<CoreWindow^, CoreWindowEventArgs^> (xtl::bind (&WindowImpl::OnClosed, this), Platform::CallbackContext::Same);
      window->KeyDown += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^> (xtl::bind (&WindowImpl::OnKeyDown, this, _2), Platform::CallbackContext::Same);
      window->KeyUp   += ref new TypedEventHandler<CoreWindow^, KeyEventArgs^> (xtl::bind (&WindowImpl::OnKeyDown, this, _2), Platform::CallbackContext::Same);
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

    context.key = get_key_code (args->VirtualKey);

    Notify (WindowEvent_OnKeyDown, context);
  }

  void OnKeyUp (KeyEventArgs^ args)
  {
    WindowEventContext context = get_event_context ();

    context.key = get_key_code (args->VirtualKey);

    Notify (WindowEvent_OnKeyUp, context);
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

    std::auto_ptr<WindowImpl> impl (new WindowImpl (handler, user_data));

    impl->window = get_context ()->MainWindow ();

    return (window_t)impl.release ();
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

  delete impl;
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
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetWindowTitle");
}

void WindowsWindowManager::GetWindowTitle (window_t, size_t, wchar_t*)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetWindowTitle");
}

/*
    Область окна / клиентская область
*/

void WindowsWindowManager::SetWindowRect (window_t, const Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetWindowRect");
}

void WindowsWindowManager::SetClientRect (window_t, const Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetClientRect");
}

void WindowsWindowManager::GetWindowRect (window_t, Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetWindowRect");
}

void WindowsWindowManager::GetClientRect (window_t, Rect&)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetClientRect");
}

/*
    Установка флагов окна
*/

void WindowsWindowManager::SetWindowFlag (window_t, WindowFlag, bool)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetWindowFlag");
}

bool WindowsWindowManager::GetWindowFlag (window_t, WindowFlag)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetWindowFlag");

  return false;
}

/*
    Установка родительского окна
*/

void WindowsWindowManager::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetParentWindow");
}

const void* WindowsWindowManager::GetParentWindowHandle (window_t child)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetParentWindow");

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

void WindowsWindowManager::InvalidateWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::InvalidateWindow");
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
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetBackgroundColor");
}

void WindowsWindowManager::SetBackgroundState (window_t window, bool state)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::SetBackgroundState");
}

Color WindowsWindowManager::GetBackgroundColor (window_t window)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetBackgroundColor");

  return Color (0, 0, 0);
}

bool WindowsWindowManager::GetBackgroundState (window_t window)
{
  throw xtl::make_not_implemented_exception ("syslib::WindowsWindowManager::GetBackgroundState");
  
  return false;
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
    
  strncpy (buffer, "Unknown", buffer_size);

  return strlen (buffer);
}

}
