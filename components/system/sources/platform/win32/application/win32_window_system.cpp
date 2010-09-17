#include "../shared.h"

using namespace syslib;
using namespace common;

namespace
{

const char* WINDOW_CLASS_NAME = "Default window class";

/*
    Получение экземпляра приложения
*/

HINSTANCE GetApplicationInstance ()
{
  return GetModuleHandle (0);
}

/*
    Копирование файла в папку temp
*/

class TempFile
{
  public:
    TempFile (const char* source_file_name)
    {
      if (!source_file_name)
        throw xtl::make_null_argument_exception ("syslib::TempFile::TempFile", "source_file_name");
        
        //генерация имени нового файла
        
      stl::string dir_name;
      
      dir_name.fast_resize (MAX_PATH);
      
      DWORD dir_len = GetTempPath (dir_name.size (), &dir_name [0]);
      
      if (!dir_len || dir_len > MAX_PATH)
        raise_error ("::GetTempPath");
        
      file_name.fast_resize (MAX_PATH);
        
      if (!GetTempFileName (dir_name.c_str (), TEXT("TEMP"), 0, &file_name [0]))
        raise_error ("::GetTempFileName");
        
        //копирование содержимого
        
      #undef CopyFile
        
      FileSystem::CopyFile (source_file_name, file_name.c_str ());
    }
    
    ~TempFile ()
    {
      FileSystem::Remove (file_name.c_str ());
    }
    
    const char* Path () const { return file_name.c_str (); }
  
  private:
    stl::string file_name; //имя файла
};

/*
    Описание реализации окна
*/

struct WindowImpl
{
  void*                          user_data;           //указатель на пользовательские данные
  Platform::WindowMessageHandler message_handler;     //функция обработки сообщений окна
  bool                           is_cursor_visible;   //видим ли курсор
  bool                           is_cursor_in_window; //находится ли курсор в окне
  HCURSOR                        default_cursor;      //курсор по умолчанию
  HCURSOR                        preferred_cursor;    //предпочитаемый курсор для данного окна
  HBRUSH                         background_brush;    //кисть для очистки заднего фона
  Color                          background_color;    //цвет заднего фона
  bool                           background_state;    //включен ли задний фон

  WindowImpl (Platform::WindowMessageHandler handler, void* in_user_data)
    : user_data (in_user_data)
    , message_handler (handler)
    , is_cursor_visible (true)
    , is_cursor_in_window (false)
    , default_cursor (LoadCursor (0, IDC_ARROW))
    , preferred_cursor (0)
    , background_brush (CreateSolidBrush (RGB (0, 0, 0)))
    , background_state (false)
  {
    if (!background_brush)
      raise_error ("::CreateSolidBrush");
  }

  void TrackCursor (HWND wnd)
  {
    if (is_cursor_in_window)
      return;
    
    TRACKMOUSEEVENT track_mouse_event;
    
    memset (&track_mouse_event, 0, sizeof (track_mouse_event));
    
    track_mouse_event.cbSize     = sizeof (TRACKMOUSEEVENT);
    track_mouse_event.dwFlags    = TME_LEAVE;
    track_mouse_event.hwndTrack  = wnd;
    
    if (!TrackMouseEvent (&track_mouse_event))
      raise_error ("::TrackMouseEvent");

    is_cursor_in_window = true;
  }

