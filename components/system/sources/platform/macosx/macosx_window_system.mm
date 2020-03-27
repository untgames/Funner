#import <AppKit/AppKit.h>
#import <Foundation/NSData.h>

#include "shared.h"

using namespace syslib;
using namespace syslib::macosx;

namespace
{

/*
    Window implementation
*/

struct WindowImpl
{
  void*                user_data;       //user data
  WindowMessageHandler message_handler; //window events handler

  WindowImpl (WindowMessageHandler handler, void* in_user_data)
    : user_data (in_user_data)
    , message_handler (handler)
    {}

  void Notify (window_t window, WindowEvent event, const WindowEventContext& context)
  {
    try
    {
      message_handler (window, event, context, user_data);
    }
    catch (...)
    {
      //supress all exceptions
    }
  }
};

/*
    Maps virtual key code to syslib::Key
*/

Key VirtualKey2SystemKey (size_t vkey)
{
  static size_t map [] =
  {
    Key_A, Key_S, Key_D, Key_F, Key_H, Key_G, Key_Z, Key_X,  //0x00-0x07
    Key_C, Key_V, 0, Key_B, Key_Q, Key_W, Key_E, Key_R,  //0x08-0x0F
    Key_Y, Key_T, Key_1, Key_2, Key_3, Key_4, Key_6, Key_5, //0x10-0x17
    Key_Plus, Key_9, Key_7, Key_Minus, Key_8, Key_0, Key_RightBracket, Key_O, //0x18-0x1F
    Key_U, Key_LeftBracket, Key_I, Key_P, Key_Enter, Key_L, Key_J, Key_Apostrophe, //0x20-0x27
    Key_K, Key_Semicolon, Key_BackSlash, Key_Comma, Key_Slash, Key_N, Key_M, Key_Dot, //0x28-0x2F
    Key_Tab, Key_Space, Key_Tilda, Key_BackSpace, 0, Key_Escape, 0, 0, //0x30-0x37
    0, 0, 0, 0, 0, 0, 0, 0, //0x38-0x3F
    0, Key_NumPadDot, 0, Key_NumPadMultiply, 0, Key_NumPadPlus, 0, Key_NumLock, //0x40-0x47
    0, 0, 0, Key_NumPadDivide, Key_Enter, 0, Key_NumPadMinus, 0, //0x48-0x4F
    0, 0, Key_NumPad0, Key_NumPad1, Key_NumPad2, Key_NumPad3, Key_NumPad4, Key_NumPad5, //0x50-0x57
    Key_NumPad6, Key_NumPad7, 0, Key_NumPad8, Key_NumPad9, 0, 0, 0, //0x58-0x5F
    Key_F5, Key_F6, Key_F7, Key_F3, Key_F8, Key_F9, 0, Key_F11, //0x60-0x67
    0, 0, 0, 0, 0, Key_F10, 0, Key_F12, //0x68-0x6F
    0, 0, Key_Insert, Key_Home, Key_PageUp, Key_Delete, Key_F4, Key_End, //0x70-0x77
    Key_F2, Key_PageDown, Key_F1, Key_Left, Key_Right, Key_Down, Key_Up, 0, //0x78-0x7F
    0, 0, 0, 0, 0, 0, 0, 0, //0x80-0x87
    0, 0, 0, 0, 0, 0, 0, 0, //0x88-0x8F
    0, 0, 0, 0, 0, 0, 0, 0, //0x90-0x97
    0, 0, 0, 0, 0, 0, 0, 0, //0x98-0x9F
    0, 0, 0, 0, 0, 0, 0, 0, //0xA0-0xA7
    0, 0, 0, 0, 0, 0, 0, 0, //0xA8-0xAF
    0, 0, 0, 0, 0, 0, 0, 0, //0xB0-0xB7
    0, 0, 0, 0, 0, 0, 0, 0, //0xB8-0xBF
    0, 0, 0, 0, 0, 0, 0, 0, //0xC0-0xC7
    0, 0, 0, 0, 0, 0, 0, 0, //0xC8-0xCF
    0, 0, 0, 0, 0, 0, 0, 0, //0xD0-0xD7
    0, 0, 0, 0, 0, 0, 0, 0, //0xD8-0xDF
    0, 0, 0, 0, 0, 0, 0, 0, //0xE0-0xE7
    0, 0, 0, 0, 0, 0, 0, 0, //0xE8-0xEF
    0, 0, 0, 0, 0, 0, 0, 0, //0xF0-0xF7
    0, 0, 0, 0, 0, 0, 0, 0, //0xF8-0xFF
  };

  if (vkey >= sizeof (map) / sizeof (*map))
    return Key_Unknown;

  return (Key)map [vkey];
}

NSWindowStyleMask get_window_style (WindowStyle style)
{
  switch (style)
  {
    case WindowStyle_Overlapped:
      return NSTitledWindowMask | NSClosableWindowMask | NSMiniaturizableWindowMask | NSResizableWindowMask;
    case WindowStyle_PopUp:
      return NSBorderlessWindowMask;
    default:
      throw xtl::make_argument_exception ("::get_window_style", "style", style);
  }
}

bool is_cursor_in_client_region (NSWindow* window)
{
  NSPoint position = [window mouseLocationOutsideOfEventStream];

  return position.x >= 0 && position.y >= 0 && position.x < window.frame.size.width && position.y < window.frame.size.height;
}

}

