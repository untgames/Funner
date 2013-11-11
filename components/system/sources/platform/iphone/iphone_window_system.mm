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

const char* LOG_NAME = "syslib.iphone.window";

InterfaceOrientation get_interface_orientation (UIInterfaceOrientation interface_orientation)
{
  switch (interface_orientation)
  {
    case UIInterfaceOrientationPortrait:           return InterfaceOrientation_Portrait;
    case UIInterfaceOrientationPortraitUpsideDown: return InterfaceOrientation_PortraitUpsideDown;
    case UIInterfaceOrientationLandscapeLeft:      return InterfaceOrientation_LandscapeLeft;
    case UIInterfaceOrientationLandscapeRight:     return InterfaceOrientation_LandscapeRight;
    default:                                       return InterfaceOrientation_Unknown;
  }
}

CGRect get_transformed_view_rect_size (CGRect frame, UIView* view)
{
  if (!CGAffineTransformIsIdentity (view.transform))
  {
    float temp = frame.size.width;

    frame.size.width  = frame.size.height;
    frame.size.height = temp;

    temp = frame.origin.x;

    frame.origin.x = frame.origin.y;
    frame.origin.y = temp;
  }

  return frame;
}

}

namespace syslib
{

namespace iphone
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание реализации окна
///////////////////////////////////////////////////////////////////////////////////////////////////
struct WindowImpl
{
  void*                user_data;       //указатель на пользовательские данные
  WindowMessageHandler message_handler; //функция обработки сообщений окна
  window_t             cocoa_window;    //окно
  common::Log          log;             //поток протоколирования

  //Конструктор/деструктор
  WindowImpl (WindowMessageHandler handler, void* in_user_data, void* new_window)
    : user_data (in_user_data)
    , message_handler (handler)
    , cocoa_window ((window_t)new_window)
    , log (LOG_NAME)
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
    catch (std::exception& e)
    {
      log.Printf ("%s\n    at syslib::iphone::WindowImpl::Notify", e.what ());
    }
    catch (...)
    {
      log.Printf ("unknown exception\n    at syslib::iphone::WindowImpl::Notify");
    }
  }
};

}

}

@interface UIWindowWrapper ()

@property (nonatomic, assign) syslib::iphone::WindowImpl* window_impl;
@property (nonatomic)         int                         allowed_orientations;

-(void)onPaint;

-(WindowEventContext&)getEventContext;

-(void)onInterfaceOrientationWillChangeFrom:(InterfaceOrientation)from_orientation to:(InterfaceOrientation)to_orientation duration:(float)duration;
-(void)onInterfaceOrientationChangedFrom:(InterfaceOrientation)from_orientation to:(InterfaceOrientation)to_orientation;

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

@end

/*
   Класс, отвечающий за управление ориентацией окна
*/

@interface UIViewControllerWrapper : UIViewController
{
  @private
    UIWindowWrapper* window;
}

@end

@implementation UIViewControllerWrapper

-(void)dealloc
{
  self.view = nil;

  [super dealloc];
}