  void Notify (Platform::window_t window, WindowEvent event, const WindowEventContext& context)
  {
    try
    {
      message_handler (window, event, context, user_data);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

/*
    Получение контекста события
*/

void GetEventContext (HWND wnd, WindowEventContext& context, RECT& client_rect)
{
  memset (&context, 0, sizeof (context));

  context.handle = wnd;

  GetClientRect (wnd, &client_rect);
 
  POINT cursor_position;

  GetCursorPos (&cursor_position);
  ScreenToClient (wnd, &cursor_position);

  if (cursor_position.x < client_rect.left)   cursor_position.x = client_rect.left;
  if (cursor_position.y < client_rect.top)    cursor_position.y = client_rect.top;
  if (cursor_position.x > client_rect.right)  cursor_position.x = client_rect.right;
  if (cursor_position.y > client_rect.bottom) cursor_position.y = client_rect.bottom;

  context.cursor_position.x = cursor_position.x;
  context.cursor_position.y = cursor_position.y;

  context.keyboard_alt_pressed        = (GetKeyState (VK_MENU) & 2) != 0;
  context.keyboard_control_pressed    = (GetKeyState (VK_CONTROL) & 2) != 0;
  context.keyboard_shift_pressed      = (GetKeyState (VK_SHIFT) & 2) != 0;
  context.mouse_left_button_pressed   = (GetKeyState (VK_LBUTTON) & 2) != 0;
  context.mouse_right_button_pressed  = (GetKeyState (VK_RBUTTON) & 2) != 0;
  context.mouse_middle_button_pressed = (GetKeyState (VK_MBUTTON) & 2) != 0;
}

/*
    Преобразование виртуальных кодов клавиш в syslib::Key
*/

Key VirtualKey2SystemKey (size_t vkey)
{
  static size_t map [] =
  {
    0, 0, 0, 0, 0, 0, 0, 0,  //0x00-0x07
    Key_BackSpace, Key_Tab, 0, 0, 0, Key_Enter, 0, 0,  //0x08-0x0F
    Key_Shift, Key_Control, Key_Alt, Key_Pause, Key_CapsLock, 0, 0, 0, //0x10-0x17
    0, 0, 0, Key_Escape, 0, 0, 0, 0, //0x18-0x1F
    Key_Space, Key_PageUp, Key_PageDown, Key_End, Key_Home, Key_Left, Key_Up, Key_Right, //0x20-0x27
    Key_Down, 0, 0, 0, 0, Key_Insert, Key_Delete, 0, //0x28-0x2F
    Key_0, Key_1, Key_2, Key_3, Key_4, Key_5, Key_6, Key_7, //0x30-0x37
    Key_8, Key_9, 0, 0, 0, 0, 0, 0, //0x38-0x3F
    0,     Key_A, Key_B, Key_C, Key_D, Key_E, Key_F, Key_G, //0x40-0x47
    Key_H, Key_I, Key_J, Key_K, Key_L, Key_M, Key_N, Key_O, //0x48-0x4F
    Key_P, Key_Q, Key_R, Key_S, Key_T, Key_U, Key_V, Key_W, //0x50-0x57
    Key_X, Key_Y, Key_Z, 0, 0, 0, 0, 0, //0x58-0x5F
    Key_NumPad0, Key_NumPad1, Key_NumPad2, Key_NumPad3, Key_NumPad4, Key_NumPad5, Key_NumPad6, Key_NumPad7, //0x60-0x67
    Key_NumPad8, Key_NumPad9, Key_NumPadMultiply, Key_NumPadPlus, 0, Key_NumPadMinus, Key_NumPadDot, Key_NumPadDivide, //0x68-0x6F
    Key_F1, Key_F2, Key_F3, Key_F4, Key_F5, Key_F6, Key_F7, Key_F8, //0x70-0x77
    Key_F9, Key_F10, Key_F11, Key_F12, 0, 0, 0, 0, //0x78-0x7F
    0, 0, 0, 0, 0, 0, 0, 0, //0x80-0x87
    0, 0, 0, 0, 0, 0, 0, 0, //0x88-0x8F
    Key_NumLock, Key_ScrollLock, 0, 0, 0, 0, 0, 0, //0x90-0x97
    0, 0, 0, 0, 0, 0, 0, 0, //0x98-0x9F
    0, 0, 0, 0, 0, 0, 0, 0, //0xA0-0xA7
    0, 0, 0, 0, 0, 0, 0, 0, //0xA8-0xAF
    0, 0, 0, 0, 0, 0, 0, 0, //0xB0-0xB7
    0, 0, Key_Semicolon, Key_Plus, Key_Comma, Key_Minus, Key_Dot, Key_Slash, //0xB8-0xBF
    Key_Tilda, 0, 0, 0, 0, 0, 0, 0, //0xC0-0xC7
    0, 0, 0, 0, 0, 0, 0, 0, //0xC8-0xCF
    0, 0, 0, 0, 0, 0, 0, 0, //0xD0-0xD7
    0, 0, 0, Key_LeftBracket, Key_BackSlash, Key_RightBracket, Key_Apostrophe, 0, //0xD8-0xDF
    0, 0, 0, 0, 0, 0, 0, 0, //0xE0-0xE7
    0, 0, 0, 0, 0, 0, 0, 0, //0xE8-0xEF
    0, 0, 0, 0, 0, 0, 0, 0, //0xF0-0xF7
    0, 0, 0, 0, 0, 0, 0, 0, //0xF8-0xFF
  };

  if (vkey >= sizeof (map) / sizeof (*map))
    return Key_Unknown;

  return (Key)map [vkey];
}

/*
    Получение скэн-кода
*/

ScanCode GetScanCode (size_t lParam)
{
  return (ScanCode)((lParam >> 16) & 0x1ff);
}

/*
    Функция обработки сообщений окна
*/

LRESULT CALLBACK WindowMessageHandler (HWND wnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    //получение указателя на пользовательские данные

  WindowImpl*        impl          = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
  Platform::window_t window_handle = (Platform::window_t)wnd;

    //если указатель на пользовательские данные не установлен - делаем попытку его установить

  if (!impl)
  {
    if (message == WM_CREATE)
    {
      CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*> (lparam);

      if (!cs->lpCreateParams)
        return 0;

      impl = reinterpret_cast<WindowImpl*> (cs->lpCreateParams);

        //устанавливаем указатель на пользовательские данные

      SetWindowLong (wnd, GWL_USERDATA, (LONG)impl);

        //оповещаем окно об изменении низкоуровневого дескриптора

      WindowEventContext context;

      memset (&context, 0, sizeof (context));

      context.handle = wnd;

      impl->Notify (window_handle, WindowEvent_OnChangeHandle, context);

      return 0;
    }

    return DefWindowProc (wnd, message, wparam, lparam);
  }

    //получение контекста события

  WindowEventContext context;
  RECT               client_rect;

  GetEventContext (wnd, context, client_rect);

    //обработка сообщений

  switch (message)
  {
    case WM_DESTROY: //уничтожение окна
      impl->Notify (window_handle, WindowEvent_OnDestroy, context);

      delete impl;

      SetWindowLong (wnd, GWL_USERDATA, 0);

      return 0;
    case WM_CLOSE: //попытка закрытия окна
      impl->Notify (window_handle, WindowEvent_OnClose, context);
      return 0;
    case WM_ACTIVATE: //активация/деактивация окна
      switch (wparam)
      {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
          impl->Notify (window_handle, WindowEvent_OnActivate, context);
          break;
        case WA_INACTIVE:
          impl->Notify (window_handle, WindowEvent_OnDeactivate, context);
          break;
      }

      return 0;
    case WM_SHOWWINDOW: //изменение видимости
      impl->Notify (window_handle, wparam ? WindowEvent_OnShow : WindowEvent_OnHide, context);
      return 0;
    case WM_SETFOCUS: //получение фокуса ввода
      impl->Notify (window_handle, WindowEvent_OnSetFocus, context);
      return 0;
    case WM_KILLFOCUS: //потеря фокуса ввода
      impl->Notify (window_handle, WindowEvent_OnLostFocus, context);
      return 0;
    case WM_SETCURSOR: //изменение положения курсора
      if (LOWORD(lparam) == HTCLIENT) //наша клиентная область
      {
        if (impl->is_cursor_visible)
        {
          SetCursor (impl->preferred_cursor ? impl->preferred_cursor : impl->default_cursor);
        }
        else if (!impl->is_cursor_visible)
        {
          SetCursor (0);
        }

        return 1;
      }

        //не клиентная область - обработчик по умолчанию

      return DefWindowProc (wnd, message, wparam, lparam);
    case WM_PAINT: //необходима перерисовка
    {
      PAINTSTRUCT ps;

      HDC dc = BeginPaint (wnd, &ps);
      
      if (impl->background_state && impl->background_brush)
      {
        SelectObject (dc, impl->background_brush);
        Rectangle (dc, client_rect.left, client_rect.top, client_rect.right, client_rect.bottom);
      }

      impl->Notify (window_handle, WindowEvent_OnPaint, context);

      EndPaint (wnd, &ps);

      return 0;
    }
    case WM_ERASEBKGND: //нужно ли очищать фон
      return 1; //очищать фон не нужно
    case WM_SIZE: //изменение размеров окна
      impl->Notify (window_handle, WindowEvent_OnSize, context);
      return 0;
    case WM_MOVE: //изменение положения окна
      impl->Notify (window_handle, WindowEvent_OnMove, context);
      return 0;
    case WM_MOUSEMOVE: //изменение положения курсора над областью окна
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->TrackCursor (wnd);
      impl->Notify (window_handle, WindowEvent_OnMouseMove, context);
      return 0;
    case WM_MOUSELEAVE:
      impl->is_cursor_in_window = false;
      impl->Notify (window_handle, WindowEvent_OnMouseLeave, context);
      return 0;
    case WM_MOUSEWHEEL: //изменилось положение вертикального колеса мыши
      context.mouse_vertical_wheel_delta = float (GET_WHEEL_DELTA_WPARAM (wparam)) / float (WHEEL_DELTA);

      impl->Notify (window_handle, WindowEvent_OnMouseVerticalWheel, context);
      return 0;
#ifdef WM_MOUSEHWHEEL
    case WM_MOUSEHWHEEL: //изменилось положение горизонтального колеса мыши
      context.mouse_horisontal_wheel_delta = float (GET_WHEEL_DELTA_WPARAM (wparam)) / float (WHEEL_DELTA);
      impl->Notify (window_handle, WindowEvent_OnMouseHorisontalWheel, context);
      return 0;
#endif
    case WM_LBUTTONDOWN: //нажата левая кнопка мыши
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnLeftButtonDown, context);
      return 0;
    case WM_LBUTTONUP: //отпущена левая кнопка мыши
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnLeftButtonUp, context);
      return 0;
    case WM_LBUTTONDBLCLK: //двойной щелчок левой кнопкой мыши
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnLeftButtonDoubleClick, context);
      return 0;
    case WM_RBUTTONDOWN: //нажата правая кнопка мыши
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnRightButtonDown, context);
      return 0;
    case WM_RBUTTONUP: //отпущена правая кнопка мыши
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnRightButtonUp, context);
      return 0;
    case WM_RBUTTONDBLCLK: //двойной щелчок правой кнопкой мыши
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnRightButtonDoubleClick, context);
      return 0;
    case WM_XBUTTONDOWN: //нажата X кнопка мыши
    {
      WindowEvent event;

      switch (HIWORD (wparam))
      {
        case 1:  event = WindowEvent_OnXButton1Down; break;
        case 2:  event = WindowEvent_OnXButton2Down; break;
        default: return 1;
      }

      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, event, context);

      return 1;
    }
    case WM_XBUTTONUP: //отпущена X кнопка мыши
    {
      WindowEvent event;

      switch (HIWORD (wparam))
      {
        case 1:  event = WindowEvent_OnXButton1Up; break;
        case 2:  event = WindowEvent_OnXButton2Up; break;
        default: return 1;
      }

      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, event, context);

      return 1;
    }
    case WM_XBUTTONDBLCLK: //двойной щелчок X кнопкой мыши
    {
      WindowEvent event;

      switch (HIWORD (wparam))
      {
        case 1:  event = WindowEvent_OnXButton1DoubleClick; break;
        case 2:  event = WindowEvent_OnXButton2DoubleClick; break;
        default: return 1;
      }

      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, event, context);

      return 1;
    }
    case WM_MBUTTONDOWN: //нажата средняя кнопка мыши
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnMiddleButtonDown, context);
      return 0;
    case WM_MBUTTONUP: //отпущена средняя кнопка мыши
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnMiddleButtonUp, context);
      return 0;
    case WM_MBUTTONDBLCLK: //двойной щелчок средней кнопкой мыши
      context.cursor_position.x = LOWORD (lparam);
      context.cursor_position.y = HIWORD (lparam);

      impl->Notify (window_handle, WindowEvent_OnMiddleButtonDoubleClick, context);
      return 0;
    case WM_KEYDOWN: //нажата клавиша клавиатуры
      context.key           = VirtualKey2SystemKey (wparam);
      context.key_scan_code = GetScanCode (lparam);

      impl->Notify (window_handle, WindowEvent_OnKeyDown, context);
      return 0;
    case WM_KEYUP: //отпущена клавиша клавиатуры
      context.key           = VirtualKey2SystemKey (wparam);
      context.key_scan_code = GetScanCode (lparam);

      impl->Notify (window_handle, WindowEvent_OnKeyUp, context);
      return 0;
    case WM_CHAR: //в буфере окна появился символ
      mbtowc (&context.char_code, (char*)&wparam, 1);
      impl->Notify (window_handle, WindowEvent_OnChar, context);
      return 0;
  }

    //обработка сообщений по умолчанию

  return DefWindowProc (wnd, message, wparam, lparam);
}