@interface NSWindowWrapper : NSWindow <NSWindowDelegate>
{
  @private
    WindowEventContext event_context;
    NSCursor           *cursor;
    WindowImpl         *window_impl;
    bool               is_cursor_visible;
    bool               is_multitouch_enabled;
    bool               is_maximized;
    bool               background_state;
    bool               last_visible_state;
}

@property (nonatomic, retain) NSCursor*   cursor;
@property (nonatomic, assign) WindowImpl* window_impl;
@property (nonatomic)         bool        is_cursor_visible;
@property (nonatomic)         bool        is_multitouch_enabled;
@property (nonatomic)         bool        is_maximized;
@property (nonatomic)         bool        background_state;

-(void) onKeyDown:(NSEvent *)event;
-(void) onKeyUp:(NSEvent *)event;
-(void) onMouseDown:(NSEvent *)event;
-(void) onMouseEntered;
-(void) onMouseExited;
-(void) onMouseMoved:(NSEvent *)event;
-(void) onMouseUp:(NSEvent *)event;
-(void) onPaint;
-(void) onScrollWheel:(NSEvent *)event;

@end


/*
   Class which handles view events
*/

@interface NSViewWrapper : NSView
{
}

@end

@implementation NSViewWrapper

-(BOOL) acceptsFirstResponder
{
  return YES;
}

-(void) drawRect:(NSRect)rect
{
  [(NSWindowWrapper*)self.window onPaint];
}

- (void)mouseDown:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onMouseDown:event];
}

- (void)rightMouseDown:(NSEvent *)event
{
  [super rightMouseDown:event];

  [(NSWindowWrapper*)self.window onMouseDown:event];
}

- (void)otherMouseDown:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onMouseDown:event];
}

- (void)mouseUp:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onMouseUp:event];
}

- (void)rightMouseUp:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onMouseUp:event];
}

- (void)otherMouseUp:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onMouseUp:event];
}

- (void)keyDown:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onKeyDown:event];
}

- (void)keyUp:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onKeyUp:event];
}

- (void)mouseMoved:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onMouseMoved:event];
}

- (void)mouseDragged:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onMouseMoved:event];
}

- (void)mouseEntered:(NSEvent *)theEvent
{
  [(NSWindowWrapper*)self.window onMouseEntered];
}

- (void)mouseExited:(NSEvent *)theEvent
{
  [(NSWindowWrapper*)self.window onMouseExited];
}

- (void)scrollWheel:(NSEvent *)event
{
  [(NSWindowWrapper*)self.window onScrollWheel:event];
}

@end

@implementation NSWindowWrapper

@synthesize cursor, window_impl, is_cursor_visible, is_multitouch_enabled, is_maximized, background_state;

- (instancetype)initWithContentRect:(NSRect)contentRect
                          styleMask:(NSWindowStyleMask)style
                            backing:(NSBackingStoreType)backingStoreType
                              defer:(BOOL)flag
{
  self = [super initWithContentRect:contentRect
                          styleMask:style
                            backing:backingStoreType
                              defer:flag];
  if (!self)
    return nil;

  is_cursor_visible = true;

  self.contentView = [[[NSViewWrapper alloc] initWithFrame:NSMakeRect (0, 0, 1, 1)] autorelease];

  self.contentView.wantsBestResolutionOpenGLSurface = YES;

  NSTrackingArea* tracking_area = [[NSTrackingArea alloc] initWithRect:NSMakeRect (0, 0, 1, 1)
                                                               options:NSTrackingMouseEnteredAndExited | NSTrackingMouseMoved | NSTrackingActiveAlways | NSTrackingInVisibleRect
                                                                 owner:self.contentView
                                                              userInfo:nil];

  [self.contentView addTrackingArea:tracking_area];

  [tracking_area release];

  self.cursor   = [NSCursor arrowCursor];
  self.delegate = self;

  return self;
}