-(id)initWithWindow:(UIWindowWrapper*)in_window
{
  self = [super init];

  if (!self)
    return nil;

  window = in_window;

  return self;
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

-(void)viewWillDisappear:(BOOL)animated
{
  [super viewWillDisappear:animated];

  ApplicationDelegate* delegate = [UIApplication sharedApplication].delegate;

  if ([delegate isKindOfClass:[ApplicationDelegate class]])
    [delegate setMainViewVisible:false];
}

-(void)viewWillAppear:(BOOL)animated
{
  [super viewWillAppear:animated];

  ApplicationDelegate* delegate = [UIApplication sharedApplication].delegate;

  if ([delegate isKindOfClass:[ApplicationDelegate class]])
    [delegate setMainViewVisible:true];
}

-(BOOL)shouldAutorotate
{
  return YES;
}

-(NSUInteger)supportedInterfaceOrientations
{
  NSUInteger return_value = 0;

  if (window.allowed_orientations & InterfaceOrientation_Portrait)
    return_value |= UIInterfaceOrientationMaskPortrait;
  if (window.allowed_orientations & InterfaceOrientation_PortraitUpsideDown)
    return_value |= UIInterfaceOrientationMaskPortraitUpsideDown;
  if (window.allowed_orientations & InterfaceOrientation_LandscapeLeft)
    return_value |= UIInterfaceOrientationMaskLandscapeLeft;
  if (window.allowed_orientations & InterfaceOrientation_LandscapeRight)
    return_value |= UIInterfaceOrientationMaskLandscapeRight;

  if (!return_value)
    return_value = UIInterfaceOrientationMaskAll;

  return return_value;
}

-(BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interface_orientation
{
  InterfaceOrientation desired_orientation = get_interface_orientation (interface_orientation);

  return window.allowed_orientations & desired_orientation;
}

-(void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)from_orientation
{
  [window onInterfaceOrientationChangedFrom:get_interface_orientation (from_orientation) to:get_interface_orientation (self.interfaceOrientation)];
}

-(void)willRotateToInterfaceOrientation:(UIInterfaceOrientation)to_orientation duration:(NSTimeInterval)duration
{
  [window onInterfaceOrientationWillChangeFrom:get_interface_orientation (self.interfaceOrientation) to:get_interface_orientation (to_orientation) duration:duration];
}

@end

@implementation UIWindowWrapper

@synthesize window_impl, allowed_orientations;

-(void) dealloc
{
  [[NSNotificationCenter defaultCenter] removeObserver:self];

  delete listeners;
  delete event_context;

  [super dealloc];
}

-(id) initWithFrame:(CGRect)rect
{
  self = [super initWithFrame:rect];

  if (!self)
    return nil;

  allowed_orientations = UIInterfaceOrientationMaskAll;

  try
  {
    event_context = new WindowEventContext;

    event_context->handle = self;

    listeners = new WindowListenerArray;
  }
  catch (...)
  {
    delete event_context;
    delete listeners;

    [self release];

    return nil;
  }

  self.rootViewController = [[UIViewControllerWrapper alloc] initWithWindow:self];
  [self.rootViewController release];

  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector (onShow) name:UIWindowDidBecomeVisibleNotification object:self];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector (onHide) name:UIWindowDidBecomeHiddenNotification object:self];
  [[NSNotificationCenter defaultCenter] addObserver:self selector:@selector (onKeyboardHide) name:UIKeyboardDidHideNotification object:nil];

  return self;
}

-(void)onPaint
{
  window_impl->Notify (WindowEvent_OnPaint, [self getEventContext]);
}

-(void)onShow
{
  window_impl->Notify (WindowEvent_OnShow, [self getEventContext]);
}

-(void)onHide
{
  window_impl->Notify (WindowEvent_OnHide, [self getEventContext]);
}

-(void)onKeyboardHide
{
  window_impl->Notify (WindowEvent_OnScreenKeyboardHide, [self getEventContext]);
}

-(WindowEventContext&) getEventContext
{
  event_context->touches_count = 0;

  return *event_context;
}

-(WindowEventContext&)getEventContextWithTouches:(NSSet*)touches
{
  if ([touches count] > MAX_TOUCHES_COUNT)
    @throw [NSException exceptionWithName:@"Invalid operation" reason:@"Touches event occured with touches count more than maximum" userInfo:nil];

  WindowEventContext& return_value = [self getEventContext];

  return_value.touches_count = [touches count];

  UIView       *view              = self.rootViewController.view;
  NSEnumerator *enumerator        = [touches objectEnumerator];
  Touch        *touch_description = return_value.touches;

  for (UITouch *iter = [enumerator nextObject]; iter; iter = [enumerator nextObject], touch_description++)
  {
    CGPoint current_location = [iter locationInView:view];

    touch_description->touch_id   = (size_t)iter;
    touch_description->position.x = current_location.x * [self contentScaleFactor];
    touch_description->position.y = current_location.y * [self contentScaleFactor];
  }

  return return_value;
}

