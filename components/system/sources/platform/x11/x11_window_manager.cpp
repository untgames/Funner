#include "shared.h"

using namespace syslib;
using namespace syslib::x11;

namespace
{

/*
    Константы
*/

const size_t DEFAULT_WINDOW_X       = 0;            //координата X по умолчанию при создании окна
const size_t DEFAULT_WINDOW_Y       = 0;            //координата Y по умолчанию при создании окна
const size_t DEFAULT_WINDOW_WIDTH   = 100;          //ширина по умолчанию при создании окна
const size_t DEFAULT_WINDOW_HEIGHT  = 100;          //высота по умолчанию при создании окна
const size_t EVENT_WAIT_TIMEOUT_SEC = 1;            //тайм-аут при ожидании сообщения
const char*  LOG_NAME               = "system.x11"; //имя канала протоколирования

typedef ::Window XWindow;

/*
    Блокировка соединения X11
*/

class DisplayLock
{
  public:
    DisplayLock (Display* in_display) : display (in_display)
    {
      XLockDisplay (display);
    }

    ~DisplayLock ()
    {
      XUnlockDisplay (display);
    }

  private:
    Display* display;
};

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
    case XK_F10:          return Key_F10;
    case XK_F11:          return Key_F11;
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

/*
    Интерфейс диспетчеризации оконных сообщений
*/

class IWindowMessageHandler
{
  public:
    virtual void ProcessEvent (const XEvent& event) = 0;
  
  protected:
    virtual ~IWindowMessageHandler () {}
};

/*
    Менеджер соединения с дисплеем
*/

class DisplayManagerImpl
{
  public:
///Конструктор
    DisplayManagerImpl ()
      : log (LOG_NAME)
      , display (0)
      , sync_mode (false)
      , event_loop (true)
    {
    }
    
///Деструктор
    ~DisplayManagerImpl ()
    {       
      try
      {      
        if (display)
        {
          log.Printf ("Stopping X11 event processing thread...");
          
          event_loop = false;

          if (events_thread)
            events_thread->Join ();

          {
            log.Printf ("Close X11 display connection");            
            
            DisplayLock lock (display);                                              
          
            XCloseDisplay (display);
          }            
        }
      }            
      catch (...)
      {
      }
    }
    
///Ссылка на дисплей
    void SetDisplayString (const char* in_display_string)
    {
      static const char* METHOD_NAME = "syslib::x11::DisplayManagerImpl::SetDisplay";

      if (!in_display_string)
        throw xtl::make_null_argument_exception (METHOD_NAME, "display_string");

      if (display)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't change display string after display has created");

      display_string = in_display_string;
    }

    const char* DisplayString ()
    {
      return display_string.c_str ();
    }
    
///Необходимость синхронизации
    void SetSyncMode (bool state)
    {
      if (sync_mode == state)
        return;
      
      if (display)
      {
        log.Printf (state ? "Switch X11 display connection to synchronous state" : "Switch X11 display connection to asynchronous state");
        
        XSynchronize (display, state);
      }

      sync_mode = state;
    }

    bool IsSyncMode ()
    {
      return sync_mode;
    }
    
///Получение дисплея
    ::Display* Display ()
    {
      try
      {
        if (display)
          return display;

        const char* display_name = display_string.empty () ? (const char*)0 : display_string.c_str ();
        
        log.Printf ("Open X11 display connection with '%s'", XDisplayName (display_name));
        
        log.Printf ("...initialize X11 for using with threads");
        
        if (!XInitThreads ())
        {
          log.Printf ("...XInitThreads failed!");
          
          throw xtl::format_operation_exception ("", "Can't initialize threads for display '%s'", XDisplayName (display_name));
        }        

        display = XOpenDisplay (display_name);

        if (!display)
          throw xtl::format_operation_exception ("", "Can't open display '%s'", XDisplayName (display_name));          
          
        log.Printf ("...X11 display connection opened (display-string='%s', server='%s' release %d)",
          XDisplayString (display), XServerVendor (display), XVendorRelease (display));

        if (sync_mode)
        {
          log.Printf ("...switch X11 display connection to synchronous state");
          
          XSynchronize (display, sync_mode);
        }
                  
          //запуск нити обработки событий
          
        log.Printf ("...starting X11 events processing thread");

        events_thread = stl::auto_ptr<Thread> (new Thread (xtl::bind (&DisplayManagerImpl::EventsThreadRoutine, this)));

        return display;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::x11::DisplayManagerImpl::Display");
        throw;
      }
    }
    
///Регистрация окна
    void RegisterWindow (XWindow window, IWindowMessageHandler* handler)
    {
      handlers [window] = handler;
    }
    
    void UnregisterWindow (XWindow window)
    {
      handlers.erase (window);
    }
    
