#include <syslib/window.h>
#include <platform/platform.h>
#include <xtl/signal.h>
#include <common/exception.h>
#include <common/strlib.h>
#include <common/utf_converter.h>
#include <string.h>

using namespace syslib;
using namespace xtl;
using namespace common;

const size_t MAX_TITLE_LENGTH = 256;

/*
    Описание реализации Window
*/

typedef signal<void (Window&, WindowEvent, const WindowEventContext&)> WindowSignal;

struct Window::Impl
{
  const void*  handle;                             //низкоуровневый дескриптор окна  
  WindowStyle  style;                              //стиль окна
  WindowSignal signals [WindowEvent_Num];          //сигналы окна
  bool         close_cancel_flag;                  //флаг отмены закрытия окна
  char         title [MAX_TITLE_LENGTH+1];         //заголовок окна
  wchar_t      title_unicode [MAX_TITLE_LENGTH+1]; //заголовок окна в Unicode
  LogHandler   debug_log;                          //функция отладочного протоколирования
  
  Impl () : handle (0), style (WindowStyle_Default)
  {
    memset (title, 0, sizeof title);
    memset (title_unicode, 0, sizeof title_unicode);
  }  
};

/*
    Конструктор и деструктор
*/

Window::Window ()
  : impl (new Impl)
  {}

