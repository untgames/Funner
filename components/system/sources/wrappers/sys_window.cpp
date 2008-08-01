#include "shared.h"

using namespace syslib;
using namespace xtl;
using namespace common;

const size_t MAX_TITLE_LENGTH = 256;

/*
    Описание реализации Window
*/

struct Window::Impl
{
  public:
///Конструктор
    Impl (Window* in_window) : window (in_window), handle (0), parent_handle (0), style (WindowStyle_Default)
    {
      memset (title, 0, sizeof title);
      memset (title_unicode, 0, sizeof title_unicode);
    }
    
///Инциализация окна
    void Init (WindowStyle in_style, Platform::window_t parent, bool is_visible)
    {
      try
      {
          //сделать через конструктор и Hide/Show
          
        switch (in_style)
        {
          case WindowStyle_Overlapped:
          case WindowStyle_PopUp:
            break;
          default:
            throw xtl::make_argument_exception ("syslib::Window::Impl::Init", "style", in_style);
            return;
        }

        if (handle)
          ForceClose ();

        style         = in_style;
        parent_handle = parent;
        
        Platform::window_t new_handle = Platform::CreateWindow (style, &MessageHandler, parent, this);        

        SetHandle (new_handle);

        if (is_visible)
          window->SetVisible (true);
      }
      catch (xtl::exception& exception)
      {
        exception.touch ("syslib::Window::Impl::Init");
        throw;
      }
    }

///Отмена закрытия окна
    void CancelClose ()
    {
      close_cancel_flag = true;      
    }

///Принудительное закрытие окна
    void ForceClose ()
    {
      if (!handle)
        return;
        
      Notify (WindowEvent_OnClose);      

      Platform::DestroyWindow (handle);
    }
    
///Стиль окна
    WindowStyle Style () const { return style; }

///Низкоуровневый дескриптор окна
    Platform::window_t Handle () const { return handle; }

    Platform::window_t CheckedHandle () const
    {
      if (!handle)
        throw xtl::message_exception<ClosedWindowException> ("syslib::Window::Impl::CheckedHandle", "Closed window exception");

      return handle;
    }    

///Установка низкоуровневого дескриптора окна
    void SetHandle (Platform::window_t new_handle)
    {
      if (handle == new_handle)
        return;

      handle = new_handle;

      Notify (WindowEvent_OnChangeHandle);
    }
    
///Низкоуровневый дескриптор родительского окна
    Platform::window_t ParentHandle () const { return parent_handle; }

///Установка низкоуровневого дескриптора родительского окна
    void SetParentHandle (Platform::window_t new_parent_handle)
    {
      bool need_window_recreate = !new_parent_handle && parent_handle || new_parent_handle && !parent_handle; //нужно пересоздавать окно      

      if (need_window_recreate)
      {
        Init (style, new_parent_handle, window->IsVisible ());
      }      
      else
      {
        parent_handle = new_parent_handle;

        Platform::SetParentWindow (CheckedHandle (), parent_handle);
      }
    }

///Заголовок окна
    const char* Title ()
    {
      strncpy (title, tostring (TitleUnicode ()).c_str (), MAX_TITLE_LENGTH);

      return title;
    }

///Заголовок окна в Unicode
    const wchar_t* TitleUnicode ()
    {
      Platform::GetWindowTitle (CheckedHandle (), MAX_TITLE_LENGTH, title_unicode);

      return title_unicode;
    }
    
///Установка пользовательской функции отладочного протоколирования
    void SetDebugLog (const LogHandler& handler)
    {
      debug_log = handler;
    }

///Получение пользовательской функции отладочного протоколирования
    const LogHandler& DebugLog () const { return debug_log; }

///Регистрация обработчиков событий
    xtl::connection RegisterEventHandler (WindowEvent event, const EventHandler& handler)
    {    
      if (event < 0 || event >= WindowEvent_Num)
        throw xtl::make_argument_exception ("syslib::Window::Impl::RegisterEventHandler", "event", event);

      return signals [event].connect (handler);
    }    

  private:  
///Оповещение о возникновении события
    void Notify (WindowEvent event, const WindowEventContext& context)
    {
      try
      {
        try
        {
          signals [event] (*window, event, context);
        }
        catch (std::exception& exception)
        {
          if (debug_log)
            debug_log (format ("exception at syslib::Window::Impl::Notify: %s", exception.what ()).c_str ());
        }
        catch (...)
        {
          if (debug_log)
            debug_log ("unknown exception at syslib::Window::Impl::Notify");
        }
      }
      catch (...)
      {
        //подавляем все исключения
      }    
    }