  private:
///Функция нити обработки событий окна
    int EventsThreadRoutine ()
    {
      log.Printf ("X11 events thread started");      
      
        //получение файлового дескриптора соединения с дисплеем
        
      int display_connection_fd = 0;
      
      {
        DisplayLock lock (display);
        
        display_connection_fd = ConnectionNumber (display);
      }
      
        //цикл обработки событий
        
      XEvent event;
        
      while (event_loop)
      {
          //ожидание появления события
        
        fd_set in_fds;
        
        FD_ZERO (&in_fds);
        FD_SET  (display_connection_fd, &in_fds);

          //настройка тайм-аута

        timeval timeout;

        timeout.tv_usec = 0;
        timeout.tv_sec  = EVENT_WAIT_TIMEOUT_SEC;

        if (!select (display_connection_fd + 1, &in_fds, 0, 0, &timeout))
          continue; //событие не пришло
          
          //обработка поступивших событий
          
        DisplayLock lock (display);
          
        while (XPending (display))
        {
            //извлечение события из очереди
          
          XNextEvent (display, &event);
          
            //поиск соответствующего обработчика
            
          WindowHandlerMap::iterator iter = handlers.find (reinterpret_cast<XAnyEvent&> (event).window);
          
          if (iter == handlers.end ())
            continue;
            
            //диспетчеризация сообщения

          try
          {
            iter->second->ProcessEvent (event);
          }
          catch (std::exception& e)
          {
            log.Printf ("%s\n    at syslib::x11::IWindowMessageHandler::Dispatch\n    at syslib::x11::DisplayManagerImpl::EventsThreadRoutine", e.what ());
          }
          catch (...)
          {
            log.Printf ("unknown exception\n    at syslib::x11::IWindowMessageHandler::Dispatch\n    at syslib::x11::DisplayManagerImpl::EventsThreadRoutine");
          }
        }   
      }     
      
      log.Printf ("Exit from X11 events thread");
      
      return 0; 
    }  

  private:
    DisplayManagerImpl (const DisplayManagerImpl&); //no impl
    DisplayManagerImpl& operator = (const DisplayManagerImpl&); //no impl
    
    typedef stl::hash_map<XWindow, IWindowMessageHandler*> WindowHandlerMap;

  private:
    common::Log           log;            //канал протоколирования
    stl::string           display_string; //строка инициализации дисплея
    ::Display*            display;        //ссылка на дисплей
    bool                  sync_mode;      //находится ли соединение в режиме синхронизации
    stl::auto_ptr<Thread> events_thread;  //нить обработки событий окна
    bool                  event_loop;     //класс находится в состоянии обработки событий
    WindowHandlerMap      handlers;       //отображение системных окон на внутренние структуры обработки сообщений
};

typedef common::Singleton<DisplayManagerImpl> DisplayManagerSingleton;

}

/*
===================================================================================================
    Window
===================================================================================================
*/

/*
    Описание реализации курсора
*/

struct syslib::cursor_handle
{
  Display* display; //ссылка на соединение с дисплеем
  Cursor   cursor;  //ссылка на X11 курсор

  cursor_handle (Display* in_display)
    : display (in_display)
    , cursor (0)
  { }
};

/*
    Описание реализации окна
*/

