#include "shared.h"

using namespace syslib;
using namespace syslib::x11;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_WINDOW_X       = 0;    //координата X по умолчанию при создании окна
const size_t DEFAULT_WINDOW_Y       = 0;    //координата Y по умолчанию при создании окна
const size_t DEFAULT_WINDOW_WIDTH   = 100;  //ширина по умолчанию при создании окна
const size_t DEFAULT_WINDOW_HEIGHT  = 100;  //высота по умолчанию при создании окна
const size_t EVENT_WAIT_TIMEOUT_SEC = 1;    //тайм-аут при ожидании сообщения

/*
    Преобразование виртуальных кодов клавиш в syslib::Key
*/

Key VirtualKey2SystemKey (KeySym vkey)
{
  switch (vkey)
  {
    case XK_BackSpace:    return Key_BackSpace;
    case XK_KP_Tab:
    case XK_Tab:          return Key_Tab;
    case XK_KP_Enter:
    case XK_Return:       return Key_Enter;
    case XK_Pause:        return Key_Pause;
    case XK_Scroll_Lock:  return Key_ScrollLock;
    case XK_Escape:       return Key_Escape;
    case XK_KP_Delete:
    case XK_Delete:       return Key_Delete;
    case XK_KP_Home:
    case XK_Home:         return Key_Home;
    case XK_KP_Left:
    case XK_Left:         return Key_Left;
    case XK_KP_Up:    
    case XK_Up:           return Key_Up;
    case XK_KP_Right:
    case XK_Right:        return Key_Right;
    case XK_KP_Down:
    case XK_Down:         return Key_Down;
    case XK_KP_Page_Up:
    case XK_Page_Up:      return Key_PageUp;
    case XK_KP_Page_Down:
    case XK_Page_Down:    return Key_PageDown;
    case XK_KP_End:
    case XK_End:          return Key_End;
    case XK_KP_Insert:
    case XK_Insert:       return Key_Insert;
    case XK_Num_Lock:     return Key_NumLock;
    case XK_KP_Multiply:  return Key_NumPadMultiply;
    case XK_KP_Add:       return Key_NumPadPlus;
    case XK_KP_Subtract:  return Key_NumPadMinus;
    case XK_KP_Decimal:   return Key_NumPadDot;
    case XK_KP_Divide:    return Key_NumPadDivide;
    case XK_KP_0:         return Key_NumPad0;
    case XK_KP_1:         return Key_NumPad1;
    case XK_KP_2:         return Key_NumPad2;
    case XK_KP_3:         return Key_NumPad3;
    case XK_KP_4:         return Key_NumPad4;
    case XK_KP_5:         return Key_NumPad5;
    case XK_KP_6:         return Key_NumPad6;
    case XK_KP_7:         return Key_NumPad7;
    case XK_KP_8:         return Key_NumPad8;
    case XK_KP_9:         return Key_NumPad9;
    case XK_F1:           return Key_F1;
    case XK_F2:           return Key_F2;
    case XK_F3:           return Key_F3;
    case XK_F4:           return Key_F4;
    case XK_F5:           return Key_F5;
    case XK_F6:           return Key_F6;
    case XK_F7:           return Key_F7;
    case XK_F8:           return Key_F8;
    case XK_F9:           return Key_F9;
    case XK_F10:          return Key_F11;
    case XK_F11:          return Key_F12;
    case XK_F12:          return Key_F12;
    case XK_Shift_R:
    case XK_Shift_L:      return Key_Shift;
    case XK_Control_R:
    case XK_Control_L:    return Key_Control;
    case XK_Alt_R:
    case XK_Alt_L:        return Key_Alt;
    case XK_Caps_Lock:    return Key_CapsLock;
    case XK_KP_Separator:
    case XK_comma:        return Key_Comma;
    case XK_plus:         return Key_Plus;
    case XK_minus:        return Key_Minus;
    case XK_period:       return Key_Dot;
    case XK_slash:        return Key_Slash;
    case XK_backslash:    return Key_BackSlash;
    case XK_apostrophe:   return Key_Apostrophe;
    case XK_KP_Space:
    case XK_space:        return Key_Space;
    case XK_colon:
    case XK_semicolon:    return Key_Semicolon;
    case XK_bracketleft:  return Key_LeftBracket;
    case XK_bracketright: return Key_RightBracket;
    case XK_asciitilde:   return Key_Tilda;
    case XK_0:
    case XK_1:
    case XK_2:
    case XK_3:
    case XK_4:
    case XK_5:
    case XK_6:
    case XK_7:
    case XK_8:
    case XK_9:            return (Key)(vkey - XK_0 + Key_0);
    case XK_a: case XK_b: case XK_c: case XK_d: case XK_e: case XK_f: case XK_g: case XK_h: case XK_i: case XK_j: case XK_k: case XK_l:
    case XK_m: case XK_n: case XK_o: case XK_p: case XK_q: case XK_r: case XK_s: case XK_t: case XK_u: case XK_v: case XK_w: case XK_x:
    case XK_y: case XK_z:
      return (Key)(vkey - XK_a + Key_A);
    case XK_A: case XK_B: case XK_C: case XK_D: case XK_E: case XK_F: case XK_G: case XK_H: case XK_I: case XK_J: case XK_K: case XK_L:
    case XK_M: case XK_N: case XK_O: case XK_P: case XK_Q: case XK_R: case XK_S: case XK_T: case XK_U: case XK_V: case XK_W: case XK_X:
    case XK_Y: case XK_Z:
      return (Key)(vkey - XK_A + Key_A);
    default:
      return Key_Unknown;
  }
}

}

