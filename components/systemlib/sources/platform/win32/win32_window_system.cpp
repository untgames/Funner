#include "shared.h"

#include <stdio.h>

using namespace syslib;
using namespace common;

namespace
{

const char* WINDOW_CLASS_NAME = "Default window class";

/*
    Описание реализации окна
*/

struct WindowImpl
{
  void*                               user_data;       //указатель на пользовательские данные
  Platform::WindowMessageHandler message_handler; //функция обработки сообщений окна
  
  WindowImpl (Platform::WindowMessageHandler handler, void* in_user_data) : 
      user_data (in_user_data), message_handler (handler) {}
  
  void Notify (WindowEvent event, const WindowEventContext& context)
  {
    message_handler (event, context, user_data);
  }
};

/*
    Получение контекста события
*/

void GetEventContext (HWND wnd, WindowEventContext& context)
{
  RECT window_rect;

  memset (&context, 0, sizeof (context));

  context.handle = wnd;
  
  GetWindowRect (wnd, &window_rect);
  
  context.window_rect.left   = window_rect.left;
  context.window_rect.right  = window_rect.right;
  context.window_rect.top    = window_rect.top;
  context.window_rect.bottom = window_rect.bottom;
  
  POINT cursor_position;
  
  GetCursorPos (&cursor_position);
  
  context.cursor_position.x = cursor_position.x - window_rect.left;
  context.cursor_position.y = cursor_position.y - window_rect.top;
  
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
    Key_F1, Key_F2, Key_F3, Key_F4, Key_F5, Key_F6, Key_F7, Key_F8, //0x70-0x78
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

  return vkey < sizeof (map) / sizeof (*map) ? (Key)map [vkey] : Key_Unknown;
}

/*
    Функция обработки сообщений окна
*/

LRESULT CALLBACK WindowMessageHandler (HWND wnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    //получение указателя на пользовательские данные
    
  WindowImpl* impl = reinterpret_cast<WindowImpl*> (GetWindowLong (wnd, GWL_USERDATA));

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

      impl->Notify (WindowEvent_OnChangeHandle, context);      

      return 0;
    }

    return DefWindowProc (wnd, message, wparam, lparam);
  }

    //получение контекста события

  WindowEventContext context;

  GetEventContext (wnd, context);  
  
    //обработка сообщений
    
  switch (message)
  {
    case WM_DESTROY: //уничтожение окна
      impl->Notify (WindowEvent_OnDestroy, context);
      
      delete impl;
      
      SetWindowLong (wnd, GWL_USERDATA, 0);

      return 0;
    case WM_CLOSE: //попытка закрытия окна
      impl->Notify (WindowEvent_OnClose, context);
      return 0;
    case WM_ACTIVATE: //активация/деактивация окна
      switch (wparam)
      {
        case WA_ACTIVE:
        case WA_CLICKACTIVE:
          impl->Notify (WindowEvent_OnActivate, context);
          break;
        case WA_INACTIVE:
          impl->Notify (WindowEvent_OnDeactivate, context);
          break;
      }

      return 0;
    case WM_SHOWWINDOW: //изменение видимости
      impl->Notify (wparam ? WindowEvent_OnShow : WindowEvent_OnHide, context);
      return 0;
    case WM_SETFOCUS: //получение фокуса ввода
      impl->Notify (WindowEvent_OnSetFocus, context);
      return 0;
    case WM_KILLFOCUS: //потеря фокуса ввода
      impl->Notify (WindowEvent_OnLostFocus, context);
      return 0;
    case WM_PAINT: //необходима перерисовка
    {
      PAINTSTRUCT ps;      

      BeginPaint (wnd, &ps);

      impl->Notify (WindowEvent_OnPaint, context);

      EndPaint (wnd, &ps);

      return 0;
    }
    case WM_SIZE: //изменение размеров окна
      impl->Notify (WindowEvent_OnSize, context);
      return 0;
    case WM_MOVE: //изменение положения окна
      impl->Notify (WindowEvent_OnMove, context);
      return 0;
    case WM_MOUSEMOVE: //изменение положения курсора над областью окна
      impl->Notify (WindowEvent_OnMouseMove, context);
      return 0;
    case WM_MOUSEWHEEL: //изменилось положение колеса мыши
      context.mouse_wheel_delta = float (GET_WHEEL_DELTA_WPARAM (wparam)) / float (WHEEL_DELTA);
      impl->Notify (WindowEvent_OnMouseWheel, context);
      return 0;
    case WM_LBUTTONDOWN: //нажата левая кнопка мыши
      impl->Notify (WindowEvent_OnLeftButtonDown, context);
      return 0;
    case WM_LBUTTONUP: //отпущена левая кнопка мыши
      impl->Notify (WindowEvent_OnLeftButtonUp, context);
      return 0;
    case WM_LBUTTONDBLCLK: //двойной щелчок левой кнопкой мыши
      impl->Notify (WindowEvent_OnLeftButtonDoubleClick, context);
      return 0;
    case WM_RBUTTONDOWN: //нажата правая кнопка мыши
      impl->Notify (WindowEvent_OnRightButtonDown, context);
      return 0;
    case WM_RBUTTONUP: //отпущена правая кнопка мыши
      impl->Notify (WindowEvent_OnRightButtonUp, context);
      return 0;
    case WM_RBUTTONDBLCLK: //двойной щелчок правой кнопкой мыши
      impl->Notify (WindowEvent_OnRightButtonDoubleClick, context);
      return 0;
    case WM_MBUTTONDOWN: //нажата средняя кнопка мыши
      impl->Notify (WindowEvent_OnMiddleButtonDown, context);
      return 0;
    case WM_MBUTTONUP: //отпущена средняя кнопка мыши
      impl->Notify (WindowEvent_OnMiddleButtonUp, context);
      return 0;
    case WM_MBUTTONDBLCLK: //двойной щелчок средней кнопкой мыши
      impl->Notify (WindowEvent_OnMiddleButtonDoubleClick, context);
      return 0;
    case WM_KEYDOWN: //нажата клавиша клавиатуры
      context.key = VirtualKey2SystemKey (wparam);
      impl->Notify (WindowEvent_OnKeyDown, context);
      return 0;
    case WM_KEYUP: //отпущена клавиша клавиатуры
      context.key = VirtualKey2SystemKey (wparam);
      impl->Notify (WindowEvent_OnKeyUp, context);      
      return 0;
    case WM_UNICHAR: //в буфере окна появился символ
      context.char_code = (wchar_t)wparam;
      impl->Notify (WindowEvent_OnChar, context);
      return 0;
  }

    //обработка сообщений по умолчанию

  return DefWindowProc (wnd, message, wparam, lparam);
}

