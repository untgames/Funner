#include "shared.h"

using namespace syslib;

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle style, WindowMessageHandler handler, window_t parent, void* user_data)
{
  return 0;
}

void Platform::CloseWindow (window_t handle)
{
}

void Platform::DestroyWindow (window_t handle)
{
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t handle, const wchar_t* title)
{
}

void Platform::GetWindowTitle (window_t handle, size_t buffer_size_in_chars, wchar_t* buffer)
{
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t handle, const Rect& rect)
{
}

void Platform::GetWindowRect (window_t handle, Rect& rect)
{
}

void Platform::GetClientRect (window_t handle, Rect& rect)
{
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
}

bool Platform::GetWindowFlag (window_t handle, WindowFlag flag)
{
  return false;
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindow (window_t child, window_t parent)
{
}

Platform::window_t Platform::GetParentWindow (window_t child)
{
  return 0;
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t handle)
{
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point& position)
{
}

syslib::Point Platform::GetCursorPosition ()
{
  return syslib::Point (0, 0);
}

void Platform::SetCursorPosition (window_t handle, const Point& client_position)
{
}

syslib::Point Platform::GetCursorPosition (window_t handle)
{
  return syslib::Point (0, 0);
}

/*
    Видимость курсора
*/

void Platform::SetCursorVisible (window_t, bool state)
{
}

bool Platform::GetCursorVisible (window_t)
{
  return true;
}