- (void)dealloc
{
  if (window_impl)
    window_impl->Notify ((window_t)self, WindowEvent_OnDestroy, [self getEventContext:nil]);

  self.delegate = nil;
  self.cursor   = nil;

  [super dealloc];
}

- (BOOL)canBecomeKeyWindow
{
  return YES;
}

- (BOOL)canBecomeMainWindow
{
  return YES;
}

- (void)onMouseDown:(NSEvent *)event
{
  if (!window_impl)
    return;

  WindowEvent window_event;

  switch (event.type)
  {
    case NSEventTypeLeftMouseDown:
      if (event.clickCount % 2)
        window_event = WindowEvent_OnLeftButtonDown;
      else
        window_event = WindowEvent_OnLeftButtonDoubleClick;
      break;
    case NSEventTypeRightMouseDown:
      if (event.clickCount % 2)
        window_event = WindowEvent_OnRightButtonDown;
      else
        window_event = WindowEvent_OnRightButtonDoubleClick;
      break;
    case NSEventTypeOtherMouseDown:
    {
      switch (event.buttonNumber)
      {
        case 2:
          if (event.clickCount % 2)
            window_event = WindowEvent_OnMiddleButtonDown;
          else
            window_event = WindowEvent_OnMiddleButtonDoubleClick;
          break;
        case 3:
          if (event.clickCount % 2)
            window_event = WindowEvent_OnXButton1Down;
          else
            window_event = WindowEvent_OnXButton1DoubleClick;
          break;
        case 4:
          if (event.clickCount % 2)
            window_event = WindowEvent_OnXButton2Down;
          else
            window_event = WindowEvent_OnXButton2DoubleClick;
          break;
        default:
          return;
      }
      break;
    }
    default:
      //unknown mouse button
      return;
  }

  window_impl->Notify ((window_t)self, window_event, [self getEventContext:event]);
}

- (void)onMouseUp:(NSEvent *)event
{
  if (!window_impl)
    return;

  WindowEvent window_event;

  switch (event.type)
  {
    case NSEventTypeLeftMouseUp:
      window_event = WindowEvent_OnLeftButtonUp;
      break;
    case NSEventTypeRightMouseUp:
      window_event = WindowEvent_OnRightButtonUp;
      break;
    case NSEventTypeOtherMouseUp:
    {
      switch (event.buttonNumber)
      {
        case 2:
          window_event = WindowEvent_OnMiddleButtonUp;
          break;
        case 3:
          window_event = WindowEvent_OnXButton1Up;
          break;
        case 4:
          window_event = WindowEvent_OnXButton2Up;
          break;
        default:
          return;
      }
      break;
    }
    default:
      //unknown mouse button
      return;
  }

  window_impl->Notify ((window_t)self, window_event, [self getEventContext:event]);
}

- (void)onMouseMoved:(NSEvent *)event
{
  if (!window_impl)
    return;

  window_impl->Notify ((window_t)self, WindowEvent_OnMouseMove, [self getEventContext:event]);
}

- (void)onMouseEntered
{
  try
  {
    if (!is_cursor_visible && CGCursorIsVisible ()) //hide cursor on entering window if it should not be visible
      check_quartz_error (CGDisplayHideCursor (kCGDirectMainDisplay), "::CGDisplayHideCursor", "Can't hide cursor");
    else
      [cursor set];
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at processing onMouseEntered event: '%s'\n", e.what ());
  }
  catch (...)
  {
    printf ("Exception at processing onMouseEntered event: unknown exception\n");
  }

  if (!window_impl)
    return;

  window_impl->Notify ((window_t)self, WindowEvent_OnMouseLeave, [self getEventContext:nil]);
}

