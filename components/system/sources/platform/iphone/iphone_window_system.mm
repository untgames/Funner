#include "shared.h"

#include <UIScreen.h>
#include <UIWindow.h>

using namespace syslib;

@interface UIWindowWrapper : UIWindow
{
}

-(void)Close
{

}

-(void)Destroy
{

}

@end

@implementation UIWindowWrapper
@end

//Конструктор/деструктор
WindowImpl::WindowImpl (Platform::WindowMessageHandler handler, void* in_user_data, void* new_window)
  : user_data (in_user_data), message_handler (handler), cocoa_window ((Platform::window_t)new_window)
  {}

WindowImpl::~WindowImpl ()
{
  [(UIWindow*)cocoa_window dealloc];
  IPhoneApplication::SetWindow (0);
}

//Посылка сообщений
void WindowImpl::Notify (WindowEvent event, const WindowEventContext& context)
{
  try
  {
    message_handler (cocoa_window, event, context, user_data);
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle window_style, WindowMessageHandler handler, window_t parent, void* user_data)
{
  static const char* METHOD_NAME = "syslib::iPhonePlatform::CreateWindow";

  if (parent)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Parent windows not supported for iPhonePlatform");

  if (IPhoneApplication::GetWindow ())
    throw xtl::format_operation_exception (METHOD_NAME, "One window already created, can't create second window.");

    //Создание окна
  UIWindowWrapper* new_window = [[UIWindowWrapper alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

  if (!new_window)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't create window.");

  new_window.multipleTouchEnabled = YES;

  new WindowImpl (handler, user_data, new_window);

  return (window_t)new_window;
}

void Platform::CloseWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::CloseWindow");
}

void Platform::DestroyWindow (window_t)
{
  throw xtl::make_not_implemented_exception ("syslib::iPhonePlatform::DestroyWindow");
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t, const wchar_t*)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::SetWindowTitle", "No window title for iPhone platform");
}

void Platform::GetWindowTitle (window_t, size_t, wchar_t*)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::GetWindowTitle", "No window title for iPhone platform");
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t, const Rect&)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::SetWindowRect", "Window rect changing not supported for iPhone platform");
}

void Platform::GetWindowRect (window_t handle, Rect& rect)
{
  CGRect bounds = ((UIView*)handle).bounds;

  rect.bottom = bounds.size.height;
  rect.right  = bounds.size.width;
  rect.top    = 0;
  rect.left   = 0;
}

void Platform::GetClientRect (window_t handle, Rect& target_rect)
{
  GetWindowRect (handle, target_rect);
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t, WindowFlag, bool)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::SetWindowRect", "Window changing not supported for iPhone platform");
}

bool Platform::GetWindowFlag (window_t handle, WindowFlag)
{
  if ((UIWindow*)handle != [UIApplication sharedApplication].keyWindow)
    return false;

  return true;
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindow (window_t, window_t)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::SetParentWindow", "Parent windows not supported for iPhone platform");
}

Platform::window_t Platform::GetParentWindow (window_t child)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::GetParentWindow", "Parent windows not supported for iPhone platform");
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t handle)
{
  [(UIView*)handle setNeedsDisplay];
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point&)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::SetCursorPosition", "No cursor for iPhone platform");
}

syslib::Point Platform::GetCursorPosition ()
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::GetCursorPosition", "No cursor for iPhone platform");
}

void Platform::SetCursorPosition (window_t, const Point& client_position)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::SetCursorPosition", "No cursor for iPhone platform");
}

syslib::Point Platform::GetCursorPosition (window_t)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::GetCursorPosition", "No cursor for iPhone platform");
}

/*
    Видимость курсора
*/

void Platform::SetCursorVisible (window_t, bool state)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::SetCursorPosition", "No cursor for iPhone platform");
}

bool Platform::GetCursorVisible (window_t)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::GetCursorPosition", "No cursor for iPhone platform");
}
