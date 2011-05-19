#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с окнами невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No window support for this platform");
}

}

/*
    Создание/закрытие/уничтожение окна
*/

window_t DefaultWindowManager::CreateWindow (WindowStyle, WindowMessageHandler, const void*, const char* init_string, void*)
{
  raise ("syslib::DefaultWindowManager::CreateWindow");

  return 0;
}

void DefaultWindowManager::CloseWindow (window_t)
{
  raise ("syslib::DefaultWindowManager::CloseWindow");
}

void DefaultWindowManager::DestroyWindow (window_t)
{
  raise ("syslib::DefaultWindowManager::DestroyWindow");
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* DefaultWindowManager::GetNativeWindowHandle (window_t)
{
  raise ("syslib::DefaultWindowManager::GetNativeWindowHandle");
  
  return 0;
}

const void* DefaultWindowManager::GetNativeDisplayHandle (window_t)
{
  raise ("syslib::DefaultWindowManager::GetNativeDisplayHandle");
  
  return 0;
}

/*
    Заголовок окна
*/

void DefaultWindowManager::SetWindowTitle (window_t, const wchar_t*)
{
  raise ("syslib::DefaultWindowManager::SetWindowTitle");
}

void DefaultWindowManager::GetWindowTitle (window_t, size_t, wchar_t*)
{
  raise ("syslib::DefaultWindowManager::GetWindowTitle");
}

/*
    Область окна / клиентская область
*/

void DefaultWindowManager::SetWindowRect (window_t, const Rect&)
{
  raise ("syslib::DefaultWindowManager::SetWindowRect");
}

void DefaultWindowManager::SetClientRect (window_t, const Rect&)
{
  raise ("syslib::DefaultWindowManager::SetClientRect");
}

void DefaultWindowManager::GetWindowRect (window_t, Rect&)
{
  raise ("syslib::DefaultWindowManager::GetWindowRect");
}

void DefaultWindowManager::GetClientRect (window_t, Rect&)
{
  raise ("syslib::DefaultWindowManager::GetClientRect");
}

/*
    Установка флагов окна
*/

void DefaultWindowManager::SetWindowFlag (window_t, WindowFlag, bool)
{
  raise ("syslib::DefaultWindowManager::SetWindowFlag");
}

bool DefaultWindowManager::GetWindowFlag (window_t, WindowFlag)
{
  raise ("syslib::DefaultWindowManager::GetWindowFlag");

  return false;
}

/*
    Установка родительского окна
*/

void DefaultWindowManager::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  raise ("syslib::DefaultWindowManager::SetParentWindow");
}

const void* DefaultWindowManager::GetParentWindowHandle (window_t child)
{
  raise ("syslib::DefaultWindowManager::GetParentWindow");

  return 0;
}

/*
   Установка multitouch режима для окна
*/

void DefaultWindowManager::SetMultitouchEnabled (window_t window, bool enabled)
{
  if (enabled)
    raise ("syslib::DefaultWindowManager::SetMultitouchEnabled");      
}

bool DefaultWindowManager::IsMultitouchEnabled (window_t window)
{
  return false;
}

/*
    Обновление окна
*/

void DefaultWindowManager::InvalidateWindow (window_t)
{
  raise ("syslib::DefaultWindowManager::InvalidateWindow");
}

/*
    Положение курсора
*/

void DefaultWindowManager::SetCursorPosition (const Point&)
{
  //ignore
//  raise ("syslib::DefaultWindowManager::SetCursorPosition");
}

Point DefaultWindowManager::GetCursorPosition ()
{
//  raise ("syslib::DefaultWindowManager::GetCursorPosition");

  return Point ();
}

void DefaultWindowManager::SetCursorPosition (window_t, const Point& client_position)
{
  //ignore
//  throw xtl::make_not_implemented_exception ("syslib::DefaultWindowManager::SetCursorPosition (window_t, const Point&)");
}

Point DefaultWindowManager::GetCursorPosition (window_t)
{
//  throw xtl::make_not_implemented_exception ("syslib::DefaultWindowManager::GetCursorPosition (window_t)");

  return Point ();
}

/*
    Видимость курсора
*/

void DefaultWindowManager::SetCursorVisible (window_t, bool state)
{
  if (state)
    raise ("syslib::DefaultWindowManager::SetCursorVisible");  
}

bool DefaultWindowManager::GetCursorVisible (window_t)
{
  return false;
}

/*
    Изображение курсора
*/

cursor_t DefaultWindowManager::CreateCursor (const char*, int, int)
{
  raise ("syslib::DefaultWindowManager::CreateCursor");
  
  return 0;
}

void DefaultWindowManager::DestroyCursor (cursor_t)
{
  raise ("syslib::DefaultWindowManager::DestroyCursor");
}

void DefaultWindowManager::SetCursor (window_t, cursor_t)
{
  raise ("syslib::DefaultWindowManager::SetCursor");
}

/*
    Цвет фона
*/

void DefaultWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  raise ("syslib::DefaultWindowManager::SetBackgroundColor");
}

void DefaultWindowManager::SetBackgroundState (window_t window, bool state)
{
  raise ("syslib::DefaultWindowManager::SetBackgroundState");
}

Color DefaultWindowManager::GetBackgroundColor (window_t window)
{
  raise ("syslib::DefaultWindowManager::GetBackgroundColor");

  return Color (0, 0, 0);
}

bool DefaultWindowManager::GetBackgroundState (window_t window)
{
  raise ("syslib::DefaultWindowManager::GetBackgroundState");
  
  return false;
}

/*
    Получение имени клавиши
*/

//возвращается длина строки без учёта '\0'
size_t DefaultWindowManager::GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer)
{
  static const char* METHOD_NAME = "syslib::DefaultWindowManager::GetKeyName";

  if (scan_code < 0 || scan_code >= ScanCode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "scan_code", scan_code);

  if (!buffer && buffer_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (!buffer_size)
    return 0;
    
  strncpy (buffer, "Unknown", buffer_size);

  return strlen (buffer);
}