struct syslib::window_handle: public IWindowMessageHandler, public MessageQueue::Handler
{
  Display*             display;               //дисплей для данного окна
  XWindow              window;                //дескриптор окна
  bool                 is_destroyed;          //удалено ли окно
  bool                 background_state;      //ложное свойство - состояние фона
  Point                last_cursor_position;  //последнее положение курсора, для которого было отправлено оповещение, для фильтрации дублирующих оповещений
  Rect                 window_rect;           //область окна
  bool                 window_rect_init;      //инициализирована ли область окна
  Cursor               invisible_cursor;      //невидимый курсор
  bool                 is_cursor_visible;     //видим ли курсор
  cursor_t             active_cursor;         //активный курсор окна
  syslib::Color        background_color;      //цвет заднего фона
  MessageQueue&        message_queue;         //очередь событий
  WindowMessageHandler message_handler;       //функция обработки сообщений окна
  void*                user_data;             //пользовательские данные для функции обратного вызова
  bool                 is_multitouch_enabled; //включен ли multi touch
  bool                 is_fullscreen;         //находится ли окно в fullscreen режиме
  Atom                 wm_delete;             //атом свойства WM_DELETE_WINDOW
  
///Конструктор
  window_handle (WindowMessageHandler in_message_handler, void* in_user_data)
    : display (DisplayManagerSingleton::Instance ()->Display ())
    , window (0)
    , is_destroyed (false)
    , background_state (true)
    , window_rect_init (false)
    , invisible_cursor (0)
    , is_cursor_visible (true)
    , active_cursor (0)
    , message_queue (*MessageQueueSingleton::Instance ())
    , message_handler (in_message_handler)
    , user_data (in_user_data)
    , is_multitouch_enabled (false)
    , is_fullscreen (false)
    , wm_delete (0)
  {
    message_queue.RegisterHandler (*this);
  }
  
///Деструктор
  ~window_handle ()
  {
    try
    {
      DisplayManagerSingleton::Instance ()->UnregisterWindow (window);
      message_queue.UnregisterHandler (*this);
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
  }
  
///Установка состояния клавиш
  void GetKeysState (unsigned int state, WindowEventContext& context)
  {
    context.keyboard_alt_pressed        = false;
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
  
  struct DestroyMessage: public Message
  {
    DestroyMessage (window_handle& in_sender) : Message (in_sender) {}
    
    void Dispatch ()
    {
      Message::Dispatch ();

      delete &sender;
    }
  };

  void NotifyMouseMove (const xtl::intrusive_ptr<Message>& message)
  {
    if (last_cursor_position.x == message->context.cursor_position.x && last_cursor_position.y == message->context.cursor_position.y)
      return;

    last_cursor_position = message->context.cursor_position;

    Notify (WindowEvent_OnMouseMove, message);
  }
  
///Обработчик события
  void ProcessEvent (const XEvent& event)
  {
    xtl::intrusive_ptr<Message> message (new Message (*this), false);
    
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
        context.key             = VirtualKey2SystemKey (XkbKeycodeToKeysym (display, event.xkey.keycode, 0, 0));
        
        Notify (WindowEvent_OnKeyDown, message);
        
        break;
      }
      case KeyRelease:
      {
        GetKeysState (event.xkey.state, context);
        
        context.cursor_position = Point (event.xkey.x, event.xkey.y);
        context.key_scan_code   = (ScanCode)event.xkey.keycode;
        context.key             = VirtualKey2SystemKey (XkbKeycodeToKeysym (display, event.xkey.keycode, 0, 0));
        
        Notify (WindowEvent_OnKeyUp, message);
        
        break;
      }
      case ButtonPress:
      {
        context.cursor_position = Point (event.xbutton.x, event.xbutton.y);

        GetKeysState (event.xbutton.state, context);
        
        NotifyMouseMove (message);

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
        
        NotifyMouseMove (message);
        
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
        
        NotifyMouseMove (message);
        
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
        Notify (WindowEvent_OnDestroy, xtl::intrusive_ptr<DestroyMessage> (new DestroyMessage (*this), false));
        return;
      case ClientMessage:
      {
        if ((Atom)event.xclient.data.l [0] == wm_delete)
        {
          Notify (WindowEvent_OnClose, message);
        }
        
        break;
      }
      case UnmapNotify:
        Notify (WindowEvent_OnHide, message);          
        break;          
      case MapNotify:
        Notify (WindowEvent_OnShow, message);
        break;          
      case ReparentNotify:
        break;          
      case ConfigureNotify:
      {
        bool is_moved   = window_rect.left != (size_t)event.xconfigure.x || window_rect.top != (size_t)event.xconfigure.y,
             is_resized = window_rect.right - window_rect.left != (size_t)event.xconfigure.width || window_rect.bottom - window_rect.top != (size_t)event.xconfigure.height;
        
        window_rect.left   = (size_t)event.xconfigure.x;
        window_rect.top    = (size_t)event.xconfigure.y;
        window_rect.right  = (size_t)event.xconfigure.x + event.xconfigure.width;
        window_rect.bottom = (size_t)event.xconfigure.y + event.xconfigure.height;
        
        //this event is sent on window creation, so do not send change event in this case
        if (window_rect_init)
        {
          if (is_moved)   Notify (WindowEvent_OnMove, message);
          if (is_resized) Notify (WindowEvent_OnSize, message);
        }
        else
        {
          window_rect_init = true;
        }
        
        break;          
      }
      default:
        break;
    }
  }
  
///Оповещение о возникновении события
  void Notify (WindowEvent event, const xtl::intrusive_ptr<Message>& message)
  {
    XUnlockDisplay (display);
    
    message->event = event;
    
    message_queue.PushMessage (*this, message);

    XLockDisplay (display);
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

namespace
{

void WmNoDecorations (Display* display, XWindow window, XWindow parent_window)
{
    //TODO: check errors

  //KDE decoration values
  enum
  {
    KDE_noDecoration      = 0,
    KDE_normalDecoration  = 1,
    KDE_tinyDecoration    = 2,
    KDE_noFocus           = 256,
    KDE_standaloneMenuBar = 512,
    KDE_desktopIcon       = 1024,
    KDE_staysOnTop        = 2048,
  };
  
  enum
  {    
    MWM_DECOR_NONE      = 0,
    MWM_DECOR_ALL       = (1L << 0),
    MWM_DECOR_BORDER    = (1L << 1),
    MWM_DECOR_RESIZEH   = (1L << 2),
    MWM_DECOR_TITLE     = (1L << 3),
    MWM_DECOR_MENU      = (1L << 4),
    MWM_DECOR_MINIMIZE  = (1L << 5),
    MWM_DECOR_MAXIMIZE  = (1L << 6),
  };

  Atom WM_HINTS = XInternAtom (display, "_MOTIF_WM_HINTS", True);

  if (WM_HINTS != None)
  {
    static const int MWM_HINTS_DECORATIONS = (1L << 1);
    
    struct
    {
      unsigned long flags;
      unsigned long functions;
      unsigned long decorations;
               long input_mode;
      unsigned long status;
    } MWMHints = { MWM_HINTS_DECORATIONS, 0, MWM_DECOR_NONE, 0, 0 };

    XChangeProperty (display, window, WM_HINTS, WM_HINTS, 32, PropModeReplace, (unsigned char*)&MWMHints, sizeof (MWMHints) / 4);
  }

  WM_HINTS = XInternAtom (display, "KWM_WIN_DECORATION", True);
  
  if (WM_HINTS != None)
  {
    long KWMHints = KDE_tinyDecoration;

    XChangeProperty (display, window, WM_HINTS, WM_HINTS, 32, PropModeReplace,  (unsigned char *)&KWMHints, sizeof (KWMHints)/4);
  }

  WM_HINTS = XInternAtom (display, "_WIN_HINTS", True);
  
  if (WM_HINTS != None)
  {
    long GNOMEHints = 0;
    
    XChangeProperty (display, window, WM_HINTS, WM_HINTS, 32, PropModeReplace, (unsigned char *)&GNOMEHints, sizeof (GNOMEHints) / 4);
  }

  WM_HINTS = XInternAtom (display, "_NET_WM_WINDOW_TYPE", True);

  if (WM_HINTS != None)
  {
    Atom NET_WMHints [2];
    
    NET_WMHints [0] = XInternAtom (display, "_KDE_NET_WM_WINDOW_TYPE_OVERRIDE", True);
    NET_WMHints [1] = XInternAtom (display, "_NET_WM_WINDOW_TYPE_NORMAL", True);

    XChangeProperty (display, window, WM_HINTS, XA_ATOM, 32, PropModeReplace, (unsigned char *)&NET_WMHints, 2);
  }

  XSetTransientForHint (display, window, parent_window);
}

}

window_t XlibWindowManager::CreateWindow (WindowStyle style, WindowMessageHandler handler, const void* parent_handle, const char* init_string, void* user_data)
{
  try
  {
      //Проверка что приложение уже запущено, в противном случае возможна ситуация, что очередь событий будет удалена раньше окна, и при удалении окна произойдет креш, поскольку окно пошлет событие в удаленную очередь

    if (!MessageQueueSingleton::IsInitialized ())
      throw xtl::format_operation_exception ("", "Can't create window before application start (Message queue singleton is not initialized)");

      //определение стиля окна
      
    switch (style)
    {
      case WindowStyle_Overlapped:
      case WindowStyle_PopUp: //not implemented
        break;
      default:
        throw xtl::make_argument_exception ("", "style", style);
    }
    
      //блокировка соединения с дисплеем
      
    ::Display   *display = DisplayManagerSingleton::Instance ()->Display ();
    DisplayLock lock (display);
    
      //создание дескриптора окна

    stl::auto_ptr<window_handle> impl (new window_handle (handler, user_data));
      
      //создание окна

    XWindow parent_window = 0;
    
    if (parent_handle) parent_window = (XWindow)parent_handle;
    else               parent_window = DefaultRootWindow (impl->display);
    
    common::PropertyMap init_params = common::parse_init_string (init_string);

    //do not enable backing store by default, because opengl rendering doesn't work with it enabled at least on Ubuntu 16.04 running via Parallels Desktop
    bool         enable_backing_store = init_params.IsPresent ("enable_backing_store") ? init_params.GetInteger ("enable_backing_store") > 0 : false;
    int          window_x             = init_params.IsPresent ("x") ? init_params.GetInteger ("x") : DEFAULT_WINDOW_X,
                 window_y             = init_params.IsPresent ("y") ? init_params.GetInteger ("y") : DEFAULT_WINDOW_Y;
    unsigned int window_width         = init_params.IsPresent ("width") ? init_params.GetInteger ("width") : DEFAULT_WINDOW_WIDTH,
                 window_height        = init_params.IsPresent ("height") ? init_params.GetInteger ("height") : DEFAULT_WINDOW_HEIGHT;

    XSetWindowAttributes window_attributes;
    unsigned long        values_mask = 0;

    if (enable_backing_store)
    {
      window_attributes.backing_store = Always;
      values_mask |= CWBackingStore;
    }

    impl->window = XCreateWindow (impl->display, parent_window, window_x, window_y, window_width, window_height, 0, CopyFromParent,
                                  InputOutput, CopyFromParent, values_mask, &window_attributes);

    if (!impl->window)
      throw xtl::format_operation_exception ("", "Can't create window for display '%s'", XDisplayString (impl->display));

    //we should set size and position hints. Without this window manager may ignore params passed to XCreateSimpleWindow (Tested on Ubuntu 16.04)
    XSizeHints size_hints = {0};

    size_hints.flags  = PPosition | PSize;
    size_hints.x      = window_x;
    size_hints.y      = window_y;
    size_hints.width  = window_width;
    size_hints.height = window_height;

    XSetWMNormalHints(impl->display, impl->window, &size_hints);

    impl->is_fullscreen = init_params.IsPresent ("fullscreen") && init_params.GetInteger ("fullscreen");

    if (impl->is_fullscreen)
    {
      //do fullscreen on window creation step, because it provides better fps comparing to changing this after window was shown
      Atom fullscreen_atom = XInternAtom (impl->display, "_NET_WM_STATE_FULLSCREEN", True),
           state_atom      = XInternAtom (impl->display, "_NET_WM_STATE", True);

      XChangeProperty (impl->display, impl->window, state_atom, XA_ATOM, 32, PropModeReplace, (unsigned char*)&fullscreen_atom, 1);
    }
      
    try
    {      
        //создание невидимого курсора
        
      char data [1] = {0};

      Pixmap blank = XCreateBitmapFromData (impl->display, impl->window, data, 1, 1);
      
      if (blank == None)
        throw xtl::format_operation_exception ("", "XCreateBitmapFromData failed");
        
      XColor dummy;        
        
      impl->invisible_cursor = XCreatePixmapCursor (impl->display, blank, blank, &dummy, &dummy, 0, 0);
      
      XFreePixmap (impl->display, blank);                            
      
      if (!impl->invisible_cursor)
        throw xtl::format_operation_exception ("", "XCreatePixmapCursor failed");
        
        //замена обработчика закрытия окна
      
      impl->wm_delete = XInternAtom (impl->display, "WM_DELETE_WINDOW", True);

      if (!impl->wm_delete)
        throw xtl::format_operation_exception ("", "WM_DELETE_WINDOW atom not found");

      if (!XSetWMProtocols (impl->display, impl->window, &impl->wm_delete, 1))
        throw xtl::format_operation_exception ("", "XSetWMProtocols failed");

        //удаление заголовка окна
        
      if (style == WindowStyle_PopUp)
        WmNoDecorations (impl->display, impl->window, DefaultRootWindow (impl->display));
      
        //регистрация окна в менеджере соединения с дисплеем

      DisplayManagerSingleton::Instance ()->RegisterWindow (impl->window, impl.get ());
      
        //настройка получения событий

      XSelectInput (impl->display, impl->window, StructureNotifyMask | ExposureMask | ButtonPressMask |
                                                 KeyPressMask | KeyReleaseMask | ButtonReleaseMask |
                                                 PointerMotionMask | FocusChangeMask | VisibilityChangeMask |
                                                 ExposureMask);

      XFlush (impl->display);    
    }
    catch (...)
    {
      if (impl->invisible_cursor)
        XFreeCursor (impl->display, impl->invisible_cursor);

      XDestroyWindow (impl->display, impl->window);

      throw;
    }
    
    return impl.release (); 
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::CreateWindow");
    throw;  
  }
}

void XlibWindowManager::CloseWindow (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);
    
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
    e.touch ("syslib::XlibWindowManager::CloseWindow");
    throw;  
  }
}

void XlibWindowManager::DestroyWindow (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");

    if (handle->is_destroyed)
      return;

    {
      DisplayLock lock (handle->display);        
      
      if (!XFreeCursor (handle->display, handle->invisible_cursor))
        throw xtl::format_operation_exception ("", "XFreeCursor failed");
    
      if (!XDestroyWindow (handle->display, handle->window))
        throw xtl::format_operation_exception ("", "XDestroyWindow failed");
    }      
        
    handle->is_destroyed = true;
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::DestroyWindow");
    throw;  
  }
}

