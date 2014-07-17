#ifndef RENDER_MANAGER_WINDOW_HEADER
#define RENDER_MANAGER_WINDOW_HEADER

#include <xtl/functional_fwd>

#include <render/manager/bounds.h>
#include <render/manager/render_target.h>

namespace syslib
{

//forward declaration
class Window;

}

namespace render
{

namespace manager
{

//implementation forwards
class WindowImpl;
class Wrappers;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowEvent
{
  WindowEvent_OnUpdate, //���� ������� �����������
  WindowEvent_OnResize, //������ ���� ���������
  
  WindowEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
class Window
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������������ / ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    Window  (const Window&);
    ~Window ();
    
    Window& operator = (const Window&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///��� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetName (const char* name);
    const char* Name    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///���� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTarget ColorBuffer        () const;
    RenderTarget DepthStencilBuffer () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    size_t Width  () const;
    size_t Height () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ���������� ����������� ���� (����� �������)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SwapBuffers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///����������� ������������ �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Window&)> EventHandler;

    xtl::connection RegisterEventHandler (WindowEvent event, const EventHandler& handler) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Window&);
    
  private:
    Window (WindowImpl*);

  private:
    WindowImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///�����
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Window&, Window&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ������� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class INativeWindowListener
{
  public:
    virtual void OnSizeChanged     (size_t width, size_t height) = 0;
    virtual void OnViewportChanged (const Rect&) = 0;
    virtual void OnHandleChanged   (void* handle) = 0;
    virtual void OnPaint           () = 0;

  protected:
    virtual ~INativeWindowListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
class INativeWindow
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ����
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual size_t GetWidth  () = 0;
    virtual size_t GetHeight () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Rect GetViewport () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///�������������� ����������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void* GetHandle () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///��������� ��������� �������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AttachListener (INativeWindowListener*) = 0;
    virtual void DetachListener (INativeWindowListener*) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///������� ������
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AddRef  () = 0;
    virtual void Release () = 0;

  protected:
    virtual ~INativeWindow () {}
};

INativeWindow* make_native_window (syslib::Window&);

}

}

#endif