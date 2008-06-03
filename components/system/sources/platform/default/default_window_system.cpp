#include <platform/platform.h>
#include <common/exception.h>

using namespace syslib;

/*
    Генерация исключения: работа с окнами невозможна для платформы по умолчанию
*/

namespace
{

void raise (const char* method_name)
{
  common::raise_not_supported (method_name, "No window support for default platform");
}

}

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle, WindowMessageHandler, void*)
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
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t, const char*)
{
  raise ("syslib::DefaultPlatform::SetWindowTitle");
}

void Platform::SetWindowTitle (window_t, const wchar_t*)
{
  raise ("syslib::DefaultPlatform::SetWindowTitle");
}

void Platform::GetWindowTitle (window_t, size_t, char*)
{
  raise ("syslib::DefaultPlatform::GetWindowTitle");
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
