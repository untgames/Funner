#include "shared.h"

using namespace syslib;
using namespace xtl;
using namespace common;

const unsigned int MAX_TITLE_LENGTH = 256;

/*
    Описание реализации Window
*/

struct Window::Impl: public xtl::trackable
{
  public:
///Конструктор
    Impl (Window* in_window, const char* in_init_string)
      : window (in_window)
      , handle (0)
      , parent_handle (0)
      , embedded (false)
      , style (WindowStyle_Default)
      , need_update_viewport (true)
    {
      if (!in_init_string)
        throw xtl::make_null_argument_exception ("syslib::Window::Window::Impl", "init_string");

      init_string = in_init_string;

      memset (title, 0, sizeof title);
      memset (title_unicode, 0, sizeof title_unicode);
    }
    
    Impl (Window* in_window, window_t in_handle)
      : window (in_window)
      , handle (in_handle)
      , parent_handle (0)
      , embedded (true)
      , style (WindowStyle_PopUp)
      , need_update_viewport (true)
    {
      if (!handle)
        throw xtl::make_null_argument_exception ("syslib::Window::Window::Impl", "handle");
           
      memset (title, 0, sizeof title);
      memset (title_unicode, 0, sizeof title_unicode);
    }    
    
///Инциализация окна
    void Init (WindowStyle in_style, const void* parent, bool is_visible, const Rect* window_rect = 0)
    {
      try
      {
        if (embedded)
          throw xtl::format_operation_exception ("", "Can't recreate embedded window");

          //сделать через конструктор и Hide/Show
          
        switch (in_style)
        {
          case WindowStyle_Overlapped:
          case WindowStyle_PopUp:
            break;
          default:
            throw xtl::make_argument_exception ("", "style", in_style);
        }

        if (handle)
          ForceClose ();

        style         = in_style;
        parent_handle = parent;

        window_t new_handle = Platform::CreateWindow (style, &MessageHandler, parent, init_string.c_str (), this);

        SetHandle (new_handle);

        if (window_rect)
        {
          window->SetPosition (window_rect->left, window_rect->top);
          window->SetSize     (window_rect->right - window_rect->left, window_rect->bottom - window_rect->top);
        }

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
      if (!handle || embedded)
        return;
        
//      Notify (WindowEvent_OnClose);      

      Platform::DestroyWindow (handle);

      //reset handle, so DestroyWindow will not be called twice
      handle = 0;
    }
    
///Стиль окна
    WindowStyle Style () const { return style; }
    
    void SetStyle (WindowStyle in_style) { style = in_style; }

///Низкоуровневый дескриптор окна
    window_t Handle () const { return handle; }

    window_t CheckedHandle () const
    {
      if (!handle)
        throw xtl::message_exception<ClosedWindowException> ("syslib::Window::Impl::CheckedHandle", "Closed window exception");

      return handle;
    }
    
///Курсор окна
    WindowCursor& Cursor () { return cursor; }

///Установка низкоуровневого дескриптора окна
    void SetHandle (window_t new_handle)
    {
      if (handle == new_handle)
        return;

      handle = new_handle;      

      Notify (WindowEvent_OnChangeHandle);
    }
    
///Низкоуровневый дескриптор родительского окна
    const void* ParentHandle () const { return parent_handle; }

///Установка низкоуровневого дескриптора родительского окна
    void SetParentHandle (const void* new_parent_handle)
    {
//      bool need_window_recreate = !new_parent_handle && parent_handle || new_parent_handle && !parent_handle; //нужно пересоздавать окно
      bool need_window_recreate = false;

      if (need_window_recreate)
      {
        Rect window_rect = window->WindowRect ();

        Init (style, new_parent_handle, window->IsVisible (), &window_rect);
      }      
      else
      {
        parent_handle = new_parent_handle;

        Platform::SetParentWindowHandle (CheckedHandle (), parent_handle);
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
    
///Область вывода
    const Rect& Viewport ()
    {
      if (!need_update_viewport)
        return viewport;

      try
      {  
          //инициализация области вывода

        Rect client_rect  = window->ClientRect (),
             new_viewport (0, 0, client_rect.right - client_rect.left, client_rect.bottom - client_rect.top);
             
          //запуск обработчика
         
        try
        {
          if (viewport_handler)
            viewport_handler (*window, new_viewport);                        
        }
        catch (...)
        {
          new_viewport = Rect (0, 0, client_rect.right - client_rect.left, client_rect.bottom - client_rect.top);
        }

          //обновление кэша
        
        need_update_viewport = false;
        viewport             = new_viewport;

        return viewport;
      }
      catch (xtl::exception& e)
      {
        e.touch ("syslib::Window::Impl::Viewport");
        throw;
      }
    }
    
    void InvalidateViewport (WindowEventContext* context = 0)
    {
      need_update_viewport = true;      
      
      if (context)
      {      
        Notify (WindowEvent_OnChangeViewport, *context);
      }
      else
      {
        Notify (WindowEvent_OnChangeViewport);
      }      
    }
    
    void SetViewportHandler (const ViewportUpdateHandler& handler)
    {
      viewport_handler = handler;
      
      InvalidateViewport ();
    }
    
    const ViewportUpdateHandler& ViewportHandler () { return viewport_handler; }
    
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
      static const char* METHOD_NAME = "syslib::Window::Impl::RegisterEventHandler";
      
      if (event < 0 || event >= WindowEvent_Num)
        throw xtl::make_argument_exception (METHOD_NAME, "event", event);
        
      if (embedded)
        throw xtl::format_operation_exception (METHOD_NAME, "Can't register event handlers for embedded window");

      return signals [event].connect (handler);
    }    

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

      context.handle = Platform::GetNativeWindowHandle (handle);

      Notify (event, context);    
    }

  private:
///Обработчик событий окна
    static void MessageHandler (window_t wnd, WindowEvent event, const WindowEventContext& context, void* user_data)
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
          case WindowEvent_OnDestroy: //окно уничтожено
            impl->SetHandle (0);
            break;
          case WindowEvent_OnSize:    //изменились размеры окна
            impl->InvalidateViewport ();
            impl->Notify (event, context);
            break;          
          default:          
            impl->Notify (event, context);
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
    Window*               window;                             //указатель на владельца
    window_t              handle;                             //низкоуровневый дескриптор окна
    const void*           parent_handle;                      //низкоуровневый дескриптор родительского окна
    bool                  embedded;                           //окно является встроенным
    WindowStyle           style;                              //стиль окна
    stl::string           init_string;                        //строка инициализации окна
    WindowSignal          signals [WindowEvent_Num];          //сигналы окна
    bool                  close_cancel_flag;                  //флаг отмены закрытия окна
    char                  title [MAX_TITLE_LENGTH+1];         //заголовок окна
    wchar_t               title_unicode [MAX_TITLE_LENGTH+1]; //заголовок окна в Unicode
    WindowCursor          cursor;                             //курсор окна
    Rect                  viewport;                           //область вывода
    bool                  need_update_viewport;               //флаг необходимости обновления области вывода
    ViewportUpdateHandler viewport_handler;                   //обработчик изменения области вывода
    LogHandler            debug_log;                          //функция отладочного протоколирования
};

/*
    Конструктор и деструктор
*/

Window::Window (window_t handle)
{
  try
  {
    impl = stl::auto_ptr<Impl> (new Impl (this, handle));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Window(window_t)");
    throw;
  }
}

Window::Window (const char* init_string)
{
  try
  {
    impl = stl::auto_ptr<Impl> (new Impl (this, init_string));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Window()");
    throw;
  }
}

Window::Window (WindowStyle style, const char* init_string)
{
  try
  {
    impl = stl::auto_ptr<Impl> (new Impl (this, init_string));        
    
    Init (style);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Window(WindowStyle)");
    throw;
  }
}

Window::Window (WindowStyle style, unsigned int width, unsigned int height, const char* init_string)
{
  try
  {
    impl = stl::auto_ptr<Impl> (new Impl (this, init_string));
    
    Init (style, width, height);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Window(WindowStyle,unsigned int,unsigned int)");
    throw;
  }
}

Window::~Window ()
{
  try
  {
      //оповещение об удалении

    impl->Notify (WindowEvent_OnDestroy);

      //принудительное закрытие окна

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

void Window::Init (WindowStyle style, unsigned int width, unsigned int height)
{
  try
  {
    Init    (style);
    SetSize (width, height);    
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Init(WindowStyle,unsigned int,unsigned int)");
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
  try
  {
    return Platform::GetNativeWindowHandle (impl->Handle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Window::Handle");
    throw;
  }
}

const void* Window::DisplayHandle () const
{
  try
  {
    return Platform::GetNativeDisplayHandle (impl->Handle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Window::DisplayHandle");
    throw;
  }
}

const void* Window::InternalHandle () const
{
  try
  {
    return impl->Handle ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Window::InternalHandle");
    throw;
  }
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
    if (impl->Handle ())
    {
      if (style == impl->Style ())
        return;
        
        //попытка изменения стиля
        
      switch (style)
      {
        case WindowStyle_Overlapped:
        case WindowStyle_PopUp:
          break;
        default:
          throw xtl::make_argument_exception ("", "style", style);
      }
        
      if (Platform::ChangeWindowStyle (impl->Handle (), style))
      {
        impl->SetStyle (style);
      }
      else
      {        
          //пересоздания окна
        
        Rect window_rect = WindowRect ();

        impl->Init (style, impl->ParentHandle (), IsVisible (), &window_rect);                
      }
      
      impl->Notify (WindowEvent_OnChangeStyle);      
    }
    else
    {
      impl->Init (style, impl->ParentHandle (), false);
    }
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

unsigned int Window::Width () const
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

unsigned int Window::Height () const
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

unsigned int Window::ClientWidth () const
{
  try
  {
    Rect rect;

    Platform::GetClientRect (impl->CheckedHandle (), rect);

    return rect.right - rect.left;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::ClientWidth");
    throw;
  }
}

unsigned int Window::ClientHeight () const
{
  try
  {
    Rect rect;

    Platform::GetClientRect (impl->CheckedHandle (), rect);

    return rect.bottom - rect.top;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::ClientHeight");
    throw;
  }
}

void Window::SetWidth (unsigned int width)
{
  SetSize (width, Height ());
}

void Window::SetHeight (unsigned int height)
{
  SetSize (Width (), height);
}

void Window::SetSize (unsigned int width, unsigned int height)
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

void Window::SetClientWidth (unsigned int width)
{
  SetClientSize (width, ClientHeight ());
}

void Window::SetClientHeight (unsigned int height)
{
  SetClientSize (ClientWidth (), height);
}

void Window::SetClientSize (unsigned int width, unsigned int height)
{
  try
  {
    Rect rect;

    Platform::GetClientRect (impl->CheckedHandle (), rect);

    rect.right  = rect.left + width;
    rect.bottom = rect.top + height;

    Platform::SetClientRect (impl->CheckedHandle (), rect);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetSize");
    throw;
  }
}

/*
    Область вывода
*/

//возвращение области вывода
Rect Window::Viewport () const
{
  try
  {    
    return impl->Viewport ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Window::Viewport");
    throw;
  }
}

//принудительное обновление области вывода
void Window::InvalidateViewport ()
{
  impl->InvalidateViewport ();
}

void Window::SetViewportHandler (const ViewportUpdateHandler& handler)
{
  impl->SetViewportHandler (handler);
}

const Window::ViewportUpdateHandler& Window::ViewportHandler () const
{
  return impl->ViewportHandler ();
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
    
    unsigned int width  = rect.right - rect.left,
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

void Window::SetPosition (unsigned int x, unsigned int y)
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

void Window::SetCursorPosition (unsigned int x, unsigned int y)
{
  SetCursorPosition (Point (x, y));
}

/*
    Видимость курсора
*/

bool Window::IsCursorVisible () const
{
  try
  {
    return Platform::GetCursorVisible (impl->CheckedHandle ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::IsCursorVisible");
    throw;
  }
}

void Window::SetCursorVisible (bool state)
{
  try
  {
    Platform::SetCursorVisible (impl->CheckedHandle (), state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetCursorVisible");
    throw;
  }
}

/*
    Установка изображения курсора
*/

void Window::SetCursor (const WindowCursor& cursor)
{
  try
  {
    Platform::SetCursor (impl->CheckedHandle (), (cursor_t)cursor.Handle ());
    
    impl->Cursor () = cursor;
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Window::SetCursor");
    throw;
  }
}

WindowCursor Window::Cursor () const
{
  return impl->Cursor ();
}

/*
   Установка/получение multitouch режима для окна
*/

void Window::SetMultitouchEnabled (bool state)
{
  try
  {
    Platform::SetMultitouchEnabled (impl->CheckedHandle (), state);
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Window::SetMultitouchEnabled");
    throw;
  }
}

bool Window::IsMultitouchEnabled () const
{
  try
  {
    return Platform::IsMultitouchEnabled (impl->CheckedHandle ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("syslib::Window::SetMultitouchEnabled");
    throw;
  }
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
    Максимизация и минимизация окна
*/

void Window::Maximize ()
{
  try
  {
    Platform::SetWindowFlag (impl->CheckedHandle (), WindowFlag_Maximized, true);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Maximize");
    throw;
  }
}

void Window::Minimize ()
{
  try
  {
    Platform::SetWindowFlag (impl->CheckedHandle (), WindowFlag_Minimized, true);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::Minimize");
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
    Цвет фона
*/

void Window::SetBackgroundColor (const Color& color)
{
  try
  {
    Platform::SetBackgroundColor (impl->CheckedHandle (), color);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetBackgroundColor");
    throw;
  }
}

void Window::SetBackgroundState (bool state)
{
  try
  {
    Platform::SetBackgroundState (impl->CheckedHandle (), state);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::SetBackgroundState");
    throw;
  }
}

Color Window::BackgroundColor () const
{
  try
  {
    return Platform::GetBackgroundColor (impl->CheckedHandle ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::GetBackgroundColor");
    throw;
  }
}

bool Window::IsBackgroundEnabled () const
{
  try
  {
    return Platform::GetBackgroundState (impl->CheckedHandle ());
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::IsBackgroundEnabled");
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
    impl->SetParentHandle (handle);
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
    return Platform::GetParentWindowHandle (impl->CheckedHandle ());
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
    Поиск экрана вмещающего окно
*/

Screen Window::ContainingScreen () const
{
  try
  {
    return ContainingScreen (Platform::GetNativeWindowHandle (impl->CheckedHandle ()));
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::ContainingScreen()");
    throw;
  }
}

Screen Window::ContainingScreen (const void* native_handle)
{
  try
  {
    return Screen::ContainingScreen (native_handle);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("syslib::Window::ContainingScreen(const void*)");
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

/*
    Получение объекта оповещения об удалении окна
*/

xtl::trackable& Window::Trackable () const
{
  return *impl;
}

namespace syslib
{

xtl::trackable& get_trackable (const Window& window)
{
  return window.Trackable ();
}

}