    void Notify (WindowEvent event)
    {
      WindowEventContext context;

      memset (&context, 0, sizeof (context));

      context.handle = handle;

      Notify (event, context);    
    }

///Обработчик событий окна
    static void MessageHandler (Platform::window_t wnd, WindowEvent event, const WindowEventContext& context, void* user_data)
    {
      Impl* impl = reinterpret_cast<Impl*> (user_data);
      
      if (!impl || wnd != impl->handle)
        return;

      try
      {
        switch (event)
        {
          case WindowEvent_OnClose: //посылается перед закрытием окна
          {
            impl->close_cancel_flag = false;

            impl->Notify (WindowEvent_OnClose, context);

              //если закрытие окна не было отменено - закрываем окно

            if (!impl->close_cancel_flag)
              impl->ForceClose ();

            break;
          }
          case WindowEvent_OnChangeHandle:  //изменение дескриптора окна, фактически создание окна
            impl->SetHandle ((Platform::window_t)context.handle);
            break;
          case WindowEvent_OnDestroy: //окно уничтожено
            impl->Notify (WindowEvent_OnDestroy);
            impl->SetHandle (0);
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
            impl->Notify (event, context);
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
    
  private:
    typedef xtl::signal<void (Window&, WindowEvent, const WindowEventContext&)> WindowSignal;

  private:
    Window*            window;                             //указатель на владельца
    Platform::window_t handle;                             //низкоуровневый дескриптор окна
    Platform::window_t parent_handle;                      //низкоуровневый дескриптор родительского окна
    WindowStyle        style;                              //стиль окна
    WindowSignal       signals [WindowEvent_Num];          //сигналы окна
    bool               close_cancel_flag;                  //флаг отмены закрытия окна
    char               title [MAX_TITLE_LENGTH+1];         //заголовок окна
    wchar_t            title_unicode [MAX_TITLE_LENGTH+1]; //заголовок окна в Unicode
    LogHandler         debug_log;                          //функция отладочного протоколирования
};

/*
    Конструктор и деструктор
*/

Window::Window ()
  : impl (new Impl (this))
  {}

Window::Window (WindowStyle style)
  : impl (new Impl (this))
{
  try
  {
    Init (style);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Window(WindowStyle)");
    throw;
  }
}

Window::Window (WindowStyle style, size_t width, size_t height)
  : impl (new Impl (this))
{
  try
  {
    Init (style, width, height);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Window(WindowStyle,size_t,size_t)");
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
}

/*
    Создание окна
*/

void Window::Init (WindowStyle style)
{
  try
  {
    impl->Init (style, impl->ParentHandle (), false);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Init(WindowStyle)");
    throw;
  }
}

void Window::Init (WindowStyle style, size_t width, size_t height)
{
  try
  {
    Init    (style);
    SetSize (width, height);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Init(WindowStyle,size_t,size_t)");
    throw;
  }
}

/*
    Закрытие окна
*/

void Window::Close ()
{
  try
  {
    Platform::CloseWindow (impl->CheckedHandle ());    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Close");
    throw;
  }
}

void Window::ForceClose ()
{
  try
  {
    impl->ForceClose ();    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::ForceClose");
    throw;
  }
}

//может быть вызван одним из обработчиков события WindowEvent_OnClose
void Window::CancelClose ()
{
  impl->CancelClose ();
}

bool Window::IsClosed () const
{
  return impl->Handle () == 0;
}

/*
    Работа с низкоуровневым дескриптором окна
*/

const void* Window::Handle () const
{
  return impl->Handle ();
}

/*
    Стиль окна
*/

WindowStyle Window::Style () const
{
  try
  {
    impl->CheckedHandle (); //для возбуждения исключения в случае закрытого окна
  
    return impl->Style ();    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Style");
    throw;
  }
}

void Window::SetStyle (WindowStyle style)
{
  try
  {
    if (style == impl->Style ())
      return;
      
    impl->Init (style, impl->ParentHandle (), IsVisible ());    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetStyle");
    throw;
  }
}

/*
    Заголовок окна
*/

const char* Window::Title () const
{
  try
  {
    return impl->Title ();    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Title");
    throw;
  }
}

const wchar_t* Window::TitleUnicode () const
{
  try
  {
    return impl->TitleUnicode ();    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::TitleUnicode");
    throw;
  }
}

void Window::SetTitle (const char* title)
{
  try
  {
    if (!title)
      throw xtl::make_null_argument_exception ("", "title");

    Platform::SetWindowTitle (impl->CheckedHandle (), towstring (title).c_str ());    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetTitle(const char*)");
    throw;
  }
}

void Window::SetTitle (const wchar_t* title)
{
  try
  {
    if (!title)
      throw xtl::make_null_argument_exception ("", "title");

    Platform::SetWindowTitle (impl->CheckedHandle (), title);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetTitle(const wchar_t*)");
    throw;
  }
}

/*
    Область окна / клиентская область
*/

Rect Window::WindowRect () const
{
  try
  {
    Rect rect;

    Platform::GetWindowRect (impl->CheckedHandle (), rect);

    return rect;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::WindowRect");
    throw;
  }
}

Rect Window::ClientRect () const
{
  try
  {
    Rect rect;

    Platform::GetClientRect (impl->CheckedHandle (), rect);

    return rect;    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::ClientRect");
    throw;
  }
}

/*
    Размеры окна
*/

size_t Window::Width () const
{
  try
  {
    Rect rect;

    Platform::GetWindowRect (impl->CheckedHandle (), rect);

    return rect.right - rect.left;    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Width");
    throw;
  }
}

size_t Window::Height () const
{
  try
  {
    Rect rect;

    Platform::GetWindowRect (impl->CheckedHandle (), rect);

    return rect.bottom - rect.top;    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Height");
    throw;
  }
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
  try
  {
    Rect rect;

    Platform::GetWindowRect (impl->CheckedHandle (), rect);
    
    rect.right  = rect.left + width;
    rect.bottom = rect.top + height;

    Platform::SetWindowRect (impl->CheckedHandle (), rect);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetSize");
    throw;
  }
}

/*
    Положение окна
*/

Point Window::Position () const
{
  try
  {
    Rect rect;

    Platform::GetWindowRect (impl->CheckedHandle (), rect);

    return Point (rect.left, rect.top);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Position");
    throw;
  }
}

void Window::SetPosition (const Point& position)
{
  try
  {
    Rect rect;

    Platform::GetWindowRect (impl->CheckedHandle (), rect);
    
    size_t width  = rect.right - rect.left,
           height = rect.bottom - rect.top;
    
    rect.left   = position.x;
    rect.top    = position.y;
    rect.right  = position.x + width;
    rect.bottom = position.y + height;

    Platform::SetWindowRect (impl->CheckedHandle (), rect);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetPosition");
    throw;
  }
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
  try
  {
    return Platform::GetCursorPosition (impl->CheckedHandle ());    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::CursorPosition");
    throw;
  }
}

void Window::SetCursorPosition (const Point& position)
{
  try
  {
    Platform::SetCursorPosition (impl->CheckedHandle (), position);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetCursorPosition");
    throw;
  }
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
  try
  {
    return Platform::GetWindowFlag (impl->CheckedHandle (), WindowFlag_Active);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::IsActive");
    throw;
  }
}

void Window::SetActive (bool state)
{
  try
  {
    Platform::SetWindowFlag (impl->CheckedHandle (), WindowFlag_Active, state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetActive");
    throw;
  }
}

/*
    Видимость окна
*/

bool Window::IsVisible () const
{
  try
  {
    return Platform::GetWindowFlag (impl->CheckedHandle (), WindowFlag_Visible);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::IsVisible");
    throw;
  }
}

void Window::SetVisible (bool state)
{
  try
  {
    Platform::SetWindowFlag (impl->CheckedHandle (), WindowFlag_Visible, state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetVisible");
    throw;
  }
}

/*
    Работа с фокусом ввода
*/

bool Window::HasFocus () const
{
  try
  {
    return Platform::GetWindowFlag (impl->CheckedHandle (), WindowFlag_Focus);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::HasFocus");
    throw;
  }
}

void Window::SetFocus (bool state)
{
  try
  {
    Platform::SetWindowFlag (impl->CheckedHandle (), WindowFlag_Focus, state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetFocus");
    throw;
  }
}

/*
    Дескриптор родительского окна
*/

void Window::SetParentHandle (const void* handle)
{
  try
  {
    impl->SetParentHandle ((Platform::window_t)handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetParentHandle");
    throw;
  }
}

const void* Window::ParentHandle () const
{
  try
  {
    return Platform::GetParentWindow (impl->CheckedHandle ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::ParentHandle");
    throw;
  }
}

/*
    Обновление содержимого окна (посылка OnPaint)
*/

void Window::Invalidate ()
{
  try
  {
    Platform::InvalidateWindow (impl->CheckedHandle ());    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Invalidate");
    throw;
  }
}

/*
    Подписка на события окна
*/

connection Window::RegisterEventHandler (WindowEvent event, const EventHandler& handler)
{
  try
  {
    return impl->RegisterEventHandler (event, handler);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::RegisterEventHandler");
    throw;
  }
}

/*
    Установка пользовательской функции отладочного протоколирования
*/

void Window::SetDebugLog (const LogHandler& debug_log)
{
  try
  {
    impl->SetDebugLog (debug_log);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetDebugLog");
    throw;
  }
}

const Window::LogHandler& Window::DebugLog () const
{
  return impl->DebugLog ();
}