- (void)onMouseExited
{
  try
  {
    if (!is_cursor_visible) //show cursor on exiting window if it was not visible
      if (!CGCursorIsVisible ())
        CGDisplayShowCursor (kCGDirectMainDisplay);

    [[NSCursor arrowCursor] set];
  }
  catch (xtl::exception& e)
  {
    printf ("Exception at processing onMouseExited event: '%s'\n", e.what ());
  }
  catch (...)
  {
    printf ("Exception at processing onMouseExited event: unknown exception\n");
  }

  if (!window_impl)
    return;

  window_impl->Notify ((window_t)self, WindowEvent_OnMouseLeave, [self getEventContext:nil]);
}

- (void)onKeyEvent:(NSEvent *)event windowEvent:(WindowEvent)windowEvent
{
  if (!window_impl)
    return;

  WindowEventContext& context = [self getEventContext:event];

  context.key           = VirtualKey2SystemKey (event.keyCode);
  context.key_scan_code = (syslib::ScanCode)event.keyCode;

  if (windowEvent == WindowEvent_OnKeyDown)
  {
    NSString* characters = event.characters;

    for (unsigned int i = 0, count = [characters length]; i < count; i++)
    {
      [characters getBytes:&context.char_code
                 maxLength:sizeof (wchar_t)
                usedLength:0
                  encoding:NSUTF32StringEncoding
                   options:0
                     range:NSMakeRange (i, 1)
            remainingRange:0];

      window_impl->Notify ((window_t)self, WindowEvent_OnChar, context);
    }
  }

  window_impl->Notify ((window_t)self, windowEvent, context);
}

- (void)onKeyDown:(NSEvent *)event
{
  [self onKeyEvent:event windowEvent:WindowEvent_OnKeyDown];
}

- (void)onKeyUp:(NSEvent *)event
{
  [self onKeyEvent:event windowEvent:WindowEvent_OnKeyUp];
}

- (void)onScrollWheel:(NSEvent *)event
{
  if (!window_impl)
    return;

  WindowEventContext& context = [self getEventContext:event];

  context.mouse_horisontal_wheel_delta = event.scrollingDeltaX;
  context.mouse_vertical_wheel_delta   = event.scrollingDeltaY;

  if (context.mouse_horisontal_wheel_delta != 0)
    window_impl->Notify ((window_t)self, WindowEvent_OnMouseHorisontalWheel, context);

  if (context.mouse_vertical_wheel_delta != 0)
    window_impl->Notify ((window_t)self, WindowEvent_OnMouseVerticalWheel, context);
}

-(void)onPaint
{
  if (window_impl)
    window_impl->Notify ((window_t)self, WindowEvent_OnPaint, [self getEventContext:nil]);
}

- (void)windowWillClose:(NSNotification *)notification
{
  if (window_impl)
    window_impl->Notify ((window_t)self, WindowEvent_OnClose, [self getEventContext:nil]);
}

- (void)windowDidBecomeKey:(NSNotification *)notification
{
  if (!window_impl)
    return;

  window_impl->Notify ((window_t)self, WindowEvent_OnSetFocus, [self getEventContext:nil]);
  window_impl->Notify ((window_t)self, WindowEvent_OnActivate, [self getEventContext:nil]);
}

- (void)windowDidResignKey:(NSNotification *)notification
{
  if (!window_impl)
    return;

  window_impl->Notify ((window_t)self, WindowEvent_OnDeactivate, [self getEventContext:nil]);
  window_impl->Notify ((window_t)self, WindowEvent_OnLostFocus, [self getEventContext:nil]);
}

- (void)windowDidChangeOcclusionState:(NSNotification *)notification
{
  if (!window_impl)
    return;

  if (self.visible == last_visible_state)
    return;

  last_visible_state = self.visible;

  if (self.visible)
    window_impl->Notify ((window_t)self, WindowEvent_OnShow, [self getEventContext:nil]);
  else
    window_impl->Notify ((window_t)self, WindowEvent_OnHide, [self getEventContext:nil]);
}

- (void)windowDidResize:(NSNotification *)notification
{
  if (!window_impl)
    return;

  window_impl->Notify ((window_t)self, WindowEvent_OnSize, [self getEventContext:nil]);
}

- (void)windowDidMove:(NSNotification *)notification
{
  if (!window_impl)
    return;

  window_impl->Notify ((window_t)self, WindowEvent_OnMove, [self getEventContext:nil]);
}