/*
    Описание реализации окна
*/

typedef ::Window XWindow;

struct Platform::window_handle
{
  Mutex                          display_mutex;    //мьютекс доступа к соединению
  Display*                       display;          //дисплей для данного окна
  XWindow                        window;           //дескриптор окна
  bool                           background_state; //ложное свойство - состояние фона
  stl::auto_ptr<Thread>          events_thread;    //нить обработки событий окна
  MessageQueue&                  message_queue;    //очередь событий
  Platform::WindowMessageHandler message_handler;  //функция обработки сообщений окна
  void*                          user_data;        //пользовательские данные для функции обратного вызова
  
///Конструктор
  window_handle (Platform::WindowMessageHandler in_message_handler, void* in_user_data)
    : display (0)
    , window (0)
    , background_state (true)
    , message_queue (*MessageQueueSingleton::Instance ())
    , message_handler (in_message_handler)
    , user_data (in_user_data)
  {
    message_queue.RegisterHandler (this);
  }
  
///Деструктор
  ~window_handle ()
  {
    try
    {
      message_queue.UnregisterHandler (this);
    }
    catch (...)
    {
    }
  }
  
///Получение контекста события
  void GetEventContext (WindowEventContext& context)
  {
    memset (&context, 0, sizeof (context));

    context.handle = this;

/*    GetClientRect (wnd, &client_rect);
   
    POINT cursor_position;

    GetCursorPos (&cursor_position);
    ScreenToClient (wnd, &cursor_position);

    if (cursor_position.x < client_rect.left)   cursor_position.x = client_rect.left;
    if (cursor_position.y < client_rect.top)    cursor_position.y = client_rect.top;
    if (cursor_position.x > client_rect.right)  cursor_position.x = client_rect.right;
    if (cursor_position.y > client_rect.bottom) cursor_position.y = client_rect.bottom;

    context.cursor_position.x = cursor_position.x;
    context.cursor_position.y = cursor_position.y;
*/
  }
  
///Установка состояния клавиш
  void GetKeysState (unsigned int state, WindowEventContext& context)
  {
    context.keyboard_alt_pressed        = (state & LockMask) != 0;
    context.keyboard_control_pressed    = (state & ControlMask) != 0;
    context.keyboard_shift_pressed      = (state & ShiftMask) != 0;
    context.mouse_left_button_pressed   = (state & Button1Mask) != 0;
    context.mouse_right_button_pressed  = (state & Button3Mask) != 0;
    context.mouse_middle_button_pressed = (state & Button2Mask) != 0;
  }
  
///Отложенное сообщение
  struct Message: public MessageQueue::Message
  {
    WindowEvent        event;
    WindowEventContext context;
    window_handle&     sender;
    
    Message (window_handle& in_sender) : sender (in_sender) {}
    