/*
    Регистрация оконного класса
*/

void RegisterWindowClass ()
{
  WNDCLASS wc;

  memset (&wc, 0, sizeof (wc));

  wc.style         = CS_OWNDC | CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = &WindowMessageHandler;
  wc.hInstance     = GetApplicationInstance ();
  wc.hIcon         = LoadIcon (GetApplicationInstance (), IDI_APPLICATION);
  wc.hCursor       = LoadCursor (GetApplicationInstance (), IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszClassName = WINDOW_CLASS_NAME;

  if (!RegisterClass (&wc))
    raise_error ("::RegisterClass");
}

}

/*
    Создание/закрытие/уничтожение окна
*/

#undef CreateWindow

Platform::window_t Platform::CreateWindow (WindowStyle style, WindowMessageHandler handler, window_t parent, void* user_data)
{
    //определене стиля окна

  UINT win_style;

  switch (style)
  {
    case WindowStyle_Overlapped:
      win_style = WS_OVERLAPPEDWINDOW;

      if (parent)
        win_style |= WS_CHILD | WS_CLIPSIBLINGS;

      break;
    case WindowStyle_PopUp:
      win_style = parent ? WS_CHILD  | WS_CLIPSIBLINGS : WS_POPUP;
      break;
    default:
      throw xtl::make_argument_exception ("syslib::Win32Platform::CreateWindow", "style", style);
  }

  try
  {
    static bool is_window_class_registered = false;

      //регистрация оконного класса

    if (!is_window_class_registered)
    {
        //корректная реализация требует также вызова UnregisterClass при выходе из приложения, но данный вызов
        //осуществляется автоматически поэтому не производится в ручную

      RegisterWindowClass ();

      is_window_class_registered = true;
    }

      //создание окна

    WindowImpl* volatile window_impl = new WindowImpl (handler, user_data);

    try
    {
      HWND wnd = CreateWindowA (WINDOW_CLASS_NAME, "", win_style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                                (HWND)parent, 0, GetApplicationInstance (), window_impl);

      if (!wnd)
        raise_error ("::CreateWindow");

      if (!UpdateWindow (wnd))
        raise_error ("::UpdateWindow");

      return (window_t)wnd;
    }
    catch (...)
    {
      delete window_impl;
      throw;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::CreateWindow");

    throw;
  }
}

void Platform::CloseWindow (window_t handle)
{
  try
  {
    HWND wnd = (HWND)handle;

    if (!PostMessage (wnd, WM_CLOSE, 0, 0))
      raise_error ("::PostMessage(window, WM_CLOSE,0, 0)");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::CloseWindow");
    throw;
  }
}

void Platform::DestroyWindow (window_t handle)
{
  try
  {
    HWND wnd = (HWND)handle;

    if (!::DestroyWindow (wnd))
      raise_error ("::DestroyWindow");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::DestroyWindow");
    throw;
  }
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

void Platform::SetWindowTitle (window_t handle, const wchar_t* title)
{
  try
  {
    if (!title)
      throw xtl::make_null_argument_exception ("", "title");

    HWND wnd = (HWND)handle;

    if (!SetWindowTextW (wnd, title))
      raise_error ("::SetWindowTextW");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetWindowTitle(const wchar_t*)");
    throw;
  }
}

void Platform::GetWindowTitle (window_t handle, size_t buffer_size, wchar_t* buffer)
{
  try
  {
    HWND wnd = (HWND)handle;

    if (!GetWindowTextW (wnd, buffer, buffer_size))
      raise_error ("::GetWindowText(wchar_t*)");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::GetWindowTitle");
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
    HWND wnd = (HWND)handle;

    RECT window_rect;

    if (!::GetWindowRect (wnd, &window_rect))
      raise_error ("::GetWindowRect");

    UINT flags = SWP_NOACTIVATE;

    if (size_t (window_rect.right - window_rect.left) == size_t (rect.right - rect.left) &&
        size_t (window_rect.bottom - window_rect.top) == size_t (rect.bottom - rect.top))
    {
        //изменять размеры окна не нужно

      flags |= SWP_NOSIZE;
    }

    if ((size_t)window_rect.left == rect.left && (size_t)window_rect.top == rect.top)
    {
        //изменять положение окна не нужно

      flags |= SWP_NOMOVE;
    }

    if (!SetWindowPos (wnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, flags))
      raise_error ("::SetWindowPos");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetWindowRect");
    throw;
  }
}

void Platform::SetClientRect (window_t handle, const Rect& rect)
{
  try
  {
    HWND wnd = (HWND)handle;

    RECT window_rect;

    window_rect.left   = rect.left;
    window_rect.right  = rect.right;
    window_rect.top    = rect.top;
    window_rect.bottom = rect.bottom;

    if (!AdjustWindowRectEx (&window_rect, GetWindowLong (wnd, GWL_STYLE), 0, GetWindowLong (wnd, GWL_EXSTYLE)))
      raise_error ("::AdjustWindowRectEx");

    Rect new_window_rect;

    new_window_rect.left   = window_rect.left;
    new_window_rect.right  = window_rect.right;
    new_window_rect.top    = window_rect.top;
    new_window_rect.bottom = window_rect.bottom;

    SetWindowRect (handle, new_window_rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetClientRect");
    throw;
  }
}

void Platform::GetWindowRect (window_t handle, Rect& rect)
{
  try
  {
    HWND wnd = (HWND)handle;

    RECT window_rect;

    if (!::GetWindowRect (wnd, &window_rect))
      raise_error ("::GetWindowRect");

    rect.left   = window_rect.left;
    rect.right  = window_rect.right;
    rect.top    = window_rect.top;
    rect.bottom = window_rect.bottom;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::GetWindowRect");
    throw;
  }
}

void Platform::GetClientRect (window_t handle, Rect& rect)
{
  try
  {
    HWND wnd = (HWND)handle;

    RECT window_rect;

    if (!::GetClientRect (wnd, &window_rect))
      raise_error ("::GetClientRect");

    rect.left   = window_rect.left;
    rect.right  = window_rect.right;
    rect.top    = window_rect.top;
    rect.bottom = window_rect.bottom;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::GetClientRect");
    throw;
  }
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  HWND wnd = (HWND)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible: //видимость окна
        ShowWindow (wnd, state ? SW_SHOW : SW_HIDE);
        SetLastError (0);  //подавление ошибки Win32 error 126. Не найден указанный модуль. Возникающей при запуске fraps.
//        check_errors ("::ShowWindow");
        break;
      case WindowFlag_Active: //активность окна
        if (state)
        {
          if (!SetActiveWindow (wnd))
            raise_error ("::SetActiveWindow");
        }
        else
        {
          ShowWindow (wnd, SW_MINIMIZE);
          check_errors ("::ShowWindow");
        }

        break;
      case WindowFlag_Focus: //фокус ввода
        if (!SetFocus (state ? wnd : HWND_TOP))
          raise_error ("::SetFocus");
        break;
      case WindowFlag_Maximized:
        if (state)
        {
          ShowWindow (wnd, SW_MAXIMIZE);
        }
        else
        {
          ShowWindow (wnd, SW_SHOW);
        }

        break;
      case WindowFlag_Minimized:
        if (state)
        {
          ShowWindow (wnd, SW_MINIMIZE);
        }
        else
        {
          ShowWindow (wnd, SW_SHOW);
        }
        break;
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
        break;
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetWindowFlag");
    throw;
  }
}

bool Platform::GetWindowFlag (window_t handle, WindowFlag flag)
{
  HWND wnd = (HWND)handle;

  try
  {
    switch (flag)
    {
      case WindowFlag_Visible:
      {
        bool result = (GetWindowLong (wnd, GWL_STYLE) & WS_VISIBLE) != 0;

//        check_errors ("::GetWindowLong");

        return result;
      }
      case WindowFlag_Active:
      {
        HWND active_wnd = GetActiveWindow ();

        check_errors ("::GetActiveWindow");

        return active_wnd == wnd;
      }
      case WindowFlag_Focus:
      {
        HWND focus_wnd = GetFocus ();

        check_errors ("::GetFocus");

        return focus_wnd == wnd;
      }
      case WindowFlag_Minimized:
      case WindowFlag_Maximized:
        throw xtl::format_operation_exception ("", "Can't get window flag %d value", flag);
      default:
        throw xtl::make_argument_exception ("", "flag", flag);
    }
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::GetWindowFlag");
    throw;
  }

  return false;
}

/*
    Установка родительского окна
*/

void Platform::SetParentWindow (window_t child, window_t parent)
{
  try
  {
    if (!SetParent ((HWND)child, (HWND)parent))
      raise_error ("::SetParent");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetParentWindow");
    throw;
  }
}

Platform::window_t Platform::GetParentWindow (window_t child)
{
  try
  {
    HWND parent = GetParent ((HWND)child);

    check_errors ("::GetParent");

    return (window_t)parent;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::GetParentWindow");
    throw;
  }
}

/*
    Обновление окна
*/

void Platform::InvalidateWindow (window_t handle)
{
  HWND wnd = (HWND)handle;

  try
  {
    if (!InvalidateRect (wnd, 0, FALSE))
      raise_error ("::InvalidateRect");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::InvalidateWindow");
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
    if (!SetCursorPos (position.x, position.y))
      raise_error  ("::SetCursorPos");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetCursorPosition(const Point&)");
    throw;
  }
}

Point Platform::GetCursorPosition ()
{
  try
  {
    POINT position;

    if (!GetCursorPos (&position))
      raise_error ("::GetCursorPos");

    return Point (position.x, position.y);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::GetCursorPosition()");
    throw;
  }
}

//в клиентских координатах окна
void Platform::SetCursorPosition (window_t handle, const Point& client_position)
{
  try
  {
    HWND wnd = (HWND)handle;

    POINT screen_position = { client_position.x, client_position.y };

    if (!ClientToScreen (wnd, &screen_position))
      raise_error ("::ClientToScreen");

    if (!SetCursorPos (screen_position.x, screen_position.y))
      raise_error ("::SetCursorPos");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetCursorPosition(window_t, const Point&)");
    throw;
  }
}

//в клиентских координатах окна
Point Platform::GetCursorPosition (window_t handle)
{
  try
  {
    HWND wnd = (HWND)handle;

    POINT position;

    if (!GetCursorPos (&position))
      raise_error ("::GetCursorPos");

    if (!ScreenToClient (wnd, &position))
      raise_error ("::ScreenToClient");

    return Point (position.x, position.y);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::GetCursorPosition(window_t)");
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
    HWND        wnd  = (HWND)handle;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));

    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");

    impl->is_cursor_visible = state;

      //посылка WM_SETCURSOR

    POINT position;

    if (!GetCursorPos (&position))
      raise_error ("::GetCursorPos");

    if (!SetCursorPos (position.x, position.y))
      raise_error ("::SetCursorPos");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::SetCursorVisible");
    throw;
  }
}