- (WindowEventContext&)getEventContext:(NSEvent *)event
{
  memset (&event_context, 0, sizeof (event_context));

  event_context.handle = self;

  if (event)
  {
    if (event.window)
    {
      event_context.cursor_position.x = event.locationInWindow.x * self.backingScaleFactor;
      event_context.cursor_position.y = (event.window.contentView.frame.size.height - event.locationInWindow.y) * self.backingScaleFactor;
    }

    event_context.keyboard_alt_pressed        = event.modifierFlags & NSEventModifierFlagOption;
    event_context.keyboard_control_pressed    = event.modifierFlags & NSEventModifierFlagControl;
    event_context.keyboard_shift_pressed      = event.modifierFlags & NSEventModifierFlagShift;

    event_context.mouse_left_button_pressed   = NSEvent.pressedMouseButtons & 1;
    event_context.mouse_right_button_pressed  = NSEvent.pressedMouseButtons & (1 << 1);
    event_context.mouse_middle_button_pressed = NSEvent.pressedMouseButtons & (1 << 2);
  }

  return event_context;
}

@end

/*
    Create/close/destroy window
*/

window_t MacOSXWindowManager::CreateWindow (WindowStyle style, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  try
  {
    NSWindowStyleMask window_style = get_window_style (style);

    WindowImpl* window_impl = new WindowImpl (handler, user_data);

    //Create window
    NSWindowWrapper* new_window = [[NSWindowWrapper alloc] initWithContentRect:NSMakeRect (0, 0, 400, 600)
                                                                     styleMask:window_style
                                                                       backing:NSBackingStoreBuffered
                                                                         defer:YES];

    new_window.releasedWhenClosed = NO; //window will be released by DestroyWindow

    new_window.window_impl = window_impl;

    return (window_t)new_window;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::MacOSXWindowManager::CreateWindow");

    throw;
  }

  return 0;
}

void MacOSXWindowManager::CloseWindow (window_t handle)
{
  //performClose doesn't closes window in window2 test
  [(NSWindow*)handle close];
}

void MacOSXWindowManager::DestroyWindow (window_t handle)
{
  [(NSWindow*)handle release];
}

/*
   Try to change window style (can be ignored)
*/

bool MacOSXWindowManager::ChangeWindowStyle (window_t handle, WindowStyle style)
{
  //TODO
  return false;
}

/*
    Get platform-dependent window handle
*/

const void* MacOSXWindowManager::GetNativeWindowHandle (window_t handle)
{
  return reinterpret_cast<const void*> (handle);
}

const void* MacOSXWindowManager::GetNativeDisplayHandle (window_t)
{
  return 0;
}

/*
    Window title
*/

void MacOSXWindowManager::SetWindowTitle (window_t handle, const wchar_t* title)
{
  CFStringRef new_title = 0;

  try
  {
    if (!title)
      throw xtl::make_null_argument_exception ("", "title");

    CFStringEncoding title_encoding;

    int dummy = 1;

    if (reinterpret_cast<char*> (&dummy)[0])
    {
      //Little endian
      if (sizeof (wchar_t) == 4) title_encoding = kCFStringEncodingUTF32LE;
      else                       title_encoding = kCFStringEncodingUTF16LE;
    }
    else
    {
      //Big endian
      if (sizeof (wchar_t) == 4) title_encoding = kCFStringEncodingUTF32BE;
      else                       title_encoding = kCFStringEncodingUTF16BE;
    }

    new_title = CFStringCreateWithBytes (0, (UInt8*)title, xtl::xstrlen (title) * sizeof (wchar_t),
                                         title_encoding, false);

    if (!new_title)
      throw xtl::format_operation_exception ("", "Can't create string with new title, CFStringCreateWithCharacters error");


    ((NSWindow*)handle).title = (NSString*)new_title;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::MacOSXWindowManager::SetWindowTitle");
    CFRelease (new_title);
    throw;
  }

  CFRelease (new_title);
}

void MacOSXWindowManager::GetWindowTitle (window_t handle, size_t buffer_size_in_chars, wchar_t* buffer)
{
  CFStringRef window_title = (CFStringRef)((NSWindow*)handle).title;

  CFRange title_range = { 0, CFStringGetLength (window_title) };

  CFIndex getted_chars = CFStringGetBytes (window_title, title_range, kCFStringEncodingUTF32, '?', false, (UInt8*)buffer,
                                           (buffer_size_in_chars - 1) * sizeof (wchar_t), 0);

  buffer [getted_chars] = 0;
}