    void Dispatch ()
    {
      sender.NotifyCore (event, context);
    }
  };
  
///Функция нити обработки событий окна
  int EventsThreadRoutine ()
  {
      //получение файлового дескриптора соединения с дисплеем
      
    int display_connection_fd = 0;
    
    {
      Lock lock (display_mutex);
      
      display_connection_fd = ConnectionNumber (display);
    }
    
      //настройка тайм-аута
    
    timeval timeout;
    
    timeout.tv_usec = 0;
    timeout.tv_sec  = EVENT_WAIT_TIMEOUT_SEC;

      //цикл обработки событий
      
    XEvent event;
      
    for (;;)
    {
        //ожидание появления события
      
      fd_set in_fds;
      
      FD_ZERO (&in_fds);
      FD_SET  (display_connection_fd, &in_fds);

      if (!select (display_connection_fd + 1, &in_fds, 0, 0, &timeout))
        continue; //событие не пришло

        //обработка поступивших событий
        
      Lock lock (display_mutex);
        
      while (XPending (display))
      {
        XNextEvent (display, &event);
        
        xtl::intrusive_ptr<Message> message (new Message (*this));
        
        WindowEventContext& context = message->context;
        
        GetEventContext (context);
        
        switch (event.type)
        {
          case Expose:
            Notify (WindowEvent_OnPaint, message);
            break;
          case KeyPress:
          {
            GetKeysState (event.xkey.state, context);
            
            context.cursor_position = Point (event.xkey.x, event.xkey.y);
            context.key_scan_code   = (ScanCode)event.xkey.keycode;
            context.key             = VirtualKey2SystemKey (XKeycodeToKeysym (display, event.xkey.keycode, 0));
            
            Notify (WindowEvent_OnKeyDown, message);
            
            break;
          }
          case KeyRelease:
          {
            GetKeysState (event.xkey.state, context);
            
            context.cursor_position = Point (event.xkey.x, event.xkey.y);
            context.key_scan_code   = (ScanCode)event.xkey.keycode;
            context.key             = VirtualKey2SystemKey (XKeycodeToKeysym (display, event.xkey.keycode, 0));
            
            Notify (WindowEvent_OnKeyUp, message);
            
            break;
          }
          case ButtonPress:
          {
            context.cursor_position = Point (event.xbutton.x, event.xbutton.y);            
            
            GetKeysState (event.xbutton.state, context);
            
            switch (event.xbutton.button)
            {
              case Button1: Notify (WindowEvent_OnLeftButtonDown, message); break;
              case Button2: Notify (WindowEvent_OnMiddleButtonDown, message); break;
              case Button3: Notify (WindowEvent_OnRightButtonDown, message); break;
              case Button4: Notify (WindowEvent_OnXButton1Down, message); break;
              case Button5: Notify (WindowEvent_OnXButton2Down, message); break;
              default: break;
            }
                        
            break;          
          }
          case ButtonRelease:
          {
            context.cursor_position = Point (event.xbutton.x, event.xbutton.y);            
            
            GetKeysState (event.xbutton.state, context);
            
            switch (event.xbutton.button)
            {
              case Button1: Notify (WindowEvent_OnLeftButtonUp, message); break;
              case Button2: Notify (WindowEvent_OnMiddleButtonUp, message); break;
              case Button3: Notify (WindowEvent_OnRightButtonUp, message); break;
              case Button4: Notify (WindowEvent_OnXButton1Up, message); break;
              case Button5: Notify (WindowEvent_OnXButton2Up, message); break;
              default: break;
            }

            break;          
          }
          case MotionNotify:
          {
            context.cursor_position = Point (event.xmotion.x, event.xmotion.y);
            
            GetKeysState (event.xmotion.state, context);
            
            Notify (WindowEvent_OnMouseMove, message);
            
            break;
          }
          case EnterNotify:
//            Notify (WindowEvent_OnMouseEnter, message);
            break;          
          case LeaveNotify:
            Notify (WindowEvent_OnMouseLeave, message);          
            break;          
          case FocusIn:
            Notify (WindowEvent_OnSetFocus, message);
            break;          
          case FocusOut:
            Notify (WindowEvent_OnLostFocus, message);
            break;          
          case CreateNotify:
            Notify (WindowEvent_OnChangeHandle, message);
            break;          
          case DestroyNotify:
            Notify (WindowEvent_OnDestroy, message);
            break;          
          case UnmapNotify:
            Notify (WindowEvent_OnHide, message);          
            break;          
          case MapNotify:
            Notify (WindowEvent_OnShow, message);          
            break;          
          case ReparentNotify:
            break;          
          case ConfigureNotify:
            break;          
          case ResizeRequest:
            Notify (WindowEvent_OnSize, message);          
            break;          
        } 
      }   
    }     

    return 0; 
  }
  
///Оповещение о возникновении события  
  void Notify (WindowEvent event, const xtl::intrusive_ptr<Message>& message)
  {
    message->event = event;
    
    message_queue.PushMessage (this, message);
  }