/*
    Получение платформо-зависимого дескриптора окна
*/

const void* XlibWindowManager::GetNativeWindowHandle (window_t handle)
{
  if (!handle)
    return 0;

  return reinterpret_cast<const void*> (handle->window);
}

const void* XlibWindowManager::GetNativeDisplayHandle (window_t handle)
{
  if (!handle)
    throw xtl::make_null_argument_exception ("syslib::XlibWindowManager::GetNativeDisplayHandle", "handle");
    
  return handle->display;
}

/*
    Заголовок окна
*/

void XlibWindowManager::SetWindowTitle (window_t handle, const wchar_t* title)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);      
    
    XStoreName (handle->display, handle->window, common::tostring (title).c_str ());
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetWindowTitle");
    throw;  
  }
}

void XlibWindowManager::GetWindowTitle (window_t handle, size_t buffer_size_in_chars, wchar_t* buffer)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);      
    
    char* window_name = 0;
    
    if (!XFetchName (handle->display, handle->window, &window_name) || !window_name)
      throw xtl::format_operation_exception ("", "XFetchName failed");
      
    stl::wstring result = common::towstring (window_name);
    
    xtl::xstrncpy (buffer, result.c_str (), buffer_size_in_chars);
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetWindowTitle");
    throw;  
  }
}

