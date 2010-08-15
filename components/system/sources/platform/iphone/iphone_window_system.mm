#include "shared.h"

#import <syslib/platform/iphone.h>

#import <UIKit/UIApplication.h>
#import <UIKit/UIScreen.h>
#import <UIKit/UITouch.h>
#import <UIKit/UIViewController.h>
#import <UIKit/UIWindow.h>

#import <QuartzCore/CAEAGLLayer.h>

using namespace syslib;
using namespace syslib::iphone;

namespace
{

const size_t DEFAULT_TOUCH_BUFFER_SIZE = 4;  //размер буфера для хранения информации о касаниях

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

typedef stl::vector <IWindowListener*>                ListenerArray;
typedef xtl::uninitialized_storage <TouchDescription> TouchDescriptionArray;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Распределитель событий окна
///////////////////////////////////////////////////////////////////////////////////////////////////
@interface UIWindowWrapper : UIWindow
{
  @private
    WindowImpl            *window_impl;          //окно
    ListenerArray         *listeners;            //подписчика на события
    TouchDescriptionArray *touch_descriptions;   //массив для хранения описаний текущего события
    WindowEventContext    *event_context;        //контекст, передаваемый обработчикам событий
    UIViewController      *root_view_controller; //корневой контроллер
}

@property (nonatomic) WindowImpl* window_impl;

-(void)onPaint;

-(WindowEventContext&)getEventContext;

@end

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс, обрабатывающий события ввода и отвечающий за отрисовку
///////////////////////////////////////////////////////////////////////////////////////////////////
@interface UIViewWrapper : UIView
{
}

@end

@implementation UIViewWrapper

+(Class)layerClass
{
  return [CAEAGLLayer class];
}

-(BOOL)canBecomeFirstResponder
{
  return YES;
}

-(id)initWithFrame:(CGRect)rect
{
  self = [super initWithFrame:rect];

  if (!self)
    return nil;

  self.layer.delegate = self;

  return self;
}

-(void) drawRect:(CGRect)rect
{
  [(UIWindowWrapper*)self.window onPaint];
}

-(void) displayLayer:(CALayer*)layer
{
  [(UIWindowWrapper*)self.window onPaint];
}

-(void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
  [(UIWindowWrapper*)self.window touchesBegan:touches withEvent:event];
}

-(void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
  [(UIWindowWrapper*)self.window touchesEnded:touches withEvent:event];
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
  [(UIWindowWrapper*)self.window touchesMoved:touches withEvent:event];
}

-(void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
  [(UIWindowWrapper*)self.window touchesCancelled:touches withEvent:event];
}

-(void) motionBegan:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  [(UIWindowWrapper*)self.window motionBegan:motion withEvent:event];
}

-(void) motionEnded:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  [(UIWindowWrapper*)self.window motionEnded:motion withEvent:event];
}

-(void) motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  [(UIWindowWrapper*)self.window motionCancelled:motion withEvent:event];
}

@end

///////////////////////////////////////////////////////////////////////////////////////////////////
///Класс, отвечающий за управление ориентацией окна
///////////////////////////////////////////////////////////////////////////////////////////////////
@interface UIViewControllerWrapper : UIViewController
{
}

@end

@implementation UIViewControllerWrapper

-(void)dealloc
{
  self.view = nil;

  [super dealloc];
}

-(void)loadView
{
  self.view = [[UIViewWrapper alloc] initWithFrame:[UIScreen mainScreen].applicationFrame];
  [self.view release];
}

-(void)viewDidUnload
{
  [super viewDidUnload];

  self.view = nil;
}

@end

@implementation UIWindowWrapper

@synthesize window_impl;

-(void) dealloc
{
  delete touch_descriptions;
  delete listeners;
  delete event_context;

  [super dealloc];
}

-(UIViewController*)rootViewController
{
  return root_view_controller;
}

-(void)setRootViewController:(UIViewController*)in_root_view_controller
{
  if (root_view_controller == in_root_view_controller)
    return;

  [root_view_controller.view removeFromSuperview];

  [root_view_controller release];
  root_view_controller = [in_root_view_controller retain];

  [self addSubview:root_view_controller.view];

  [root_view_controller.view becomeFirstResponder];
}

-(id) initWithFrame:(CGRect)rect
{
  self = [super initWithFrame:rect];

  if (!self)
    return nil;

  try
  {
    event_context = new WindowEventContext;

    event_context->handle = self;

    listeners          = new ListenerArray;
    touch_descriptions = new TouchDescriptionArray (DEFAULT_TOUCH_BUFFER_SIZE);
  }
  catch (...)
  {
    delete event_context;
    delete listeners;
    delete touch_descriptions;

    [self release];

    return nil;
  }

  self.rootViewController = [[UIViewControllerWrapper alloc] init];
  [self.rootViewController release];

  return self;
}

-(WindowEventContext&) getEventContext
{
  return *event_context;
}

-(void) onPaint
{
  window_impl->Notify (WindowEvent_OnPaint, [self getEventContext]);
}

-(void) fillTouchDescriptionsBuffer:(NSSet*)touches
{
  CGSize window_size = self.frame.size;

  if ([touches count] > touch_descriptions->size ())
    touch_descriptions->resize ([touches count], false);

  NSEnumerator     *enumerator        = [touches objectEnumerator];
  TouchDescription *touch_description = touch_descriptions->data ();

  for (UITouch *iter = [enumerator nextObject]; iter; iter = [enumerator nextObject], touch_description++)
  {
    CGPoint current_location  = [iter locationInView:self];

    touch_description->touch     = (touch_t)iter;
    touch_description->x         = current_location.x / window_size.width;
    touch_description->y         = current_location.y / window_size.height;
    touch_description->tap_count = iter.tapCount;
  }
}