/*
    Window rect / client rect
*/

void MacOSXWindowManager::SetWindowRect (window_t handle, const Rect& rect)
{
  //TODO support for multiple screens
  NSWindow* window = (NSWindow*)handle;

  float bottom = NSScreen.mainScreen.visibleFrame.size.height - rect.bottom / window.backingScaleFactor;

  [window setFrame:NSMakeRect (rect.left / window.backingScaleFactor, bottom, (rect.right - rect.left) / window.backingScaleFactor, (rect.bottom - rect.top) / window.backingScaleFactor) display:YES];
}

void MacOSXWindowManager::SetClientRect (window_t handle, const Rect& rect)
{
  //TODO support for multiple screens
  NSWindow* window = (NSWindow*)handle;

  float bottom = NSScreen.mainScreen.visibleFrame.size.height - rect.bottom / window.backingScaleFactor;

  [(NSWindow*)handle setFrameOrigin:NSMakePoint (rect.left / window.backingScaleFactor, bottom)];
  [(NSWindow*)handle setContentSize:NSMakeSize ((rect.right - rect.left) / window.backingScaleFactor, (rect.bottom - rect.top) / window.backingScaleFactor)];
}

void MacOSXWindowManager::GetWindowRect (window_t handle, Rect& rect)
{
  NSWindow *window = (NSWindow*)handle;

  CGRect frame = window.frame;

  //TODO support for multiple screens
  unsigned int screen_height = NSScreen.mainScreen.visibleFrame.size.height;

  rect.bottom = (screen_height - frame.origin.y) * window.backingScaleFactor;
  rect.right  = (frame.origin.x + frame.size.width) * window.backingScaleFactor;
  rect.top    = (screen_height - (frame.origin.y + frame.size.height)) * window.backingScaleFactor;
  rect.left   = frame.origin.x * window.backingScaleFactor;
}

void MacOSXWindowManager::GetClientRect (window_t handle, Rect& rect)
{
  NSWindow *window = (NSWindow*)handle;
  NSView   *view   = window.contentView;

  CGRect frame = view.frame;

  rect.bottom = (frame.origin.y + frame.size.height) * window.backingScaleFactor;
  rect.right  = (frame.origin.x + frame.size.width) * window.backingScaleFactor;
  rect.top    = frame.origin.y * window.backingScaleFactor;
  rect.left   = frame.origin.x * window.backingScaleFactor;
}

/*
    Window flags
*/

void MacOSXWindowManager::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  NSWindowWrapper* wnd = (NSWindowWrapper*)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
        if (state)
        {
          [wnd makeKeyAndOrderFront:wnd];
          [wnd makeMainWindow];
        }
        else
          [wnd orderOut:wnd];
        break;
      case WindowFlag_Active:
        if (state)
          [wnd makeMainWindow];
        else
        {
          //can't make window inactive
        }

        break;
      case WindowFlag_Focus:
        if (state)
          [wnd makeKeyWindow];
        else
        {
          //can't remove window focus
        }

        break;
      case WindowFlag_Maximized:
        if (!wnd.visible)
          [wnd setIsVisible:true];

        if (wnd.styleMask == NSBorderlessWindowMask)
        {
          if (state)
          {
            check_window_manager_error (SetSystemUIMode (kUIModeAllHidden, 0), "::SetSystemUIMode", "Can't set required system UI mode");

            wnd.is_maximized = true;

            CGRect display_bounds = CGDisplayBounds (CGMainDisplayID ());

            [wnd setFrame:display_bounds display:YES];
          }
          else
          {
            check_window_manager_error (SetSystemUIMode (kUIModeNormal, 0), "::SetSystemUIMode", "Can't set required system UI mode");

            wnd.is_maximized = false;
          }
        }
        else
          if (wnd.zoomed != state)
            [wnd zoom:wnd];

        break;
      case WindowFlag_Minimized:
        if (wnd.miniaturized != state)
        {
          if (state)
            [wnd miniaturize:wnd];
          else
            [wnd deminiaturize:wnd];
        }
        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
        break;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::MacOSXWindowManager::SetWindowFlag");
    throw;
  }
}

bool MacOSXWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  NSWindowWrapper* wnd = (NSWindowWrapper*)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
        return wnd.visible;
      case WindowFlag_Active:
        return wnd.mainWindow;
      case WindowFlag_Focus:
        return wnd.keyWindow;
      case WindowFlag_Maximized:
        return wnd.zoomed;
      case WindowFlag_Minimized:
        return wnd.miniaturized;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
        break;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::MacOSXWindowManager::GetWindowFlag");
    throw;
  }
}

