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
///События окна рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
enum WindowEvent
{
  WindowEvent_OnUpdate, //окно требует перерисовки
  WindowEvent_OnResize, //размер окна изменился
  
  WindowEvent_Num
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Окно рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
class Window
{
  friend class Wrappers;
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор
///////////////////////////////////////////////////////////////////////////////////////////////////
    Window  (const Window&);
    ~Window ();
    
    Window& operator = (const Window&);
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Имя окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    void        SetName (const char* name);
    const char* Name    () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Цели рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    RenderTarget ColorBuffer        () const;
    RenderTarget DepthStencilBuffer () const;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры целей рендеринга
///////////////////////////////////////////////////////////////////////////////////////////////////
    unsigned int Width  () const;
    unsigned int Height () const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Принудительное обновление содержимого окна (обмен буферов)
///////////////////////////////////////////////////////////////////////////////////////////////////
    void SwapBuffers ();

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация обработчиков событий
///////////////////////////////////////////////////////////////////////////////////////////////////
    typedef xtl::function<void (Window&)> EventHandler;

    xtl::connection RegisterEventHandler (WindowEvent event, const EventHandler& handler) const;
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Swap (Window&);
    
  private:
    Window (WindowImpl*);

  private:
    WindowImpl* impl;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обмен
///////////////////////////////////////////////////////////////////////////////////////////////////
void swap (Window&, Window&);

///////////////////////////////////////////////////////////////////////////////////////////////////
///Слушатель событий нативного окна
///////////////////////////////////////////////////////////////////////////////////////////////////
class INativeWindowListener
{
  public:
    virtual void OnSizeChanged     (unsigned int width, unsigned int height) = 0;
    virtual void OnViewportChanged (const Rect&) = 0;
    virtual void OnHandleChanged   (void* handle) = 0;
    virtual void OnPaint           () = 0;

  protected:
    virtual ~INativeWindowListener () {}
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Интерфейс нативного окна
///////////////////////////////////////////////////////////////////////////////////////////////////
class INativeWindow
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Размеры окна
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual unsigned int GetWidth  () = 0;
    virtual unsigned int GetHeight () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Область вывода
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual Rect GetViewport () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Низкоуровневый дескриптор
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void* GetHandle () = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Установка слушателя событий
///////////////////////////////////////////////////////////////////////////////////////////////////
    virtual void AttachListener (INativeWindowListener*) = 0;
    virtual void DetachListener (INativeWindowListener*) = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Подсчет ссылок
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
