#include "shared.h"

#import <syslib/platform/iphone.h>

#import <UIScreen.h>
#import <UIWindow.h>

#import <UIApplication.h>

using namespace syslib;

namespace
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации окна
///////////////////////////////////////////////////////////////////////////////////////////////////
struct WindowImpl
{
  void*                          user_data;       //указатель на пользовательские данные
  Platform::WindowMessageHandler message_handler; //функция обработки сообщений окна
  Platform::window_t             cocoa_window;    //окно

  //Конструктор/деструктор
  WindowImpl (Platform::WindowMessageHandler handler, void* in_user_data, void* new_window)
    : user_data (in_user_data), message_handler (handler), cocoa_window ((Platform::window_t)new_window)
    {}

  ~WindowImpl ()
  {
    [(UIWindow*)cocoa_window release];
  }

  //Посылка сообщений
  void Notify (WindowEvent event, const WindowEventContext& context)
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
};

}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Распределитель событий окна
///////////////////////////////////////////////////////////////////////////////////////////////////
@interface UIWindowWrapper : UIWindow <WindowEventProvider>
{
  @private
    WindowImpl* window_impl;
    bool        destroyed;
}

@property (nonatomic, readwrite) WindowImpl* window_impl;
@property (nonatomic, readwrite) bool        destroyed;

-(id) initWithFrame:(CGRect)rect;
-(void) drawRect:(CGRect)rect;

@end

@implementation UIWindowWrapper

@synthesize window_impl;
@synthesize destroyed;

-(id) initWithFrame:(CGRect)rect
{
  self = [super initWithFrame:rect];

  if (self)
  {
    window_impl = 0;
    destroyed   = false;
  }

  return self;
}

-(void) drawRect:(CGRect)rect
{
  WindowEventContext dummy_context;

  window_impl->Notify (WindowEvent_OnPaint, dummy_context);
}

-(void) touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{

}

-(void) touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{

}

-(void) touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{

}

-(void) touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{

}

-(void) motionBegan:(UIEventSubtype)motion withEvent:(UIEvent *)event
{

}

-(void) motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent *)event
{

}

-(void) motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event
{

}

/*
   Добавление/удаление подписчиков (подписчик не захватывается)
*/

-(void) addEventHandler:(id <WindowEventHandler>)handler
{

}

-(void) removeEventHandler:(id <WindowEventHandler>)handler
{

}

@end

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle window_style, WindowMessageHandler handler, window_t parent, void* user_data)
{
  static const char* METHOD_NAME = "syslib::iPhonePlatform::CreateWindow";

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't create window before entering run loop");

  if (parent)
    throw xtl::format_not_supported_exception (METHOD_NAME, "Parent windows not supported for iPhonePlatform");

    //Создание окна
  UIWindowWrapper* new_window = [[UIWindowWrapper alloc] initWithFrame:[[UIScreen mainScreen] bounds]];

  if (!new_window)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't create window.");

  new_window.clearsContextBeforeDrawing = NO;

  WindowImpl* window_impl = new WindowImpl (handler, user_data, new_window);

  new_window.window_impl = window_impl;

  return (window_t)new_window;
}

void Platform::CloseWindow (window_t handle)
{
  WindowImpl* window = ((UIWindowWrapper*)handle).window_impl;

  WindowEventContext dummy_context;

  window->Notify (WindowEvent_OnClose, dummy_context);
}

void Platform::DestroyWindow (window_t handle)
{
  ((UIWindowWrapper*)handle).destroyed = true;

  WindowImpl* window = ((UIWindowWrapper*)handle).window_impl;

  WindowEventContext dummy_context;

  window->Notify (WindowEvent_OnDestroy, dummy_context);

  delete window;
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

void Platform::SetWindowRect (window_t handle, const Rect& rect)
{
  CGRect frame;

  frame.size.width  = rect.right - rect.left;
  frame.size.height = rect.bottom - rect.top;
  frame.origin.x    = rect.left;
  frame.origin.y    = rect.top;

  ((UIView*)handle).frame = frame;

  WindowImpl* window = ((UIWindowWrapper*)handle).window_impl;

  WindowEventContext dummy_context;

  window->Notify (WindowEvent_OnMove, dummy_context);
  window->Notify (WindowEvent_OnSize, dummy_context);
}

void Platform::GetWindowRect (window_t handle, Rect& rect)
{
  CGRect frame = ((UIView*)handle).frame;

  rect.bottom = frame.origin.y + frame.size.height;
  rect.right  = frame.origin.x + frame.size.width;
  rect.top    = frame.origin.y;
  rect.left   = frame.origin.x;
}

void Platform::GetClientRect (window_t handle, Rect& target_rect)
{
  GetWindowRect (handle, target_rect);
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  if (state == GetWindowFlag (handle, flag))
    return;

  UIWindowWrapper* wnd = (UIWindowWrapper*)handle;

  WindowImpl* window = wnd.window_impl;

  WindowEventContext dummy_context;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
        if (state)
        {
          wnd.hidden = NO;

          window->Notify (WindowEvent_OnShow, dummy_context);
        }
        else
        {
          wnd.hidden = YES;

          window->Notify (WindowEvent_OnHide, dummy_context);
        }

        break;
      case WindowFlag_Active: //активность окна
        if (state)
        {
          [wnd makeKeyWindow];

          window->Notify (WindowEvent_OnActivate, dummy_context);
        }
        else
          throw xtl::format_operation_exception ("", "Can't make window inactive");

        break;
      case WindowFlag_Focus: //фокус ввода
        if (state)
        {
          wnd.userInteractionEnabled = YES;

          window->Notify (WindowEvent_OnSetFocus, dummy_context);
        }
        else
        {
          wnd.userInteractionEnabled = NO;

          window->Notify (WindowEvent_OnLostFocus, dummy_context);
        }

        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
        break;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::iPhonePlatform::SetWindowFlag");
    throw;
  }
}

bool Platform::GetWindowFlag (window_t handle, WindowFlag flag)
{
  UIWindow* wnd = (UIWindow*)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
        return wnd.hidden == NO;
      case WindowFlag_Active:
        return wnd == [UIApplication sharedApplication].keyWindow;
      case WindowFlag_Focus:
        return wnd.userInteractionEnabled == YES;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
        break;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::iPhonePlatform::GetWindowFlag");
    throw;
  }
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