/*
    Получение экземпляра приложения
*/

HINSTANCE GetApplicationInstance ()
{
  return GetModuleHandle (0);
}

/*
    Регистрация оконного класса
*/

void RegisterWindowClass ()
{
  WNDCLASS wc;
  
  memset (&wc, 0, sizeof (wc));

  wc.style         = CS_HREDRAW | CS_VREDRAW;
  wc.lpfnWndProc   = &WindowMessageHandler;
  wc.hInstance     = GetApplicationInstance ();
  wc.hIcon         = LoadIcon (GetApplicationInstance (), IDI_APPLICATION);  
  wc.hCursor       = LoadCursor (GetApplicationInstance (), IDC_ARROW);
  wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
  wc.lpszClassName = WINDOW_CLASS_NAME;  

  if (!RegisterClass (&wc))
    CheckErrors ("syslib::RegisterWindowClass");
}

}

/*
    Создание/закрытие/уничтожение окна
*/

#undef CreateWindow

Platform::window_t Platform::CreateWindow (WindowStyle style, WindowMessageHandler handler, void* user_data)
{
    //определене стиля окна
    
  UINT win_style;  
    
  switch (style)
  {
    case WindowStyle_Overlapped:
      win_style = WS_OVERLAPPEDWINDOW;
      break;
    case WindowStyle_PopUp:
      win_style = WS_POPUP;
      break;
    default:
      RaiseInvalidArgument ("syslib::Win32Platform::CreateWindow", "style", style);
      return 0;
  }

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
 
  WindowImpl* window_impl = new WindowImpl (handler, user_data);

  try
  {    
    HWND wnd = CreateWindowA (WINDOW_CLASS_NAME, "", win_style, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
                              0, 0, GetApplicationInstance (), window_impl);
                                
    if (!wnd)
      throw WinAPIException ("syslib::Win32Platform::CreateWindow", "Error at create window");

    CheckErrors  ("syslib::Win32Platform::CreateWindow");    
    ShowWindow   (wnd, SW_SHOW);
    UpdateWindow (wnd);
    CheckErrors  ("syslib::Win32Platform::CreateWindow");    

    return (window_t)wnd;
  }
  catch (...)
  {
    delete window_impl;
    throw;
  }  
}

void Platform::CloseWindow (window_t handle)
{
  HWND wnd = (HWND)handle;
  
  PostMessage (wnd, WM_CLOSE, 0, 0);
  CheckErrors ("syslib::Win32Platform::CloseWindow");
}