bool Platform::GetCursorVisible (window_t handle)
{
  try
  {
    HWND        wnd  = (HWND)handle;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));

    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");

    return impl->is_cursor_visible;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::GetCursorVisible");
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
    if (!file_name)
      throw xtl::make_null_argument_exception ("", "file_name");
    
    if (hotspot_x != -1)
      throw xtl::format_not_supported_exception ("", "Custom hotspot_x=%d not supported", hotspot_x);
      
    if (hotspot_y != -1)
      throw xtl::format_not_supported_exception ("", "Custom hotspot_y=%d not supported", hotspot_y);
      
    common::TempFile cursor_file (file_name);
      
    HANDLE cursor = LoadImageA (0, cursor_file.Path (), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
    
    if (!cursor)
      raise_error ("::LoadImageA");

    return reinterpret_cast<Platform::cursor_t> (cursor);      
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::CreateCursor");
    throw;
  }
}

void Platform::DestroyCursor (cursor_t cursor)
{
  try
  {
    if (GetCursor () == reinterpret_cast<HCURSOR> (cursor))
      ::SetCursor (LoadCursor (0, IDC_ARROW));
//      throw xtl::format_operation_exception ("", "Can't destroy active cursor");

    if (!::DestroyCursor (reinterpret_cast<HCURSOR> (cursor)))
      raise_error ("::DestroyCursor");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Win32Platform::DestroyCursor");
    throw;
  }
}

