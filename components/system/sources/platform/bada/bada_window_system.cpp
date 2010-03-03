#include "shared.h"

using namespace syslib;

namespace
{

const char* LOG_NAME = "syslib.Platform";

/*
    Описание реализации окна
*/

struct WindowImpl
{
  void Notify (Platform::window_t window, WindowEvent event, const WindowEventContext& context)
  {
/*    try
    {
      message_handler (window, event, context, user_data);
    }
    catch (...)
    {
      //подавление всех исключений
    }*/
  }
};

}


/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle style, WindowMessageHandler handler, window_t parent, void* user_data)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::CreateWindow");
}

void Platform::CloseWindow (window_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::CloseWindow");
}

void Platform::DestroyWindow (window_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::DestroyWindow");
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t handle, const wchar_t* title)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::SetWindowTitle");
}

void Platform::GetWindowTitle (window_t handle, size_t buffer_size_in_chars, wchar_t* buffer)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::GetWindowTitle");
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t handle, const Rect& rect)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::SetWindowRect");
}

void Platform::SetClientRect (window_t handle, const Rect& rect)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::SetClientRect");
}

void Platform::GetWindowRect (window_t handle, Rect& rect)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::GetWindowRect");
}

void Platform::GetClientRect (window_t handle, Rect& rect)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::GetClientRect");
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::SetWindowFlag");
}

bool Platform::GetWindowFlag (window_t handle, WindowFlag flag)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::GetWindowFlag");
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindow (window_t child, window_t parent)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::SetParentWindow");
}

Platform::window_t Platform::GetParentWindow (window_t child)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::GetParentWindow");
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::InvalidateWindow");
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point& position)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::SetCursorPosition(const Point&)");
}

syslib::Point Platform::GetCursorPosition ()
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::GetCursorPosition()");
}

void Platform::SetCursorPosition (window_t handle, const Point& client_position)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::SetCursorPosition(window_t,const Point&)");
}

syslib::Point Platform::GetCursorPosition (window_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::GetCursorPosition(window_t)");
}

/*
    Видимость курсора
*/

void Platform::SetCursorVisible (window_t handle, bool state)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::SetCursorVisible");
}

bool Platform::GetCursorVisible (window_t handle)
{
  throw xtl::make_not_implemented_exception ("syslib::BadaPlatform::GetCursorVisible");
}