void Platform::DestroyWindow (window_t handle)
{
  HWND wnd = (HWND)handle;

  ::DestroyWindow (wnd);
  CheckErrors ("syslib::Win32Platform::DestroyWindow");
}
    
/*
    Заголовок окна
*/

void Platform::SetWindowTitle (window_t handle, const char* title)
{
  HWND wnd = (HWND)handle;  
  
  SetWindowText (wnd, title);
  CheckErrors ("syslib::Win32Platform::SetWindowTitle");
}

void Platform::GetWindowTitle (window_t handle, size_t buffer_size, char* buffer)
{
  HWND wnd = (HWND)handle;
  
  GetWindowText (wnd, buffer, buffer_size);
  CheckErrors ("syslib::Win32Platform::GetWindowTitle");
}

/*
    Область окна / клиентская область
*/

void Platform::SetWindowRect (window_t handle, const Rect& rect)
{
  HWND wnd = (HWND)handle;
  
  RECT window_rect;
  
  ::GetWindowRect (wnd, &window_rect);  
  CheckErrors     ("syslib::Win32Platform::SetWindowRect");
  
  UINT flags = SWP_NOACTIVATE;
  
  if (window_rect.right - window_rect.left == rect.right - rect.left &&
      window_rect.bottom - window_rect.top == rect.bottom - rect.top)
  {
      //изменять размеры окна не нужно

    flags |= SWP_NOSIZE;
  }
  
  if (window_rect.left == rect.left && window_rect.top == rect.top)
  {
      //изменять положение окна не нужно

    flags |= SWP_NOMOVE;
  }

  SetWindowPos (wnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, flags);
  CheckErrors  ("syslib::Win32Platform::SetWindowRect");
}

void Platform::GetWindowRect (window_t handle, Rect& rect)
{
  HWND wnd = (HWND)handle;
  
  RECT window_rect;
  
  ::GetWindowRect (wnd, &window_rect);  
  CheckErrors     ("syslib::Win32Platform::GetWindowRect");
  
  rect.left   = window_rect.left;
  rect.right  = window_rect.right;
  rect.top    = window_rect.top;
  rect.bottom = window_rect.bottom;
}

void Platform::GetClientRect (window_t handle, Rect& rect)
{
  HWND wnd = (HWND)handle;
  
  RECT window_rect;
  
  ::GetClientRect (wnd, &window_rect);  
  CheckErrors     ("syslib::Win32Platform::GetClientRect");
  
  rect.left   = window_rect.left;
  rect.right  = window_rect.right;
  rect.top    = window_rect.top;
  rect.bottom = window_rect.bottom;
}

/*
    Установка флагов окна
*/

void Platform::SetWindowFlag (window_t handle, WindowFlag flag, bool state)
{
  HWND wnd = (HWND)handle;
  
  switch (flag)
  {
    case WindowFlag_Visible: //видимость окна
      ShowWindow (wnd, state ? SW_SHOW : SW_HIDE);
      break;
    case WindowFlag_Active: //активность окна
      if (state) SetActiveWindow (wnd);
      else       ShowWindow (wnd, SW_MINIMIZE);

      break;
    case WindowFlag_Focus: //фокус ввода
      SetFocus (state ? wnd : HWND_TOP);
      break;
    default:
      RaiseInvalidArgument ("syslib::Win32Platform::SetWindowFlag", "flag", flag);
  }

  CheckErrors ("syslib::Win32Platform::SetWindowFlag");
}

bool Platform::GetWindowFlag (window_t handle, WindowFlag flag)
{
  HWND wnd    = (HWND)handle;
  bool result = false;
  
  switch (flag)
  {
    case WindowFlag_Visible:
      result = ShowWindow (wnd, SW_SHOWNA) == TRUE;
      break;
    case WindowFlag_Active:
      result = GetActiveWindow () == wnd;
      break;
    case WindowFlag_Focus:
      result = GetFocus () == wnd;
      break;
    default:
      RaiseInvalidArgument ("syslib::Win32Platform::GetWindowFlag", "flag", flag);
      break;
  }
  
  CheckErrors ("syslib::Win32Platform::GetWindowFlag");
  
  return result;
}

/*
    Положение курсора
*/

void Platform::SetCursorPosition (const Point& position)
{
  SetCursorPos (position.x, position.y);
  CheckErrors  ("syslib::Win32Platform::SetCursorPosition");
}

Point Platform::GetCursorPosition ()
{
  POINT position;
  
  GetCursorPos (&position);
  CheckErrors  ("syslib::Win32Platform::GetCursorPosition");
  
  return Point (position.x, position.y);
}