-(void) touchesBegan:(NSSet*)touches withEvent:(UIEvent*)event
{
  window_impl->Notify (WindowEvent_OnTouchesBegan, [self getEventContextWithTouches:touches]);

  NSMutableSet* doubletap_touches = [[NSMutableSet alloc] initWithCapacity:[touches count]];

  for (UITouch* touch in touches)
  {
    if (!(touch.tapCount % 2))
      [doubletap_touches addObject:touch];
  }

  if ([doubletap_touches count])
    window_impl->Notify (WindowEvent_OnTouchesDoubletap, [self getEventContextWithTouches:doubletap_touches]);

  [doubletap_touches release];
}

-(void) touchesEnded:(NSSet*)touches withEvent:(UIEvent*)event
{
  window_impl->Notify (WindowEvent_OnTouchesEnded, [self getEventContextWithTouches:touches]);
}

-(void) touchesMoved:(NSSet*)touches withEvent:(UIEvent*)event
{
  window_impl->Notify (WindowEvent_OnTouchesMoved, [self getEventContextWithTouches:touches]);
}

-(void) touchesCancelled:(NSSet*)touches withEvent:(UIEvent*)event
{
  [self touchesEnded:touches withEvent:event];
}

-(void) motionBegan:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  if (motion != UIEventSubtypeMotionShake)
    return;

  for (WindowListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnShakeMotionBegan ();
}

-(void) motionEnded:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  if (motion != UIEventSubtypeMotionShake)
    return;

  for (WindowListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnShakeMotionEnded ();
}

-(void) motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent*)event
{
  [self motionEnded:motion withEvent:event];
}