/*
    Область окна / клиентская область
*/

void XlibWindowManager::SetWindowRect (window_t handle, const Rect& rect)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    if (handle->is_fullscreen)
      return;

    DisplayLock lock (handle->display);      
    
    XWindowChanges changes;
    
    memset (&changes, 0, sizeof (XWindowChanges));        
    
    changes.x      = (int)rect.left;
    changes.y      = (int)rect.top;
    changes.width  = rect.right - rect.left;
    changes.height = rect.bottom - rect.top;

    if (!XConfigureWindow (handle->display, handle->window, CWX | CWY | CWWidth | CWHeight, &changes))
      throw xtl::format_operation_exception ("", "XConfigureWindow failed");

    if (!XSync (handle->display, false))
      throw xtl::format_operation_exception ("", "XConfigureWindow->XSync failed");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetWindowRect");
    throw;  
  }
}

void XlibWindowManager::SetClientRect (window_t handle, const Rect& rect)
{
  try
  {
    Rect window_rect;
    
    GetWindowRect (handle, window_rect);
    
    window_rect.right  = window_rect.left + rect.right - rect.left;
    window_rect.bottom = window_rect.top + rect.bottom - rect.top;
    
    SetWindowRect (handle, window_rect);
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetClientRect");
    throw;  
  }
}

void XlibWindowManager::GetWindowRect (window_t handle, Rect& rect)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);          
      
    XWindow root_return = 0;
    int x_return = 0, y_return = 0;
    unsigned int width_return = 0, height_return = 0, border_width_return = 0, depth_return = 0;
    
    if (!XGetGeometry (handle->display, handle->window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return))
      throw xtl::format_operation_exception ("", "XGetGeometry failed");
      
    rect.left   = (size_t)x_return;
    rect.top    = (size_t)y_return;
    rect.right  = rect.left + width_return;
    rect.bottom = rect.top + height_return;

    //Window may change it's size more than once during creation before sending first ConfigureNotify.
    //In this case other code may request window rect before ConfigureNotify was sent and if we do not save rect and init flag here,
    //ConfigureNotify handler will not send resize notification
    if (!handle->window_rect_init)
    {
      handle->window_rect      = rect;
      handle->window_rect_init = true;
    }
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetWindowRect");
    throw;  
  }
}