/*
    Window parenting
*/

void MacOSXWindowManager::SetParentWindowHandle (window_t child, const void* parent)
{
  throw xtl::make_not_implemented_exception ("syslib::MacOSXWindowManager::SetParentWindowHandle");
}

const void* MacOSXWindowManager::GetParentWindowHandle (window_t child)
{
  throw xtl::make_not_implemented_exception ("syslib::MacOSXWindowManager::GetParentWindowHandle");
}

/*
    Window updating
*/

void MacOSXWindowManager::InvalidateWindow (window_t handle)
{
  if ([(id)handle isKindOfClass:[NSWindow class]])
    ((NSWindow*)handle).contentView.needsDisplay = YES;
}

/*
    Cursor position
*/

void MacOSXWindowManager::SetCursorPosition (const Point& position)
{
  float   screen_scale_factor = [NSScreen mainScreen].backingScaleFactor;
  CGPoint new_cursor_position = { position.x / screen_scale_factor, position.y / screen_scale_factor };

  try
  {
    CGEventRef event = CGEventCreateMouseEvent (0, kCGEventMouseMoved, new_cursor_position, 0);

    if (!event)
      throw xtl::format_operation_exception ("::CGEventCreateMouseEvent", "Can't create mouse event");

    CGEventPost (kCGHIDEventTap, event);

    CFRelease (event);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::MacOSXWindowManager::SetCursorPosition (const Point&)");
    throw;
  }
}

syslib::Point MacOSXWindowManager::GetCursorPosition ()
{
  throw xtl::make_not_implemented_exception ("syslib::MacOSXWindowManager::GetParentWindowHandle");
}

void MacOSXWindowManager::SetCursorPosition (window_t handle, const Point& client_position)
{
  NSWindow* window = (NSWindow*)handle;

  Point new_cursor_position = client_position;

  //translate to point coordinate system
  new_cursor_position.x /= window.backingScaleFactor;
  new_cursor_position.y /= window.backingScaleFactor;

  CGRect frame = window.frame;

  if (new_cursor_position.x > frame.size.width)
    new_cursor_position.x = frame.size.width;
  if (new_cursor_position.y > frame.size.height)
    new_cursor_position.y = frame.size.height;

  //TODO support for multiple screens
  unsigned int screen_height = NSScreen.mainScreen.visibleFrame.size.height;

  new_cursor_position.x += frame.origin.x;
  new_cursor_position.y = (screen_height - frame.origin.y) - (frame.size.height - new_cursor_position.y);

  //translate to pixel coordinate system
  new_cursor_position.x *= window.backingScaleFactor;
  new_cursor_position.y *= window.backingScaleFactor;

  try
  {
    SetCursorPosition (new_cursor_position);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetCursorPosition (window_t, const Point&)");
    throw;
  }
}

syslib::Point MacOSXWindowManager::GetCursorPosition (window_t handle)
{
  NSWindow* window   = (NSWindow*)handle;
  NSPoint   position = [window mouseLocationOutsideOfEventStream];

  float x = position.x * window.backingScaleFactor,
        y = (window.frame.size.height - position.y) * window.backingScaleFactor;

  return syslib::Point (x > 0 ? x : 0, y > 0 ? y : 0);
}

/*
    Cursor visibility
*/

void MacOSXWindowManager::SetCursorVisible (window_t handle, bool state)
{
  NSWindowWrapper* window = (NSWindowWrapper*)handle;

  if (window.is_cursor_visible == state)
    return;

  try
  {
    if (GetWindowFlag (handle, WindowFlag_Active))
    {
      //update cursor state if inside window
      if (is_cursor_in_client_region (window))
      {
        if (state)
        {
          if (!CGCursorIsVisible ())
            check_quartz_error (CGDisplayShowCursor (kCGDirectMainDisplay), "::CGDisplayShowCursor", "Can't show cursor");
        }
        else
        {
          if (CGCursorIsVisible ())
            check_quartz_error (CGDisplayHideCursor (kCGDirectMainDisplay), "::CGDisplayHideCursor", "Can't hide cursor");
        }
      }
    }

    window.is_cursor_visible = state;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::CarbonWindowManager::SetCursorVisible");
    throw;
  }
}