-(void)onInterfaceOrientationWillChangeFrom:(InterfaceOrientation)from_orientation to:(InterfaceOrientation)to_orientation duration:(float)duration
{
  for (WindowListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnInterfaceOrientationWillChange (from_orientation, to_orientation, duration);
}

-(void)onInterfaceOrientationChangedFrom:(InterfaceOrientation)from_orientation to:(InterfaceOrientation)to_orientation
{
  for (WindowListenerArray::iterator iter = listeners->begin (), end = listeners->end (); iter != end; ++iter)
    (*iter)->OnInterfaceOrientationChanged (from_orientation, to_orientation);

  window_impl->Notify (WindowEvent_OnSize, [self getEventContext]);
}

-(void)onCharInput:(wchar_t)char_code
{
  WindowEventContext& current_event_context = [self getEventContext];

  current_event_context.char_code = char_code;

  window_impl->Notify (WindowEvent_OnChar, current_event_context);
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

window_t IPhoneWindowManager::CreateWindow (WindowStyle window_style, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::CreateWindow";

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception (METHOD_NAME, "Can't create window before entering run loop");

  if (parent_handle)
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

void IPhoneWindowManager::CloseWindow (window_t handle)
{
  if (!handle)
    return;

  WindowImpl* window = ((UIWindowWrapper*)handle).window_impl;

  window->Notify (WindowEvent_OnClose, [(UIWindowWrapper*)handle getEventContext]);
}

void IPhoneWindowManager::DestroyWindow (window_t handle)
{
  if (!handle)
    return;

  WindowImpl* window = ((UIWindowWrapper*)handle).window_impl;

  window->Notify (WindowEvent_OnDestroy, [(UIWindowWrapper*)handle getEventContext]);

  delete window;
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* IPhoneWindowManager::GetNativeWindowHandle (window_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GetNativeWindowHandle", "handle");

  return reinterpret_cast<const void*> (handle);
}

const void* IPhoneWindowManager::GetNativeDisplayHandle (window_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GetNativeDisplayHandle", "handle");

  return 0;
}

/*
    Заголовок окна
*/

void IPhoneWindowManager::SetWindowTitle (window_t handle, const wchar_t* title)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::SetWindowTitle";

  if (!handle)
    throw xtl::make_null_argument_exception (METHOD_NAME, "handle");

  if (!title)
    throw xtl::make_null_argument_exception (METHOD_NAME, "title");
}

void IPhoneWindowManager::GetWindowTitle (window_t handle, size_t size, wchar_t* buffer)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::GetWindowTitle";

  if (!handle)
    throw xtl::make_null_argument_exception (METHOD_NAME, "handle");

  if (!size)
    return;

  if (!buffer)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  *buffer = L'\0';
}

/*
    Область окна / клиентская область
*/

void IPhoneWindowManager::SetWindowRect (window_t handle, const Rect& rect)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::SetWindowRect", "handle");

  UIView*          view           = (UIView*)handle;
  UIWindowWrapper* window_wrapper = nil;
  UIWindow*        window         = nil;

  if ([view isKindOfClass:[UIWindowWrapper class]])
  {
    window_wrapper = (UIWindowWrapper*)handle;
    window         = (UIWindow*)handle;
  }
  else
    window = view.window;

  float scale_factor = [UIScreen mainScreen].scale;

  CGRect frame;

  frame.size.width  = (rect.right - rect.left) / scale_factor;
  frame.size.height = (rect.bottom - rect.top) / scale_factor;
  frame.origin.x    = rect.left / scale_factor;
  frame.origin.y    = rect.top / scale_factor;

  view.frame = get_transformed_view_rect_size (frame, view);

  frame = view.frame;

  if (window_wrapper)
  {
    WindowImpl* window_impl = window_wrapper.window_impl;

    WindowEventContext& dummy_context = [window_wrapper getEventContext];

    window_impl->Notify (WindowEvent_OnMove, dummy_context);
    window_impl->Notify (WindowEvent_OnSize, dummy_context);
  }
}

void IPhoneWindowManager::SetClientRect (window_t handle, const Rect& rect)
{
  SetWindowRect (handle, rect);
}

void IPhoneWindowManager::GetWindowRect (window_t handle, Rect& rect)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GetWindowRect", "handle");

  UIView *view = (UIView*)handle;

  if ([view isKindOfClass:[UIWindowWrapper class]])
    view = ((UIWindow*)view).rootViewController.view;

  float scale_factor = [UIScreen mainScreen].scale;

  CGRect frame = view.frame;

  rect.bottom = (frame.origin.y + frame.size.height) * scale_factor;
  rect.right  = (frame.origin.x + frame.size.width) * scale_factor;
  rect.top    = frame.origin.y * scale_factor;
  rect.left   = frame.origin.x * scale_factor;

  if (!CGAffineTransformIsIdentity (view.transform))
  {
      size_t temp = rect.bottom;

      rect.bottom = rect.right;
      rect.right  = temp;

      temp = rect.top;

      rect.top  = rect.left;
      rect.left = temp;
  }
}

void IPhoneWindowManager::GetClientRect (window_t handle, Rect& target_rect)
{
  GetWindowRect (handle, target_rect);
}

/*
    Установка флагов окна
*/