void XlibWindowManager::GetClientRect (window_t handle, Rect& rect)
{
  try
  {        
    GetWindowRect (handle, rect);
    
    rect.right  -= rect.left;
    rect.bottom -= rect.top;
    rect.left    = 0;
    rect.top     = 0;
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetClientRect");
    throw;  
  }
}

/*
    Установка флагов окна
*/

void XlibWindowManager::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);          
    
    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
      {
        if (state)
        {
          if (!XMapWindow (handle->display, handle->window))
            throw xtl::format_operation_exception ("", "XMapWindow failed");
        }
        else
        {
          if (!XUnmapWindow (handle->display, handle->window))
            throw xtl::format_operation_exception ("", "XUnmapWindow failed");
        }

        break;
      }
      case WindowFlag_Active: //активность окна
        if (state)
        {
          if (!XRaiseWindow (handle->display, handle->window))
            throw xtl::format_operation_exception ("", "XRaiseWindow failed");
        }
        else
        {
          if (!XLowerWindow (handle->display, handle->window))
            throw xtl::format_operation_exception ("", "XLowerWindow failed");
        }

        break;
      case WindowFlag_Focus: //фокус ввода
      {
        if (state)
        {
          XWindowAttributes window_attributes_return;

          if (!XGetWindowAttributes (handle->display, handle->window, &window_attributes_return))
            throw xtl::format_operation_exception ("", "XGetWindowAttributes failed");

          //window focus can be set only if it is viewable
          if (window_attributes_return.map_state == IsViewable)
          {
            if (!XSetInputFocus (handle->display, state ? handle->window : None, RevertToNone, CurrentTime))
              throw xtl::format_operation_exception ("", "XSetInputFocus failed");
          }
        }
        else
        {
          if (!XSetInputFocus (handle->display, None, RevertToNone, CurrentTime))
            throw xtl::format_operation_exception ("", "XSetInputFocus failed");
        }

        break;
      }
      case WindowFlag_Maximized:
      {
        XWindowAttributes xwa;

        if (!XGetWindowAttributes (handle->display, DefaultRootWindow (handle->display), &xwa))
          throw xtl::format_operation_exception ("", "XGetWindowAttributes failed");

        if (!XMoveResizeWindow (handle->display, handle->window, 0, 0, xwa.width, xwa.height))
          throw xtl::format_operation_exception ("", "XMoveResizeWindow failed");
          
        if (!XMapWindow (handle->display, handle->window))
          throw xtl::format_operation_exception ("", "XMapWindow failed");

        break;
      }
      case WindowFlag_Minimized:
      {
        if (state)
        {
          if (!XUnmapWindow (handle->display, handle->window))
            throw xtl::format_operation_exception ("", "XUnmapWindow failed");
        }
        else
        {
          if (!XMapWindow (handle->display, handle->window))
            throw xtl::format_operation_exception ("", "XMapWindow failed");
        }
        
        break;
      }
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
    
    XFlush (handle->display);    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetWindowFlag");
    throw;  
  }
}

bool XlibWindowManager::GetWindowFlag (window_t handle, WindowFlag flag)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);          
    
    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
      {
        XWindowAttributes window_attributes_return;

        if (!XGetWindowAttributes (handle->display, handle->window, &window_attributes_return))
          throw xtl::format_operation_exception ("", "XGetWindowAttributes failed");
          
        return window_attributes_return.map_state == IsViewable;
      }
      case WindowFlag_Active: //активность окна
        return false; //TODO
//        break;
      case WindowFlag_Focus: //фокус ввода
      {
        XWindow focused_window = 0;
        int revert_to_return = 0;
        
        if (!XGetInputFocus (handle->display, &focused_window, &revert_to_return))
          throw xtl::format_operation_exception ("", "XGetInputFocus failed");
          
        return focused_window == handle->window;
      }
      case WindowFlag_Maximized:
      {
        return false; //TODO
        
//        break;
      }
      case WindowFlag_Minimized:
      {
        return false; //TODO
//        break;
      }
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetWindowFlag");
    throw;  
  }
}

/*
    Установка родительского окна
*/

void XlibWindowManager::SetParentWindowHandle (window_t child, const void* parent_handle)
{
  try
  {
    if (!child)
      throw xtl::make_null_argument_exception ("", "child");
    
    DisplayLock lock (child->display);
    
    XWindow parent = parent_handle ? (XWindow)parent_handle : DefaultRootWindow (child->display);
    
    XWindow root_return = 0;
    int x_return = 0, y_return = 0;
    unsigned int width_return = 0, height_return = 0, border_width_return = 0, depth_return = 0;
    
    if (!XGetGeometry (child->display, child->window, &root_return, &x_return, &y_return, &width_return, &height_return, &border_width_return, &depth_return))
      throw xtl::format_operation_exception ("", "XGetGeometry failed");    
    
    if (!XReparentWindow (child->display, child->window, parent, x_return, y_return))
      throw xtl::format_operation_exception ("", "XReparentWindow failed");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetParentWindow");
    throw;  
  }
}