  void NotifyCore (WindowEvent event, const WindowEventContext& context)
  {
    try
    {
      message_handler (this, event, context, user_data);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }  
};

/*
    Создание/закрытие/уничтожение окна
*/

Platform::window_t Platform::CreateWindow (WindowStyle style, WindowMessageHandler handler, window_t parent, const char* init_string, void* user_data)
{
  try
  {
      //определение стиля окна
      
    switch (style)
    {
      case WindowStyle_Overlapped:
      case WindowStyle_PopUp: //not implemented
        break;
      default:
        throw xtl::make_argument_exception ("", "style", style);
    }
    
      //создание дескриптора окна

    stl::auto_ptr<window_handle> impl (new window_handle (handler, user_data));

      //открытие соединения

    common::PropertyMap properties = common::parse_init_string (init_string);

    const char* display_name = properties.IsPresent ("display") ? properties.GetString ("display") : 0;

    impl->display = XOpenDisplay (display_name);

    if (!impl->display)
      throw xtl::format_operation_exception ("", "Can't open display '%s'", XDisplayName (display_name));
      
      //настройка соединения

    bool synchronize = properties.IsPresent ("synchronize") && properties.GetInteger ("synchronize") != 0;
    
    if (synchronize)
      XSynchronize (impl->display, true);          
      
      //создание окна

    int blackColor = BlackPixel (impl->display, DefaultScreen (impl->display));
    
    XWindow parent_window = 0;
    
    if (parent) parent_window = parent->window;
    else        parent_window = DefaultRootWindow (impl->display);
    
    impl->window = XCreateSimpleWindow (impl->display, parent_window, DEFAULT_WINDOW_X, DEFAULT_WINDOW_Y,
      DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, 0, blackColor, blackColor);

    if (!impl->window)
      throw xtl::format_operation_exception ("", "Can't create window for display '%s'", XDisplayName (display_name));
      
      //настройка получения событий

    XSelectInput (impl->display, impl->window, StructureNotifyMask | ExposureMask | ButtonPressMask | KeyPressMask | KeyReleaseMask);  
    
      //запуск нити обработки событий
      
    Lock lock (impl->display_mutex); 
    
    impl->events_thread = stl::auto_ptr<Thread> (new Thread (xtl::bind (&window_handle::EventsThreadRoutine, &*impl)));
    
    XFlush (impl->display);    

    return impl.release (); 
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::CreateWindow");
    throw;  
  }
}

void Platform::CloseWindow (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);
    
    XEvent e;
     
    memset (&e, 0, sizeof (e));
     
    e.xclient.type         = ClientMessage;
    e.xclient.window       = handle->window;
    e.xclient.message_type = XInternAtom (handle->display, "WM_PROTOCOLS", true);
    e.xclient.format       = 32;
    e.xclient.data.l [0]   = XInternAtom (handle->display, "WM_DELETE_WINDOW", false);
    e.xclient.data.l [1]   = CurrentTime;

    if (!XSendEvent (handle->display, handle->window, False, NoEventMask, &e))
      throw xtl::format_operation_exception ("", "XSendEvent failed");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::CloseWindow");
    throw;  
  }
}

void Platform::DestroyWindow (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    {
      Lock lock (handle->display_mutex);        
    
      if (!XDestroyWindow (handle->display, handle->window))
        throw xtl::format_operation_exception ("", "XDestroyWindow failed");
      
      if (!XCloseDisplay (handle->display))
        throw xtl::format_operation_exception ("", "XCloseDisplay failed");
    }
      
    handle->events_thread->Join ();
    
    delete handle;
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::DestroyWindow");
    throw;  
  }
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* Platform::GetNativeWindowHandle (window_t handle)
{
  return reinterpret_cast<const void*> (handle->window);
}

/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t handle, const wchar_t* title)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);      
    
    XStoreName (handle->display, handle->window, common::tostring (title).c_str ());
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetWindowTitle");
    throw;  
  }
}

void Platform::GetWindowTitle (window_t handle, size_t buffer_size_in_chars, wchar_t* buffer)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);      
    
    char* window_name = 0;
    
    if (!XFetchName (handle->display, handle->window, &window_name) || !window_name)
      throw xtl::format_operation_exception ("", "XFetchName failed");
      
    stl::wstring result = common::towstring (window_name);
    
    xtl::xstrncpy (buffer, result.c_str (), buffer_size_in_chars);
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetWindowTitle");
    throw;  
  }
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t handle, const Rect& rect)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);      
    
    XWindowChanges changes;
    
    memset (&changes, 0, sizeof (XWindowChanges));        
    
    changes.x      = (int)rect.left;
    changes.y      = (int)rect.top;
    changes.width  = rect.right - rect.left;
    changes.height = rect.bottom - rect.top;
    
    if (!XConfigureWindow (handle->display, handle->window, CWX | CWY | CWWidth | CWHeight, &changes))
      throw xtl::format_operation_exception ("", "XConfigureWindow failed");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetWindowRect");
    throw;  
  }
}

void Platform::SetClientRect (window_t handle, const Rect& rect)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);    
    
    throw xtl::make_not_implemented_exception ("");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetClientRect");
    throw;  
  }
}