-(void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self fillTouchDescriptionsBuffer:touches];

  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnTouchesBegan ([touches count], touch_descriptions->data ());
}

-(void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self fillTouchDescriptionsBuffer:touches];

  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnTouchesEnded ([touches count], touch_descriptions->data ());
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self fillTouchDescriptionsBuffer:touches];

  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnTouchesMoved ([touches count], touch_descriptions->data ());
}

-(void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self touchesEnded:touches withEvent:event];
}

-(void) motionBegan:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  if (motion != UIEventSubtypeMotionShake)
    return;

  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnShakeMotionBegan ();
}

-(void) motionEnded:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  if (motion != UIEventSubtypeMotionShake)
    return;

  for (ListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnShakeMotionEnded ();
}

-(void) motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  [self motionEnded:motion withEvent:event];
}

/*
   Добавление/удаление подписчиков
*/

-(void) attachListener:(IWindowListener*)listener
{
  listeners->push_back (listener);
}

-(void) detachListener:(IWindowListener*)listener
{
  listeners->erase (stl::remove (listeners->begin (), listeners->end (), listener), listeners->end ());
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
  UIWindowWrapper* new_window = [[UIWindowWrapper alloc] initWithFrame:[UIScreen mainScreen].applicationFrame];

  if (!new_window)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't create window.");

  new_window.rootViewController.view.clearsContextBeforeDrawing = NO;
  new_window.rootViewController.view.multipleTouchEnabled       = YES;

  WindowImpl* window_impl = new WindowImpl (handler, user_data, new_window);

  new_window.window_impl = window_impl;

  return (window_t)new_window;
}

void Platform::CloseWindow (window_t handle)
{
  WindowImpl* window = ((UIWindowWrapper*)handle).window_impl;

  window->Notify (WindowEvent_OnClose, [(UIWindowWrapper*)handle getEventContext]);
}

void Platform::DestroyWindow (window_t handle)
{
  WindowImpl* window = ((UIWindowWrapper*)handle).window_impl;

  window->Notify (WindowEvent_OnDestroy, [(UIWindowWrapper*)handle getEventContext]);

  delete window;
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* Platform::GetNativeWindowHandle (window_t handle)
{
  return reinterpret_cast<const void*> (handle);
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t, const wchar_t*)
{
}

void Platform::GetWindowTitle (window_t, size_t size, wchar_t* buffer)
{
  if (!size)
    return;

  if (!buffer)
    throw xtl::make_null_argument_exception ("syslib::iPhonePlatform::GetWindowTitle", "buffer");

  *buffer = L'\0';
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

  WindowEventContext& dummy_context = [(UIWindowWrapper*)handle getEventContext];

  window->Notify (WindowEvent_OnMove, dummy_context);
  window->Notify (WindowEvent_OnSize, dummy_context);
}

void Platform::SetClientRect (window_t handle, const Rect& rect)
{
  SetWindowRect (handle, rect);
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

  WindowEventContext& dummy_context = [(UIWindowWrapper*)handle getEventContext];

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
        if (state)
        {
          [wnd makeKeyAndVisible];
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
          [wnd makeKeyAndVisible];

          window->Notify (WindowEvent_OnActivate, dummy_context);
        }
        else
          throw xtl::format_operation_exception ("", "Can't make window inactive");

        break;
      case WindowFlag_Focus: //фокус ввода
        if (state)
        {
          wnd.rootViewController.view.userInteractionEnabled = YES;

          window->Notify (WindowEvent_OnSetFocus, dummy_context);
        }
        else
        {
          wnd.rootViewController.view.userInteractionEnabled = NO;

          window->Notify (WindowEvent_OnLostFocus, dummy_context);
        }

        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
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
        return wnd.rootViewController.view.userInteractionEnabled == YES;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
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
  [((UIWindowWrapper*)handle).rootViewController.view setNeedsDisplay];
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
  if (state)
    throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::SetCursorPosition", "No cursor for iPhone platform");
}

bool Platform::GetCursorVisible (window_t)
{
  return false;
}

/*
    Изображение курсора
*/

Platform::cursor_t Platform::CreateCursor (const char*, int, int)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::CreateCursor", "No cursor for iPhone platform");
}

void Platform::DestroyCursor (cursor_t)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::DestroyCursor", "No cursor for iPhone platform");
}

void Platform::SetCursor (window_t, cursor_t)
{
  throw xtl::format_not_supported_exception ("syslib::iPhonePlatform::CreateCursor", "No cursor for iPhone platform");
}

namespace syslib
{

namespace iphone
{

/*
   Добавление/удаление подписчиков
*/

void attach_window_listener (const Window& window, IWindowListener* listener)
{
  if  (!listener)
    return;

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception ("syslib::iphone::attach_window_listener", "Can't attach window listener before entering run loop");

  [(UIWindowWrapper*)(window.Handle ()) attachListener:listener];
}

void detach_window_listener (const Window& window, IWindowListener* listener)
{
  if (!is_in_run_loop ())
    return;

  [(UIWindowWrapper*)(window.Handle ()) detachListener:listener];
}

/*
   Установка multitouch режима для окна
*/

void set_multitouch_enabled (const Window& window, bool enabled)
{
  ((UIWindowWrapper*)window.Handle ()).rootViewController.view.multipleTouchEnabled = enabled;
}

bool get_multitouch_enabled (const Window& window)
{
  return ((UIWindowWrapper*)window.Handle ()).rootViewController.view.multipleTouchEnabled;
}

}

}