const void* XlibWindowManager::GetParentWindowHandle (window_t child)
{
  try
  {
    if (!child)
      throw xtl::make_null_argument_exception ("", "child");
      
    DisplayLock lock (child->display);
      
    XWindow root_return = 0, parent_return = 0, *children_return = 0;
    unsigned int nchildren_return = 0;
    
    if (!XQueryTree (child->display, child->window, &root_return, &parent_return, &children_return, &nchildren_return))
      throw xtl::format_operation_exception ("", "XQueryTree failed");
    
    return reinterpret_cast<const void*> (parent_return);
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetParentWindow");
    throw;  
  }
}

/*
    Обновление окна
*/

void XlibWindowManager::InvalidateWindow (window_t handle)
{
  try
  {    
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
   
    if (!XClearArea (handle->display, handle->window, 0, 0, 0, 0, True))
      throw xtl::format_operation_exception ("", "XClearArea failed");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::InvalidateWindow");
    throw;  
  }
}

/*
    Положение курсора
*/

void XlibWindowManager::SetCursorPosition (const Point& position)
{
  try
  {
    Display* display = DisplayManagerSingleton::Instance ()->Display ();
    
    DisplayLock lock (display);
    
    if (!XWarpPointer (display, None, XDefaultRootWindow (display), 0, 0, 0, 0, position.x, position.y))
      throw xtl::format_operation_exception ("", "XWarpPointer failed");
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetCursorPosition(const Point&)");
    throw;  
  }
}

syslib::Point XlibWindowManager::GetCursorPosition ()
{
  try
  {
    Display* display = DisplayManagerSingleton::Instance ()->Display ();
    
    DisplayLock lock (display);    
    
    XWindow ret_root, ret_child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;

    if (!XQueryPointer (display, DefaultRootWindow (display), &ret_root, &ret_child, &root_x, &root_y, &win_x, &win_y, &mask))
      throw xtl::format_operation_exception ("", "XQueryPointer failed");
      
    return Point (root_x, root_y);    
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetCursorPosition()");
    throw;  
  }
}

void XlibWindowManager::SetCursorPosition (window_t handle, const Point& client_position)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);          
    
    if (!XWarpPointer (handle->display, None, handle->window, 0, 0, 0, 0, client_position.x, client_position.y))
      throw xtl::format_operation_exception ("", "XWarpPointer failed");
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetCursorPosition(window_t,const Point&)");
    throw;  
  }
}

syslib::Point XlibWindowManager::GetCursorPosition (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);          
    
    XWindow ret_root, ret_child;
    int root_x, root_y, win_x, win_y;
    unsigned int mask;

    if (!XQueryPointer (handle->display, handle->window, &ret_root, &ret_child, &root_x, &root_y, &win_x, &win_y, &mask))
      throw xtl::format_operation_exception ("", "XQueryPointer failed");
      
    return Point (root_x, root_y);
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetCursorPosition(window_t)");
    throw;  
  }
}

/*
    Видимость курсора
*/

void XlibWindowManager::SetCursorVisible (window_t handle, bool state)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);          
    
    if (handle->is_cursor_visible == state)
      return;                
      
    Cursor cursor = state ? handle->active_cursor ? handle->active_cursor->cursor : (Cursor)0 : handle->invisible_cursor;
      
    if (cursor)
    {
      if (!XDefineCursor (handle->display, handle->window, cursor))
        throw xtl::format_operation_exception ("", "XDefineCursor failed");
    }
    else
    {
      if (!XUndefineCursor (handle->display, handle->window))
        throw xtl::format_operation_exception ("", "XUndefineCursor failed");
    }
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetCursorVisible");
    throw;  
  }
}

bool XlibWindowManager::GetCursorVisible (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    return handle->is_cursor_visible;
  }    
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetCursorVisible");
    throw;  
  }  
}

/*
    Изображение курсора
*/

cursor_t XlibWindowManager::CreateCursor (const char* file_name, int hotspot_x, int hotspot_y)
{
  try
  {    
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
      
    stl::auto_ptr<cursor_handle> cursor (new cursor_handle (DisplayManagerSingleton::Instance ()->Display ()));
    
    DisplayLock lock (cursor->display);
    
    media::Image image (file_name, media::PixelFormat_RGBA8);
    xtl::uninitialized_storage<unsigned char> mask_bitmap (image.Width () * image.Height () / 8 + 1);
    
    unsigned char*       dst    = mask_bitmap.data ();
    const unsigned char* src    = (const unsigned char*)image.Bitmap ();    
    size_t               offset = 0;
    
    for (size_t i=0, height=image.Height (); i<height; i++)
    {
      for (size_t j=0, width=image.Width (); j<width; j++, src+=4, offset++)
      {
        if (offset == 8)
        {
          *++dst = 0;          
          offset = 0;
        }
        
        if (src [3] > 128) *dst = *dst | (1 << offset);
      }
    }
    
    Pixmap mask_pixmap = XCreateBitmapFromData (cursor->display, DefaultRootWindow (cursor->display), (char*)mask_bitmap.data (), image.Width (), image.Height ());
    
    if (mask_pixmap == None)
      throw xtl::format_operation_exception ("", "XCreatePixmapFromBitmapData failed");

    XColor color;
        
    memset (&color, 0, sizeof (XColor));
    
    color.red   = 65535;
    color.green = 65535;
    color.blue  = 65535;

    cursor->cursor = XCreatePixmapCursor (cursor->display, mask_pixmap, mask_pixmap, &color, &color, 0, 0);//hotspot_x == -1 ? 0 : hotspot_x, hotspot_y == -1 ? hotspot_y : 0);
    
    XFreePixmap (cursor->display, mask_pixmap);
    
    if (!cursor->cursor)
      throw xtl::format_operation_exception ("", "XCreatePixmapCursor failed");

    return cursor.release ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::XlibWindowManager::CreateCursor");
    throw;
  }
}