void IPhoneWindowManager::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::SetWindowFlag", "handle");

  if (state == GetWindowFlag (handle, flag))
    return;

  UIView             *view          = (UIView*)handle;
  UIWindowWrapper    *wnd           = nil;
  UIWindow           *window        = nil;
  WindowImpl         *window_impl   = 0;
  WindowEventContext *dummy_context = 0;

  if ([view isKindOfClass:[UIWindowWrapper class]])
  {
    wnd           = (UIWindowWrapper*)handle;
    window        = wnd;
    window_impl   = wnd.window_impl;
    dummy_context = &[wnd getEventContext];
  }
  else
    window = view.window;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
        if (state)
        {
          [wnd makeKeyAndVisible];

          view.hidden = NO;

          if (window_impl)
            window_impl->Notify (WindowEvent_OnShow, *dummy_context);
        }
        else
        {
          view.hidden = YES;

          if (window_impl)
            window_impl->Notify (WindowEvent_OnHide, *dummy_context);
        }

        break;
      case WindowFlag_Active: //активность окна
        if (state)
        {
          [wnd makeKeyAndVisible];

          if (window_impl)
            window_impl->Notify (WindowEvent_OnActivate, *dummy_context);
        }
        else
          throw xtl::format_operation_exception ("", "Can't make window inactive");

        break;
      case WindowFlag_Focus: //фокус ввода
        if (state)
        {
          if (wnd)
            wnd.rootViewController.view.userInteractionEnabled = YES;
          else
            view.userInteractionEnabled = YES;

          if (window_impl)
            window_impl->Notify (WindowEvent_OnSetFocus, *dummy_context);
        }
        else
        {
          if (wnd)
            wnd.rootViewController.view.userInteractionEnabled = NO;
          else
            view.userInteractionEnabled = NO;

          if (window_impl)
            window_impl->Notify (WindowEvent_OnLostFocus, *dummy_context);
        }

        break;
      case WindowFlag_Maximized:
        if (wnd)
          wnd.bounds = [UIScreen mainScreen].applicationFrame;
        else
          view.frame = window.rootViewController.view.bounds;

        break;
      case WindowFlag_Minimized:
        throw xtl::format_operation_exception ("", "Can't minimize window");
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::IPhoneWindowManager::SetWindowFlag");
    throw;
  }
}

bool IPhoneWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::GetWindowFlag", "handle");

  UIView*          view   = (UIView*)handle;
  UIWindow*        window = 0;
  UIWindowWrapper* wnd    = 0;

  if ([view isKindOfClass:[UIWindowWrapper class]])
  {
    wnd    = (UIWindowWrapper*)handle;
    window = wnd;
  }
  else
    window = view.window;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
        return view.hidden == NO;
      case WindowFlag_Active:
        return window == [UIApplication sharedApplication].keyWindow;
      case WindowFlag_Focus:
        if (wnd)
          return wnd.rootViewController.view.userInteractionEnabled == YES;
        else
          return view.userInteractionEnabled == YES;
      case WindowFlag_Maximized:
      {
        CGRect maximized_rect;

        if (wnd)
          maximized_rect = [UIScreen mainScreen].applicationFrame;
        else
          maximized_rect = get_transformed_view_rect_size (window.rootViewController.view.bounds, view);

        maximized_rect.origin.x = 0;
        maximized_rect.origin.y = 0;

        return CGRectEqualToRect (view.frame, maximized_rect);
      }
      case WindowFlag_Minimized:
        throw xtl::format_operation_exception ("", "Can't get window flag %d value", flag);
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::IPhoneWindowManager::GetWindowFlag");
    throw;
  }
}

/*
    Обновление окна
*/

void IPhoneWindowManager::InvalidateWindow (window_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::InvalidateWindow", "handle");

  if ([(id)handle isKindOfClass:[UIWindowWrapper class]])
    [((UIWindowWrapper*)handle).rootViewController.view setNeedsDisplay];
  else
    [(UIView*)handle setNeedsDisplay];
}

/*
   Установка multitouch режима для окна
*/

void IPhoneWindowManager::SetMultitouchEnabled (window_t window, bool enabled)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::SetMultitouchEnabled", "window");

  if ([(id)window isKindOfClass:[UIWindowWrapper class]])
    ((UIWindowWrapper*)window).rootViewController.view.multipleTouchEnabled = enabled;
  else
    ((UIView*)window).multipleTouchEnabled = enabled;
}