bool MacOSXWindowManager::GetCursorVisible (window_t handle)
{
  return ((NSWindowWrapper*)handle).is_cursor_visible;
}

/*
    Cursor with image
*/

cursor_t MacOSXWindowManager::CreateCursor (const char* file_name, int hotspot_x, int hotspot_y)
{
  try
  {
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");

    common::InputFile image_file (file_name);

    size_t bytes_to_read = image_file.Size ();

    NSMutableData* image_data = [[NSMutableData alloc] initWithLength:bytes_to_read];

    if (!image_data)
      throw xtl::format_operation_exception ("[NSMutableData initWithLength:]", "Can't create mutable data with size %u", bytes_to_read);

    char* read_buffer = (char*)[image_data mutableBytes];

    try
    {
      while (bytes_to_read)
      {
        size_t readed_bytes = image_file.Read (read_buffer, bytes_to_read);

        bytes_to_read -= readed_bytes;
        read_buffer   += readed_bytes;
      }
    }
    catch (...)
    {
      [image_data release];
      throw;
    }

    NSImage* cursor_image = [[NSImage alloc] initWithData:image_data];

    [image_data release];

    if (!cursor_image)
      throw xtl::format_operation_exception ("[NSImage initWithData:]", "Can't create image");

    NSPoint cursor_hot_spot;

    cursor_hot_spot.x = hotspot_x == -1 ? 0 : hotspot_x;
    cursor_hot_spot.y = hotspot_y == -1 ? 0 : hotspot_y;

    NSCursor *cursor = [[NSCursor alloc] initWithImage:cursor_image hotSpot:cursor_hot_spot];

    [cursor_image release];

    if (!cursor)
      throw xtl::format_operation_exception ("[NSCursor initWithImage:hotSpot:]", "Can't create cursor");

    return reinterpret_cast<cursor_t> (cursor);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::MacOSXWindowManager::CreateCursor");
    throw;
  }
}

void MacOSXWindowManager::DestroyCursor (cursor_t cursor)
{
  [(NSCursor*)cursor release];
}

void MacOSXWindowManager::SetCursor (window_t handle, cursor_t cursor)
{
  NSWindowWrapper* window = (NSWindowWrapper*)handle;

  window.cursor = (NSCursor*)cursor ? (NSCursor*)cursor : [NSCursor arrowCursor];

  if (window.is_cursor_visible && is_cursor_in_client_region (window))
    [window.cursor set];
}

/*
   Set/change multitouch mode
*/

void MacOSXWindowManager::SetMultitouchEnabled (window_t handle, bool state)
{
  ((NSWindowWrapper*)handle).is_multitouch_enabled = state;
}

bool MacOSXWindowManager::IsMultitouchEnabled (window_t handle)
{
  return ((NSWindowWrapper*)handle).is_multitouch_enabled;
}

/*
   Background color
*/

void MacOSXWindowManager::SetBackgroundColor (window_t handle, const Color& color)
{
  NSWindow* window = (NSWindow*)handle;

  window.backgroundColor = [NSColor colorWithCalibratedRed:color.red / 255.f
                                                     green:color.green / 255.f
                                                      blue:color.blue / 255.f
                                                     alpha:1.f];
}

void MacOSXWindowManager::SetBackgroundState (window_t window, bool state)
{
  if (GetBackgroundState (window) == state)
    return;

  Color color;

  memset (&color, 0, sizeof (color));

  if (state)
    color = GetBackgroundColor (window);

  SetBackgroundColor (window, color);

  ((NSWindowWrapper*)window).background_state = state;
}

Color MacOSXWindowManager::GetBackgroundColor (window_t handle)
{
  NSWindow* window           = (NSWindow*)handle;
  NSColor*  background_color = window.backgroundColor;

  Color return_value;

  if (background_color)
  {
    background_color = [background_color colorUsingColorSpace:[NSColorSpace deviceRGBColorSpace]];

    if (background_color)
    {
      return_value.red   = background_color.redComponent * 255;
      return_value.green = background_color.greenComponent * 255;
      return_value.blue  = background_color.blueComponent * 255;
    }
  }

  if (!background_color)
    memset (&return_value, 0, sizeof (return_value));

  return return_value;
}

bool MacOSXWindowManager::GetBackgroundState (window_t handle)
{
  return ((NSWindowWrapper*)handle).background_state;
}