void Platform::GetWindowRect (window_t handle, Rect& rect)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);          
      
    XWindow root_return = 0;
    int x_return = 0, y_return = 0;
    unsigned int width_return = 0, height_return = 0, border_width_return = 0, depth_return = 0;
    
    if (!XGetGeometry (handle->display, handle->window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return))
      throw xtl::format_operation_exception ("", "XGetGeometry failed");
      
    rect.left   = (size_t)x_return;
    rect.top    = (size_t)y_return;
    rect.right  = rect.left + width_return;
    rect.bottom = rect.top + height_return;
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetWindowRect");
    throw;  
  }
}

void Platform::GetClientRect (window_t handle, Rect& rect)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);
    
    throw xtl::make_not_implemented_exception ("");    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetClientRect");
    throw;  
  }
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);          
    
    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
      {
        if (!XMapWindow (handle->display, handle->window))
          throw xtl::format_operation_exception ("", "XMapWindow failed");
          
        XFlush (handle->display);

        break;
      }
      case WindowFlag_Active: //активность окна
        break;
      case WindowFlag_Focus: //фокус ввода
        break;
      case WindowFlag_Maximized:
        break;
      case WindowFlag_Minimized:            
        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetWindowFlag");
    throw;  
  }
}

bool Platform::GetWindowFlag (window_t handle, WindowFlag flag)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);          
    
    return false;    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetWindowFlag");
    throw;  
  }
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindow (window_t child, window_t parent)
{
  try
  {
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetParentWindow");
    throw;  
  }
}

Platform::window_t Platform::GetParentWindow (window_t child)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
    
    if (!child)
      throw xtl::make_null_argument_exception ("", "child");
      
    XWindow root_return = 0, parent_return = 0, *children_return = 0;
    unsigned int nchildren_return = 0;
    
    if (!XQueryTree (child->display, child->window, &root_return, &parent_return, &children_return, &nchildren_return))
      throw xtl::format_operation_exception ("", "XQueryTree failed");
    
//    return ????; 
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetParentWindow");
    throw;  
  }
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t handle)
{
  try
  {    
    //????
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::InvalidateWindow");
    throw;  
  }
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point& position)
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursorPosition(const Point&)");
    throw;  
  }
}

syslib::Point Platform::GetCursorPosition ()
{
  try
  {
    throw xtl::make_not_implemented_exception ("");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetCursorPosition()");
    throw;  
  }
}

void Platform::SetCursorPosition (window_t handle, const Point& client_position)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);          
    
    throw xtl::make_not_implemented_exception ("");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursorPosition(window_t,const Point&)");
    throw;  
  }
}

syslib::Point Platform::GetCursorPosition (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);          
    
    throw xtl::make_not_implemented_exception ("");
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetCursorPosition(window_t)");
    throw;  
  }
}

/*
    Видимость курсора
*/

void Platform::SetCursorVisible (window_t handle, bool state)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);          
    
    throw xtl::make_not_implemented_exception ("");
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursorVisible");
    throw;  
  }
}

bool Platform::GetCursorVisible (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);          
    
    throw xtl::make_not_implemented_exception (""); 
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetCursorVisible");
    throw;  
  }  
}

/*
    Изображение курсора
*/

Platform::cursor_t Platform::CreateCursor (const char* file_name, int hotspot_x, int hotspot_y)
{
  try
  {    
    throw xtl::make_not_implemented_exception ("");    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::X11Platform::CreateCursor");
    throw;
  }
}

void Platform::DestroyCursor (cursor_t cursor)
{
  try
  {    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::X11Platform::DestroyCursor");
    throw;
  }
}

void Platform::SetCursor (window_t handle, cursor_t cursor)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);    
    
    throw xtl::make_not_implemented_exception ("");    
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetCursor");
    throw;
  }
}

/*
    Цвет фона
*/

void Platform::SetBackgroundColor (window_t handle, const Color& color)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    Lock lock (handle->display_mutex);
    
    throw xtl::make_not_implemented_exception ("");
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetBackgroundColor");
    throw;
  }
}

void Platform::SetBackgroundState (window_t handle, bool state)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");    
      
    handle->background_state = state;
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::SetBackgroundState");
    throw;
  }
}

Color Platform::GetBackgroundColor (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");    
      
    Lock lock (handle->display_mutex); 
    
    return Color (0, 0, 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetBackgroundColor");
    throw;
  }
}

bool Platform::GetBackgroundState (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");    
      
    return handle->background_state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::X11Platform::GetBackgroundState");
    throw;
  }
}