void XlibWindowManager::DestroyCursor (cursor_t cursor)
{
  try
  {
    if (!cursor)
      throw xtl::make_null_argument_exception ("", "cursor");
      
    DisplayLock lock (cursor->display);
      
    if (!XFreeCursor (cursor->display, cursor->cursor))
      throw xtl::format_operation_exception ("", "XFreeCursor failed");
      
    delete cursor;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::XlibWindowManager::DestroyCursor");
    throw;
  }
}

void XlibWindowManager::SetCursor (window_t handle, cursor_t cursor)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");      
      
    DisplayLock lock (handle->display);    
    
    handle->active_cursor = cursor; 
    
    Cursor cursor = handle->is_cursor_visible ? handle->active_cursor ? handle->active_cursor->cursor : (Cursor)0 : handle->invisible_cursor;      
    if (cursor)
    {
      if (!XDefineCursor (handle->display, handle->window, cursor))
        throw xtl::format_operation_exception ("", "XDefineCursor failed");
    }
    else
    {
      if (!XUndefineCursor (handle->display, handle->window))
        throw xtl::format_operation_exception ("", "XUndefineCursor failed");
    }
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetCursor");
    throw;
  }
}

/*
    Цвет фона
*/

void XlibWindowManager::SetBackgroundColor (window_t handle, const Color& color)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");
      
    DisplayLock lock (handle->display);
    
    XColor exact_color, screen_color;
    
    memset (&exact_color, 0, sizeof (XColor));
    memset (&screen_color, 0, sizeof (XColor));    
    
    if (!XAllocNamedColor (handle->display, DefaultColormap (handle->display, DefaultScreen (handle->display)), common::format ("rgb:%02x/%02x/%02x", color.red, color.green, color.blue).c_str (), &screen_color, &exact_color))
      throw xtl::format_operation_exception ("", "XAllocNamedColor failed");

    XSetWindowAttributes attr;

    attr.background_pixel = screen_color.pixel;
    
    if (!XChangeWindowAttributes (handle->display, handle->window, CWBackPixel, &attr))
      throw xtl::format_operation_exception ("", "XChangeWindowAttributes failed");
      
    handle->background_color = color;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetBackgroundColor");
    throw;
  }
}

void XlibWindowManager::SetBackgroundState (window_t handle, bool state)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");    
      
    handle->background_state = state;
  }  
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetBackgroundState");
    throw;
  }
}

Color XlibWindowManager::GetBackgroundColor (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");    
      
    return handle->background_color;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetBackgroundColor");
    throw;
  }
}

bool XlibWindowManager::GetBackgroundState (window_t handle)
{
  try
  {
    if (!handle)
      throw xtl::make_null_argument_exception ("", "handle");    
      
    return handle->background_state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::GetBackgroundState");
    throw;
  }
}

/*
    Получение имени клавиши
*/

//возвращается длина строки без учёта '\0'
size_t XlibWindowManager::GetKeyName (ScanCode scan_code, size_t buffer_size, char* buffer)
{
  static const char* METHOD_NAME = "syslib::XlibWindowManager::GetKeyName";

  if (scan_code < 0 || scan_code >= ScanCode_Num)
    throw xtl::make_argument_exception (METHOD_NAME, "scan_code", scan_code);

  if (!buffer && buffer_size)
    throw xtl::make_null_argument_exception (METHOD_NAME, "buffer");

  if (!buffer_size)
    return 0;
    
  Display* display = DisplayManagerSingleton::Instance ()->Display ();
  
  DisplayLock lock (display);
    
  char* name = XKeysymToString (XkbKeycodeToKeysym (display, scan_code, 0, 0));

  if (!name)
    return 0;

  strncpy (buffer, name, buffer_size);

  return strlen (buffer);
}

/*
   Установка/получение multitouch режима
*/

void XlibWindowManager::SetMultitouchEnabled (window_t window, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    window->is_multitouch_enabled = state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::SetMultitouchEnabled");
    throw;
  }
}

bool XlibWindowManager::IsMultitouchEnabled (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    return window->is_multitouch_enabled;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::XlibWindowManager::IsMultitouchEnabled");
    throw;
  }
}


/*
===================================================================================================
    DisplayManager
===================================================================================================
*/

void DisplayManager::SetDisplay (const char* display_string)
{
  DisplayManagerSingleton::Instance ()->SetDisplayString (display_string);
}

const char* DisplayManager::Display ()
{
  return DisplayManagerSingleton::Instance ()->DisplayString ();
}

void DisplayManager::SetSyncMode (bool state)
{
  DisplayManagerSingleton::Instance ()->SetSyncMode (state);
}

bool DisplayManager::IsSyncMode ()
{
  return DisplayManagerSingleton::Instance ()->IsSyncMode ();
}

const void* DisplayManager::DisplayHandle ()
{
  return DisplayManagerSingleton::Instance ()->Display ();
}
