#include "shared.h"

using namespace syslib;

/*
    Генерация исключения: работа с окнами невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  throw xtl::format_not_supported_exception (method_name, "No window support for default platform");
}

}

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle, WindowMessageHandler, const void*, const char* init_string, void*)
{
  raise ("syslib::DefaultPlatform::CreateWindow");

  return 0;
}

void Platform::CloseWindow (window_t)
{
  raise ("syslib::DefaultPlatform::CloseWindow");
}

void Platform::DestroyWindow (window_t)
{
  raise ("syslib::DefaultPlatform::DestroyWindow");
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* Platform::GetNativeWindowHandle (window_t)
{
  raise ("syslib::DefaultPlatform::GetNativeWindowHandle");
}

const void* Platform::GetNativeDisplayHandle (window_t)
{
  raise ("syslib::DefaultPlatform::GetNativeDisplayHandle");
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t, const wchar_t*)
{
  raise ("syslib::DefaultPlatform::SetWindowTitle");
}

void Platform::GetWindowTitle (window_t, size_t, wchar_t*)
{
  raise ("syslib::DefaultPlatform::GetWindowTitle");
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t, const Rect&)
{
  raise ("syslib::DefaultPlatform::SetWindowRect");
}

void Platform::SetClientRect (window_t, const Rect&)
{
  raise ("syslib::DefaultPlatform::SetClientRect");
}

void Platform::GetWindowRect (window_t, Rect&)
{
  raise ("syslib::DefaultPlatform::GetWindowRect");
}

void Platform::GetClientRect (window_t, Rect&)
{
  raise ("syslib::DefaultPlatform::GetClientRect");
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t, WindowFlag, bool)
{
  raise ("syslib::DefaultPlatform::SetWindowFlag");
}

bool Platform::GetWindowFlag (window_t, WindowFlag)
{
  raise ("syslib::DefaultPlatform::GetWindowFlag");

  return false;
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  raise ("syslib::DefaultPlatform::SetParentWindow");
}

const void* Platform::GetParentWindowHandle (window_t child)
{
  raise ("syslib::DefaultPlatform::GetParentWindow");

  return 0;
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t)
{
  raise ("syslib::DefaultPlatform::InvalidateWindow");
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point&)
{
  raise ("syslib::DefaultPlatform::SetCursorPosition");
}

Point Platform::GetCursorPosition ()
{
  raise ("syslib::DefaultPlatform::GetCursorPosition");

  return Point ();
}

void Platform::SetCursorPosition (window_t, const Point& client_position)
{
  throw xtl::make_not_implemented_exception ("syslib::Platform::SetCursorPosition (window_t, const Point&)");
}

Point Platform::GetCursorPosition (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::Platform::GetCursorPosition (window_t)");
}

/*
    Видимость курсора
*/

void Platform::SetCursorVisible (window_t, bool state)
{
  raise ("syslib::DefaultPlatform::SetCursorVisible");
}

bool Platform::GetCursorVisible (window_t)
{
  raise ("syslib::DefaultPlatform::GetCursorVisible");

  return false;
}

/*
    Изображение курсора
*/

Platform::cursor_t Platform::CreateCursor (const char*, int, int)
{
  raise ("syslib::DefaultPlatform::CreateCursor");
}

void Platform::DestroyCursor (cursor_t)
{
  raise ("syslib::DefaultPlatform::DestroyCursor");
}

void Platform::SetCursor (window_t, cursor_t)
{
  raise ("syslib::DefaultPlatform::SetCursor");
}

/*
    Цвет фона
*/

void Platform::SetBackgroundColor (window_t window, const Color& color)
{
  raise ("syslib::SetBackgroundColor");
}

void Platform::SetBackgroundState (window_t window, bool state)
{
  raise ("syslib::SetBackgroundState");
}

Color Platform::GetBackgroundColor (window_t window)
{
  raise ("syslib::GetBackgroundColor");

  return Color (0, 0, 0);
}

bool Platform::GetBackgroundState (window_t window)
{
  raise ("syslib::GetBackgroundState");
  
  return false;
}