bool IPhoneWindowManager::IsMultitouchEnabled (window_t window)
{
  if (!window)
    throw xtl::make_null_argument_exception ("syslib::IPhoneWindowManager::IsMultitouchEnabled", "window");

  if ([(id)window isKindOfClass:[UIWindowWrapper class]])
    return ((UIWindowWrapper*)window).rootViewController.view.multipleTouchEnabled;
  else
    return ((UIView*)window).multipleTouchEnabled;
}

/*
   Цвет фона
*/

void IPhoneWindowManager::SetBackgroundColor (window_t window, const Color& color)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::SetBackgroundColor";

  if (!window)
    throw xtl::make_null_argument_exception (METHOD_NAME, "window");

  if (![(id)window isKindOfClass:[UIWindowWrapper class]])
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set background color for this window");

  UIWindow*      wnd  = (UIWindow*)window;
  UIViewWrapper* view = (UIViewWrapper*)wnd.rootViewController.view;

  UIColor *background_color = [[UIColor alloc] initWithRed:color.red / 255.f green:color.green / 255.f blue:color.blue / 255.f alpha:1];

  [view setBackground_color:color];

  view.ui_background_color = background_color;

  if (view.background_state)
    view.backgroundColor = background_color;

  [background_color release];
}

void IPhoneWindowManager::SetBackgroundState (window_t window, bool state)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::SetBackgroundState";

  if (!window)
    throw xtl::make_null_argument_exception (METHOD_NAME, "window");

  if (![(id)window isKindOfClass:[UIWindowWrapper class]])
    throw xtl::format_operation_exception (METHOD_NAME, "Can't set background state for this window");

  UIWindow*      wnd  = (UIWindow*)window;
  UIViewWrapper* view = (UIViewWrapper*)wnd.rootViewController.view;

  view.clearsContextBeforeDrawing = state;
  view.backgroundColor            = state ? view.ui_background_color : nil;
  view.background_state           = state;
}

Color IPhoneWindowManager::GetBackgroundColor (window_t window)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::GetBackgroundColor";

  if (!window)
    throw xtl::make_null_argument_exception (METHOD_NAME, "window");

  if (![(id)window isKindOfClass:[UIWindowWrapper class]])
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get background color for this window");

  UIWindow*      wnd  = (UIWindow*)window;
  UIViewWrapper *view = (UIViewWrapper*)wnd.rootViewController.view;

  return view.ui_background_color ? view.background_color : Color ();
}

bool IPhoneWindowManager::GetBackgroundState (window_t window)
{
  static const char* METHOD_NAME = "syslib::IPhoneWindowManager::GetBackgroundState";

  if (!window)
    throw xtl::make_null_argument_exception (METHOD_NAME, "window");

  if (![(id)window isKindOfClass:[UIWindowWrapper class]])
    throw xtl::format_operation_exception (METHOD_NAME, "Can't get background state for this window");

  return ((UIViewWrapper*)((UIWindow*)window).rootViewController.view).background_state;
}

/*
   Менеджер окон
*/

/*
   Добавление/удаление подписчиков
*/

void WindowManager::AttachWindowListener (const Window& window, IWindowListener* listener)
{
  if  (!listener)
    return;

  if (!is_in_run_loop ())
    throw xtl::format_operation_exception ("syslib::iphone::attach_window_listener", "Can't attach window listener before entering run loop");

  [(UIWindowWrapper*)(window.Handle ()) attachListener:listener];
}

void WindowManager::DetachWindowListener (const Window& window, IWindowListener* listener)
{
  if (!is_in_run_loop ())
    return;

  [(UIWindowWrapper*)(window.Handle ()) detachListener:listener];
}

/*
   Установка/получение разрешенных ориентаций окна
*/

void WindowManager::SetAllowedOrientations (const Window& window, int orientations)
{
  ((UIWindowWrapper*)window.Handle ()).allowed_orientations = orientations;
}

int WindowManager::GetAllowedOrientations (const Window& window)
{
  return ((UIWindowWrapper*)window.Handle ()).allowed_orientations;
}
