#ifndef SYSLIB_WINDOW_HEADER
#define SYSLIB_WINDOW_HEADER

#include <stddef.h>
#include <exception>
#include <syslib/keydefs.h>
#include <xtl/functional_fwd>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Стиль окна
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowStyle
{
  WindowStyle_PopUp,      //окно без рамки
  WindowStyle_Overlapped, //окно с рамкой
  
  WindowStyle_Default = WindowStyle_Overlapped
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///События окна
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowEvent
{
  WindowEvent_OnChangeHandle,            //изменён низкоуровневый дескриптор окна
  WindowEvent_OnDestroy,                 //окно уничтожено
  WindowEvent_OnClose,                   //посылается перед закрытием окна
  WindowEvent_OnActivate,                //окно стало активным
  WindowEvent_OnDeactivate,              //окно перестало быть активным
  WindowEvent_OnShow,                    //окно стало видимым
  WindowEvent_OnHide,                    //окно стало не видимым
  WindowEvent_OnSetFocus,                //окно получило фокус ввода
  WindowEvent_OnLostFocus,               //окно потеряло фокус ввода
  WindowEvent_OnPaint,                   //необходима перерисовка
  WindowEvent_OnSize,                    //изменились размеры окна
  WindowEvent_OnMove,                    //изменилось положение окна
  WindowEvent_OnMouseMove,               //курсор мыши переместился над областью окна
  WindowEvent_OnMouseVerticalWheel,      //изменилось положение вертикального колеса мыши
  WindowEvent_OnMouseHorisontalWheel,    //изменилось положение горизонтального колеса мыши
  WindowEvent_OnLeftButtonDown,          //нажата левая кнопка мыши
  WindowEvent_OnLeftButtonUp,            //отпущена левая кнопка мыши
  WindowEvent_OnLeftButtonDoubleClick,   //двойной щелчок левой клавишей мыши
  WindowEvent_OnRightButtonDown,         //нажата правая кнопка мыши
  WindowEvent_OnRightButtonUp,           //отпущена правая кнопка мыши  
  WindowEvent_OnRightButtonDoubleClick,  //двойной щелчок правой клавишей мыши
  WindowEvent_OnMiddleButtonDown,        //нажата средняя кнопка мыши  
  WindowEvent_OnMiddleButtonUp,          //отпущена средняя кнопка мыши
  WindowEvent_OnMiddleButtonDoubleClick, //двойной щелчок правой клавишей мыши  
  WindowEvent_OnKeyDown,                 //нажата клавиша клавиатуры
  WindowEvent_OnKeyUp,                   //отпущена клавиша клавиатуры
  WindowEvent_OnChar,                    //в буфере ввода окна появился символ
  
  WindowEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Точка
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Point
{
  size_t x, y;

  Point ();
  Point (size_t x, size_t y);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Прямоугольник
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Rect
{
  size_t left, top, right, bottom;
  
  Rect ();
  Rect (size_t left, size_t top, size_t right, size_t bottom);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Контекст события
///////////////////////////////////////////////////////////////////////////////////////////////////
struct WindowEventContext
{
  const void*   handle;                          //дескриптор окна
  Rect          window_rect;                     //область окна
  Point         cursor_position;                 //координаты курсора
  float         mouse_vertical_wheel_delta;      //изменение положения вертикального колеса мыши (в строках)
  float         mouse_horisontal_wheel_delta;    //изменение положения горизонтального колеса мыши (в строках)
  Key           key;                             //клавиша клавиатуры
  ScanCode      key_scan_code;                   //скэн-код клавиши клавиатуры
  wchar_t       char_code;                       //код символа
  bool          mouse_left_button_pressed : 1;   //удерживается ли левая кнопка мыши
  bool          mouse_right_button_pressed : 1;  //удерживается ли правая кнопка мыши
  bool          mouse_middle_button_pressed : 1; //удерживается ли средняя кнопка мыши
  bool          keyboard_alt_pressed : 1;        //удерживается Alt
  bool          keyboard_control_pressed : 1;    //удерживается Control
  bool          keyboard_shift_pressed : 1;      //удерживается Shift
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Исключение: попытка работы с закрытым окном
///////////////////////////////////////////////////////////////////////////////////////////////////
class ClosedWindowException: public std::exception
{
  public:
    const char* what () const throw () { return "closed window exception"; }
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Фасад для работы с окнами
///////////////////////////////////////////////////////////////////////////////////////////////////
class Window
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструктор и деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Window  (); //создаётся закрытое окно
    Window  (WindowStyle style);
    Window  (WindowStyle style, size_t width, size_t height);
    ~Window ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Создание окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Init (WindowStyle style);
    void Init (WindowStyle style, size_t width, size_t height);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Закрытие окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Close       ();
    void CancelClose (); //может быть вызван одним из обработчиков события WindowEvent_OnClose
    void ForceClose  (); //вызовы CancelClose игнорируются
    
    bool IsClosed () const; //проверка: закрыто ли окно

///////////////////////////////////////////////////////////////////////////////////////////////////
///Заголовок окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*    Title        () const;
    const wchar_t* TitleUnicode () const;
    void           SetTitle     (const char*);
    void           SetTitle     (const wchar_t*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Стиль окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    WindowStyle Style    () const;
    void        SetStyle (WindowStyle style);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Width     () const;
    size_t Height    () const;
    void   SetWidth  (size_t width);
    void   SetHeight (size_t height);
    void   SetSize   (size_t width, size_t height);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Область окна / клиентская область
///////////////////////////////////////////////////////////////////////////////////////////////////
    Rect WindowRect () const;
    Rect ClientRect () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Положение окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    Point Position    () const;
    void  SetPosition (const Point& position);
    void  SetPosition (size_t x, size_t y);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Положение курсора
///////////////////////////////////////////////////////////////////////////////////////////////////
    Point  CursorPosition    () const;    
    void   SetCursorPosition (const Point&);
    void   SetCursorPosition (size_t x, size_t y);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Видимость окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsVisible  () const;
    void SetVisible (bool state);
    void Show       () { SetVisible (true);  }
    void Hide       () { SetVisible (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Активность окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsActive   () const;
    void SetActive  (bool state);
    void Activate   () { SetActive (true); }
    void Deactivate () { SetActive (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Работа с фокусом ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasFocus   () const;
    void SetFocus   (bool state = true);
    void KillFocus  () { SetFocus (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обновление содержимого окна (посылка OnPaint)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Invalidate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Получение низкоуровневого дескриптора окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* Handle () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подписка на события окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Window&, WindowEvent, const WindowEventContext&)> EventHandler;

    xtl::connection RegisterEventHandler (WindowEvent event, const EventHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка пользовательской функции отладочного протоколирования
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char*)> LogHandler;

    void              SetDebugLog (const LogHandler&);
    const LogHandler& DebugLog    () const;

  private:
    const void* CheckedHandle  () const;
    void        SetHandle      (const void* handle);
    void        Notify         (WindowEvent, const WindowEventContext&);
    void        Notify         (WindowEvent);
    static void MessageHandler (WindowEvent, const WindowEventContext&, void*);

  private:
    Window (const Window&); //no impl
    Window& operator = (const Window&); //no impl

  private:
    struct Impl;
    Impl* impl;
};

#include <syslib/detail/window.inl>

}

#endif
