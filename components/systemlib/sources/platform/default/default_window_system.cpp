#include <platform/platform.h>
#include <common/exception.h>

using namespace syslib;

/*
    Генерация исключения: работа с окнами невозможна для платформы по умолчанию
*/

namespace
{

void Raise (const char* method_name)
{
  common::RaiseNotSupported (method_name, "No window support for default platform");
}

}

/*
    Создание/закрытие/уничтожение окна
*/

DefaultPlatform::window_t DefaultPlatform::CreateWindow (WindowStyle, WindowMessageHandler, void*)
{
  Raise ("syslib::DefaultPlatform::CreateWindow");
  
  return 0;
}

void DefaultPlatform::CloseWindow (window_t)
{
  Raise ("syslib::DefaultPlatform::CloseWindow");
}

void DefaultPlatform::DestroyWindow (window_t)
{
  Raise ("syslib::DefaultPlatform::DestroyWindow");
}
    
/*
    Заголовок окна
*/

void DefaultPlatform::SetWindowTitle (window_t, const char*)
{
  Raise ("syslib::DefaultPlatform::SetWindowTitle");
}

void DefaultPlatform::GetWindowTitle (window_t, size_t, char*)
{
  Raise ("syslib::DefaultPlatform::GetWindowTitle");
}

/*
    Область окна / клиентская область
*/

void DefaultPlatform::SetWindowRect (window_t, const Rect&)
{
  Raise ("syslib::DefaultPlatform::SetWindowRect");
}

void DefaultPlatform::GetWindowRect (window_t, Rect&)
{
  Raise ("syslib::DefaultPlatform::GetWindowRect");  
}

void DefaultPlatform::GetClientRect (window_t, Rect&)
{
  Raise ("syslib::DefaultPlatform::GetClientRect");
}

/*
    Установка флагов окна
*/

void DefaultPlatform::SetWindowFlag (window_t, WindowFlag, bool)
{
  Raise ("syslib::DefaultPlatform::SetWindowFlag");
}

bool DefaultPlatform::GetWindowFlag (window_t, WindowFlag)
{
  Raise ("syslib::DefaultPlatform::GetWindowFlag");
  
  return false;
}

/*
    Положение курсора
*/

void DefaultPlatform::SetCursorPosition (const Point&)
{
  Raise ("syslib::DefaultPlatform::SetCursorPosition");
}

Point DefaultPlatform::GetCursorPosition ()
{
  Raise ("syslib::DefaultPlatform::GetCursorPosition");
  
  return Point ();
}
