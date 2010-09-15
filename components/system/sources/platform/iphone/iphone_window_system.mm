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

/*
   Класс, обрабатывающий события ввода и отвечающий за отрисовку
*/

@interface UIViewWrapper : UIView
{
  @private
    Color   background_color;       //цвет фона
    UIColor *ui_background_color;   //цвет фона
    bool    background_state;       //состояние включения цвета фона
}

@property (nonatomic, assign) Color    background_color;
@property (nonatomic, retain) UIColor* ui_background_color;
@property (nonatomic, assign) bool     background_state;

@end

@implementation UIViewWrapper

@synthesize background_color, ui_background_color, background_state;

-(void)dealloc
{
  self.ui_background_color = nil;

  [super dealloc];
}

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

  if ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)
    self.view.contentScaleFactor = [UIScreen mainScreen].scale;
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

-(CGFloat)contentScaleFactor
{
  if ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)
    return super.contentScaleFactor;

  return 1;
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

  if ([[[UIDevice currentDevice] systemVersion] compare:@"4.0" options:NSNumericSearch] != NSOrderedAscending)
    new_window.contentScaleFactor = [UIScreen mainScreen].scale;

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

void Platform::SetWindowRect (window_t window, const Rect& rect)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::iPhonePlatform::SetWindowRect", "window");

  UIWindowWrapper* wnd = (UIWindowWrapper*)window;

  float scale_factor = wnd.contentScaleFactor;

  CGRect frame;

  frame.size.width  = (rect.right - rect.left) / scale_factor;
  frame.size.height = (rect.bottom - rect.top) / scale_factor;
  frame.origin.x    = rect.left / scale_factor;
  frame.origin.y    = rect.top / scale_factor;

  wnd.frame = frame;

  WindowImpl* window_impl = wnd.window_impl;

  WindowEventContext& dummy_context = [wnd getEventContext];

  window_impl->Notify (WindowEvent_OnMove, dummy_context);
  window_impl->Notify (WindowEvent_OnSize, dummy_context);
}

void Platform::SetClientRect (window_t handle, const Rect& rect)
{
  SetWindowRect (handle, rect);
}

void Platform::GetWindowRect (window_t window, Rect& rect)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::iPhonePlatform::GetWindowRect", "window");

  UIWindow* wnd = (UIWindow*)window;

  float scale_factor = wnd.contentScaleFactor;

  CGRect frame = wnd.frame;

  rect.bottom = (frame.origin.y + frame.size.height) * scale_factor;
  rect.right  = (frame.origin.x + frame.size.width) * scale_factor;
  rect.top    = frame.origin.y * scale_factor;
  rect.left   = frame.origin.x * scale_factor;
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
      case WindowFlag_Maximized:
        wnd.frame = [UIScreen mainScreen].applicationFrame;
        break;
      case WindowFlag_Minimized:
        throw xtl::format_operation_exception ("", "Can't minimize window");
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
      case WindowFlag_Maximized:
        return CGRectEqualToRect (wnd.frame, [UIScreen mainScreen].applicationFrame);
      case WindowFlag_Minimized:
        throw xtl::format_operation_exception ("", "Can't get window flag %d value", flag);
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

/*
   Цвет фона
*/

void Platform::SetBackgroundColor (window_t window, const Color& color)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::iPhonePlatform::SetBackgroundColor", "window");

  UIWindow*      wnd  = (UIWindow*)window;
  UIViewWrapper* view = (UIViewWrapper*)wnd.rootViewController.view;

  UIColor *background_color = [[UIColor alloc] initWithRed:color.red / 255.f green:color.green / 255.f blue:color.blue / 255.f alpha:1];

  view.background_color    = color;
  view.ui_background_color = background_color;

  if (view.background_state)
    view.backgroundColor = background_color;

  [background_color release];
}

void Platform::SetBackgroundState (window_t window, bool state)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::iPhonePlatform::SetBackgroundState", "window");

  UIWindow*      wnd  = (UIWindow*)window;
  UIViewWrapper* view = (UIViewWrapper*)wnd.rootViewController.view;

  view.clearsContextBeforeDrawing = state;
  view.backgroundColor            = state ? view.ui_background_color : nil;
  view.background_state           = state;
}

Color Platform::GetBackgroundColor (window_t window)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::iPhonePlatform::GetBackgroundColor", "window");

  UIWindow*      wnd  = (UIWindow*)window;
  UIViewWrapper *view = (UIViewWrapper*)wnd.rootViewController.view;

  return view.ui_background_color ? view.background_color : Color ();
}

bool Platform::GetBackgroundState (window_t window)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::iPhonePlatform::GetBackgroundState", "window");

  return ((UIViewWrapper*)((UIWindow*)window).rootViewController.view).background_state;
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