void Platform::SetCursor (window_t window, cursor_t cursor)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");

    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));

    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");    
    
    impl->preferred_cursor = reinterpret_cast<HCURSOR> (cursor);

    if (impl->is_cursor_visible)
      ::SetCursor (impl->preferred_cursor ? impl->preferred_cursor : impl->default_cursor);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Win32Platform::SetCursor");
    throw;
  }
}

/*
    Цвет фона
*/

void Platform::SetBackgroundColor (window_t window, const Color& color)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");
      
    if (impl->background_brush)
    {
      DeleteObject (impl->background_brush);
      impl->background_brush = 0;
    }
    
    impl->background_brush = CreateSolidBrush (RGB (color.red, color.green, color.blue));
    
    if (!impl->background_brush)
      raise_error ("::CreateSolidBrush");
      
    impl->background_color = color; 
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Win32Platform::SetBackgroundColor");
    throw;
  }
}

void Platform::SetBackgroundState (window_t window, bool state)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");
      
    impl->background_state = state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Win32Platform::SetBackgroundState");
    throw;
  }
}

Color Platform::GetBackgroundColor (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");
      
    return impl->background_color;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Win32Platform::GetBackgroundColor");
    throw;
  }
}

bool Platform::GetBackgroundState (window_t window)
{
  try
  {
    if (!window)
      throw xtl::make_null_argument_exception ("", "window");
      
    HWND        wnd  = (HWND)window;
    WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));
    
    if (!impl)
      throw xtl::format_operation_exception ("", "Null GWL_USERDATA");
      
    return impl->background_state;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Win32Platform::GetBackgroundState");
    throw;
  }
}
