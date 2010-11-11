#ifndef SYSLIB_WINDOW_HEADER
#define SYSLIB_WINDOW_HEADER

#include <stl/auto_ptr.h>

#include <xtl/functional_fwd>
#include <xtl/exception.h>

#include <syslib/keydefs.h>

namespace syslib
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowStyle
{
  WindowStyle_PopUp,      //���� ��� �����
  WindowStyle_Overlapped, //���� � ������
  
  WindowStyle_Default = WindowStyle_Overlapped
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowEvent
{
  WindowEvent_OnChangeHandle,            //������� �������������� ���������� ����
  WindowEvent_OnDestroy,                 //���� ����������
  WindowEvent_OnClose,                   //���������� ����� ��������� ����
  WindowEvent_OnActivate,                //���� ����� ��������
  WindowEvent_OnDeactivate,              //���� ��������� ���� ��������
  WindowEvent_OnShow,                    //���� ����� �������
  WindowEvent_OnHide,                    //���� ����� �� �������
  WindowEvent_OnSetFocus,                //���� �������� ����� �����
  WindowEvent_OnLostFocus,               //���� �������� ����� �����
  WindowEvent_OnPaint,                   //���������� �����������
  WindowEvent_OnSize,                    //���������� ������� ����
  WindowEvent_OnChangeViewport,          //���������� ������� ������ ����
  WindowEvent_OnMove,                    //���������� ��������� ����
  WindowEvent_OnMouseMove,               //������ ���� ������������ ��� �������� ����
  WindowEvent_OnMouseLeave,              //������ ���� ������� ������� ����
  WindowEvent_OnMouseVerticalWheel,      //���������� ��������� ������������� ������ ����
  WindowEvent_OnMouseHorisontalWheel,    //���������� ��������� ��������������� ������ ����
  WindowEvent_OnLeftButtonDown,          //������ ����� ������ ����
  WindowEvent_OnLeftButtonUp,            //�������� ����� ������ ����
  WindowEvent_OnLeftButtonDoubleClick,   //������� ������ ����� �������� ����
  WindowEvent_OnRightButtonDown,         //������ ������ ������ ����
  WindowEvent_OnRightButtonUp,           //�������� ������ ������ ����
  WindowEvent_OnRightButtonDoubleClick,  //������� ������ ������ �������� ����
  WindowEvent_OnMiddleButtonDown,        //������ ������� ������ ����
  WindowEvent_OnMiddleButtonUp,          //�������� ������� ������ ����
  WindowEvent_OnMiddleButtonDoubleClick, //������� ������ ������ �������� ����
  WindowEvent_OnXButton1Down,            //������ ������ � ������ ����
  WindowEvent_OnXButton1Up,              //�������� ������ � ������ ����
  WindowEvent_OnXButton1DoubleClick,     //������� ������ ������ � �������� ����
  WindowEvent_OnXButton2Down,            //������ ������ � ������ ����
  WindowEvent_OnXButton2Up,              //�������� ������ � ������ ����
  WindowEvent_OnXButton2DoubleClick,     //������� ������ ������ � �������� ����
  WindowEvent_OnKeyDown,                 //������ ������� ����������
  WindowEvent_OnKeyUp,                   //�������� ������� ����������
  WindowEvent_OnChar,                    //� ������ ����� ���� �������� ������
  
  WindowEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Point
{
  size_t x, y;

  Point ();
  Point (size_t x, size_t y);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Rect
{
  size_t left, top, right, bottom;
  
  Rect ();
  Rect (size_t left, size_t top, size_t right, size_t bottom);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Color
{
  unsigned char red, green, blue;
  
  Color ();
  Color (unsigned char red, unsigned char green, unsigned char blue);
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
struct WindowEventContext
{
  const void*   handle;                          //���������� ����
  Point         cursor_position;                 //���������� �������
  float         mouse_vertical_wheel_delta;      //��������� ��������� ������������� ������ ���� (� �������)
  float         mouse_horisontal_wheel_delta;    //��������� ��������� ��������������� ������ ���� (� �������)
  Key           key;                             //������� ����������
  ScanCode      key_scan_code;                   //����-��� ������� ����������
  wchar_t       char_code;                       //��� �������
  bool          mouse_left_button_pressed : 1;   //������������ �� ����� ������ ����
  bool          mouse_right_button_pressed : 1;  //������������ �� ������ ������ ����
  bool          mouse_middle_button_pressed : 1; //������������ �� ������� ������ ����
  bool          keyboard_alt_pressed : 1;        //������������ Alt
  bool          keyboard_control_pressed : 1;    //������������ Control
  bool          keyboard_shift_pressed : 1;      //������������ Shift
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������: ������� ������ � �������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ClosedWindowException: virtual public xtl::exception {};

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class WindowCursor
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ���������� / ������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    WindowCursor  ();
    WindowCursor  (const char* image_name, int hotspot_x = -1, int hotspot_y = -1); //-1 - default value
    WindowCursor  (const WindowCursor&);
    ~WindowCursor ();

    WindowCursor& operator = (const WindowCursor&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char* Image () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������������� ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* Handle () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (WindowCursor&);
    
  private:
    struct Impl;
    Impl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (WindowCursor&, WindowCursor&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ��� ������ � ������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Window
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� � ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Window  (const char* init_string = ""); //�������� �������� ����
    Window  (WindowStyle style, const char* init_string = "");
    Window  (WindowStyle style, size_t width, size_t height, const char* init_string = "");
    ~Window ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Init (WindowStyle style);
    void Init (WindowStyle style, size_t width, size_t height);

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Close       ();
    void CancelClose (); //����� ���� ������ ����� �� ������������ ������� WindowEvent_OnClose
    void ForceClose  (); //������ CancelClose ������������
    
    bool IsClosed () const; //��������: ������� �� ����

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    const char*    Title        () const;
    const wchar_t* TitleUnicode () const;
    void           SetTitle     (const char*);
    void           SetTitle     (const wchar_t*);

///////////////////////////////////////////////////////////////////////////////////////////////////
///����� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    WindowStyle Style    () const;
    void        SetStyle (WindowStyle style);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Width           () const;
    size_t Height          () const;
    size_t ClientWidth     () const;
    size_t ClientHeight    () const;
    void   SetWidth        (size_t width);
    void   SetHeight       (size_t height);
    void   SetSize         (size_t width, size_t height);
    void   SetClientWidth  (size_t width);
    void   SetClientHeight (size_t height);
    void   SetClientSize   (size_t width, size_t height);

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� / ���������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Rect WindowRect () const;
    Rect ClientRect () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Rect Viewport           () const; //����������� ������� ������
    void InvalidateViewport ();       //�������������� ���������� ������� ������

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ��������� ������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Window& window, Rect& viewport)> ViewportUpdateHandler;
    
    void                         SetViewportHandler (const ViewportUpdateHandler&);
    const ViewportUpdateHandler& ViewportHandler    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    Point Position    () const;
    void  SetPosition (const Point& position);
    void  SetPosition (size_t x, size_t y);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Point CursorPosition    () const;    
    void  SetCursorPosition (const Point&);
    void  SetCursorPosition (size_t x, size_t y);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsCursorVisible  () const;
    void SetCursorVisible (bool state);
    void ShowCursor       () { SetCursorVisible (true); }
    void HideCursor       () { SetCursorVisible (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool CursorCaptureState    () const;
    void SetCursorCaptureState (bool state);
    void CaptureCursor         () { SetCursorCaptureState (true); }
    void ReleaseCursor         () { SetCursorCaptureState (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    void         SetCursor (const WindowCursor&);
    WindowCursor Cursor    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsVisible  () const;
    void SetVisible (bool state);
    void Show       () { SetVisible (true);  }
    void Hide       () { SetVisible (false); }
    void Maximize   ();
    void Minimize   ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool IsActive   () const;
    void SetActive  (bool state);
    void Activate   () { SetActive (true); }
    void Deactivate () { SetActive (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///������ � ������� �����
///////////////////////////////////////////////////////////////////////////////////////////////////
    bool HasFocus   () const;
    void SetFocus   (bool state = true);
    void KillFocus  () { SetFocus (false); }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void  SetBackgroundColor  (const Color&);
    void  SetBackgroundState  (bool state);
    Color BackgroundColor     () const;
    bool  IsBackgroundEnabled () const;
    void  EnableBackground    () { SetBackgroundState (true); }
    void  DisableBackground   () { SetBackgroundState (false); }

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ����������� ���� (������� OnPaint)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Invalidate ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������������� ����������� ���� � ����������� ������� (��� X11)
///////////////////////////////////////////////////////////////////////////////////////////////////
    const void* Handle        () const;
    const void* DisplayHandle () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���������� ������������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetParentHandle (const void* handle);
    const void* ParentHandle    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������� �� ������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Window&, WindowEvent, const WindowEventContext&)> EventHandler;

    xtl::connection RegisterEventHandler (WindowEvent event, const EventHandler& handler);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ���������������� ������� ����������� ����������������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (const char*)> LogHandler;

    void              SetDebugLog (const LogHandler&);
    const LogHandler& DebugLog    () const;

  private:
    Window (const Window&); //no impl
    Window& operator = (const Window&); //no impl

  private:
    struct Impl;
    stl::auto_ptr<Impl> impl;
};

#include <syslib/detail/window.inl>

}

#endif