Window::Window (WindowStyle style)
  : impl (new Impl)
{
  try
  {
    Init (style);
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Window::Window (WindowStyle style, size_t width, size_t height)
  : impl (new Impl)
{
  try
  {
    Init (style, width, height);
  }
  catch (...)
  {
    delete impl;
    throw;
  }
}

Window::~Window ()
{
  try
  {
    ForceClose ();
  }
  catch (...)
  {
    //подавляем все исключения
  }

  delete impl;
}

/*
    Обработка событий окна
*/

void Window::MessageHandler (WindowEvent event, const WindowEventContext& context, void* user_data)
{
  Window* window = reinterpret_cast<Window*> (user_data);
  
  if (!window)
    return;    
    
  try
  {
    switch (event)
    {
      case WindowEvent_OnClose: //посылается перед закрытием окна
      {
        window->impl->close_cancel_flag = false;

        window->Notify (WindowEvent_OnClose, context);

          //если закрытие окна не было отменено - закрываем окно

        if (!window->impl->close_cancel_flag)
          window->ForceClose ();

        break;
      }
      case WindowEvent_OnChangeHandle:  //изменение дескриптора окна, фактически создание окна
        window->SetHandle (context.handle);
        break;
      case WindowEvent_OnDestroy: //окно уничтожено
        window->Notify (WindowEvent_OnDestroy);
        window->SetHandle (0);
        break;
      case WindowEvent_OnActivate:                //окно стало активным   
      case WindowEvent_OnDeactivate:              //окно перестало быть активным
      case WindowEvent_OnShow:                    //окно стало видимым
      case WindowEvent_OnHide:                    //окно стало не видимым
      case WindowEvent_OnSetFocus:                //окно получило фокус ввода
      case WindowEvent_OnLostFocus:               //окно потеряло фокус ввода
      case WindowEvent_OnPaint:                   //необходима перерисовка
      case WindowEvent_OnSize:                    //изменились размеры окна
      case WindowEvent_OnMove:                    //изменилось положение окна
      case WindowEvent_OnMouseMove:               //курсор мыши переместился над областью окна
      case WindowEvent_OnMouseVerticalWheel:      //изменилось положение вертикального колеса мыши
      case WindowEvent_OnMouseHorisontalWheel:    //изменилось положение горизонтального колеса мыши
      case WindowEvent_OnLeftButtonDown:          //нажата левая кнопка мыши
      case WindowEvent_OnLeftButtonUp:            //отпущена левая кнопка мыши
      case WindowEvent_OnLeftButtonDoubleClick:   //двойной щелчок левой кнопкой мыши
      case WindowEvent_OnRightButtonDown:         //нажата правая кнопка мыши
      case WindowEvent_OnRightButtonUp:           //отпущена правая кнопка мыши  
      case WindowEvent_OnRightButtonDoubleClick:  //двойной щелчок правой кнопкой мыши
      case WindowEvent_OnMiddleButtonDown:        //нажата средняя кнопка мыши
      case WindowEvent_OnMiddleButtonUp:          //отпущена средняя кнопка мыши      
      case WindowEvent_OnMiddleButtonDoubleClick: //двойной щелчок средней кнопкой мыши
      case WindowEvent_OnXButton1Down:            //нажата первая Х кнопка мыши
      case WindowEvent_OnXButton1Up:              //отпущена первая Х кнопка мыши      
      case WindowEvent_OnXButton1DoubleClick:     //двойной щелчок первой Х кнопкой мыши
      case WindowEvent_OnXButton2Down:            //нажата вторая Х кнопка мыши
      case WindowEvent_OnXButton2Up:              //отпущена вторая Х кнопка мыши      
      case WindowEvent_OnXButton2DoubleClick:     //двойной щелчок второй Х кнопкой мыши
      case WindowEvent_OnKeyDown:                 //нажата клавиша клавиатуры
      case WindowEvent_OnKeyUp:                   //отпущена клавиша клавиатуры
      case WindowEvent_OnChar:                    //в буфере ввода окна появился символ
        window->Notify (event, context);
        break;
      default:
        break;
    }
  }
  catch (...)
  {
    //подавляем все исключения
  }
}

/*
    Создание окна
*/

void Window::Init (WindowStyle style)
{
  switch (style)
  {
    case WindowStyle_Overlapped:
    case WindowStyle_PopUp:
      break;
    default:
      raise_invalid_argument ("syslib::Window::Window", "style", style);
      return;
  }
  
  if (impl->handle)
    ForceClose ();
  
  impl->style  = style;
  
  SetHandle (Platform::CreateWindow (impl->style, &MessageHandler, this));
}

void Window::Init (WindowStyle style, size_t width, size_t height)
{
  Init    (style);
  SetSize (width, height);
}

/*
    Закрытие окна
*/

void Window::Close ()
{
  Platform::CloseWindow ((Platform::window_t)CheckedHandle ());
}

void Window::ForceClose ()
{
  if (!impl->handle)
    return;
    
  Notify (WindowEvent_OnClose);

  Platform::DestroyWindow ((Platform::window_t)impl->handle);
}

//может быть вызван одним из обработчиков события WindowEvent_OnClose
void Window::CancelClose ()
{
  impl->close_cancel_flag = true;
}

bool Window::IsClosed () const
{
  return impl->handle == 0;
}

/*
    Работа с низкоуровневым дескриптором окна
*/

const void* Window::Handle () const
{
  return impl->handle;
}

const void* Window::CheckedHandle () const
{
  if (!impl->handle)
    throw ClosedWindowException ();    

  return impl->handle;
}

void Window::SetHandle (const void* handle)
{
  if (handle == impl->handle)
    return;

  impl->handle = handle;
  
  Notify (WindowEvent_OnChangeHandle);
}

/*
    Стиль окна
*/

WindowStyle Window::Style () const
{
  CheckedHandle (); //для возбуждения исключения в случае закрытого окна
  
  return impl->style;
}

void Window::SetStyle (WindowStyle style)
{
  if (style == impl->style)
    return;
    
  bool is_visible = IsVisible ();
    
  Init (style, Width (), Height ());
  
  if (!is_visible)
    Hide ();
}

/*
    Заголовок окна
*/

const char* Window::Title () const
{
  strncpy (impl->title, tostring (TitleUnicode ()).c_str (), MAX_TITLE_LENGTH);

  return impl->title;
}

const wchar_t* Window::TitleUnicode () const
{
  Platform::GetWindowTitle ((Platform::window_t)CheckedHandle (), MAX_TITLE_LENGTH, impl->title_unicode);

  return impl->title_unicode;
}

void Window::SetTitle (const char* title)
{
  if (!title)
    raise_null_argument ("syslib::Window::SetTitle(const char*)", "title");

  Platform::SetWindowTitle ((Platform::window_t)CheckedHandle (), towstring (title).c_str ());
}

void Window::SetTitle (const wchar_t* title)
{
  if (!title)
    raise_null_argument ("syslib::Window::SetTitle(const wchar_t*)", "title");

  Platform::SetWindowTitle ((Platform::window_t)CheckedHandle (), title);
}

/*
    Область окна / клиентская область
*/

Rect Window::WindowRect () const
{
  Rect rect;

  Platform::GetWindowRect ((Platform::window_t)CheckedHandle (), rect);

  return rect;
}

Rect Window::ClientRect () const
{
  Rect rect;

  Platform::GetClientRect ((Platform::window_t)CheckedHandle (), rect);

  return rect;
}

/*
    Размеры окна
*/

size_t Window::Width () const
{
  Rect rect;

  Platform::GetWindowRect ((Platform::window_t)CheckedHandle (), rect);

  return rect.right - rect.left;
}

size_t Window::Height () const
{
  Rect rect;

  Platform::GetWindowRect ((Platform::window_t)CheckedHandle (), rect);

  return rect.bottom - rect.top;
}

void Window::SetWidth (size_t width)
{
  SetSize (width, Height ());
}

void Window::SetHeight (size_t height)
{
  SetSize (Width (), height);
}

void Window::SetSize (size_t width, size_t height)
{
  Rect rect;

  Platform::GetWindowRect ((Platform::window_t)CheckedHandle (), rect);
  
  rect.right  = rect.left + width;
  rect.bottom = rect.top + height;

  Platform::SetWindowRect ((Platform::window_t)CheckedHandle (), rect);
}

/*
    Положение окна
*/

Point Window::Position () const
{
  Rect rect;

  Platform::GetWindowRect ((Platform::window_t)CheckedHandle (), rect);

  return Point (rect.left, rect.top);
}

void Window::SetPosition (const Point& position)
{
  Rect rect;

  Platform::GetWindowRect ((Platform::window_t)CheckedHandle (), rect);
  
  size_t width  = rect.right - rect.left,
         height = rect.bottom - rect.top;
  
  rect.left   = position.x;
  rect.top    = position.y;
  rect.right  = position.x + width;
  rect.bottom = position.y + height;

  Platform::SetWindowRect ((Platform::window_t)CheckedHandle (), rect);
}

void Window::SetPosition (size_t x, size_t y)
{
  SetPosition (Point (x, y));
}

/*
    Положение курсора
*/

Point Window::CursorPosition () const
{
  Rect window_rect, client_rect;
  
  Platform::GetWindowRect ((Platform::window_t)CheckedHandle (), window_rect);
  Platform::GetClientRect ((Platform::window_t)CheckedHandle (), client_rect);

  Point position = Platform::GetCursorPosition ();

  position.x -= window_rect.left + client_rect.left;
  position.y -= window_rect.top + client_rect.top;

  return position;
}

void Window::SetCursorPosition (const Point& position)
{
  Rect window_rect, client_rect;
  
  Platform::GetWindowRect ((Platform::window_t)CheckedHandle (), window_rect);
  Platform::GetClientRect ((Platform::window_t)CheckedHandle (), client_rect);

  Platform::SetCursorPosition (Point (position.x + client_rect.left + window_rect.left,
                                      position.y + client_rect.top + window_rect.top));
}

void Window::SetCursorPosition (size_t x, size_t y)
{
  SetCursorPosition (Point (x, y));
}

/*
    Активность окна
*/

bool Window::IsActive () const
{
  return Platform::GetWindowFlag ((Platform::window_t)CheckedHandle (), WindowFlag_Active);
}

void Window::SetActive (bool state)
{
  Platform::SetWindowFlag ((Platform::window_t)CheckedHandle (), WindowFlag_Active, state);
}

/*
    Видимость окна
*/

bool Window::IsVisible () const
{
  return Platform::GetWindowFlag ((Platform::window_t)CheckedHandle (), WindowFlag_Visible);
}

void Window::SetVisible (bool state)
{
  Platform::SetWindowFlag ((Platform::window_t)CheckedHandle (), WindowFlag_Visible, state);
}

/*
    Работа с фокусом ввода
*/

bool Window::HasFocus () const
{
  return Platform::GetWindowFlag ((Platform::window_t)CheckedHandle (), WindowFlag_Focus);
}

void Window::SetFocus (bool state)
{
  Platform::SetWindowFlag ((Platform::window_t)CheckedHandle (), WindowFlag_Focus, state);
}

/*
    Обновление содержимого окна (посылка OnPaint)
*/

void Window::Invalidate ()
{
  Platform::InvalidateWindow ((Platform::window_t)CheckedHandle ());
}

/*
    Подписка на события окна
*/

connection Window::RegisterEventHandler (WindowEvent event, const EventHandler& handler)
{
  if (event < 0 || event >= WindowEvent_Num)
    raise_invalid_argument ("syslib::Window::RegisterEventHandler", "event", event);
    
  return impl->signals [event].connect (handler);
}

/*
    Установка пользовательской функции отладочного протоколирования
*/

namespace
{

//функция протоколирования по умолчанию
void default_debug_log (const char*)
{
}

}

void Window::SetDebugLog (const LogHandler& debug_log)
{
  impl->debug_log = debug_log ? debug_log : default_debug_log;
}

const Window::LogHandler& Window::DebugLog () const
{
  return impl->debug_log;
}

/*
    Оповещение о событиях окна
*/

void Window::Notify (WindowEvent event)
{
  WindowEventContext context;
  
  memset (&context, 0, sizeof (context));

  context.handle = impl->handle;

  Notify (event, context);
}

void Window::Notify (WindowEvent event, const WindowEventContext& context)
{
  try
  {
    try
    {
      impl->signals [event] (*this, event, context);
    }
    catch (std::exception& exception)
    {
      impl->debug_log (format ("exception at syslib::Window::Notify: %s", exception.what ()).c_str ());
    }
    catch (...)
    {
      impl->debug_log ("unknown exception at syslib::Window::Notify");
    }
  }
  catch (...)
  {
    //подавляем все исключения
  }
}
