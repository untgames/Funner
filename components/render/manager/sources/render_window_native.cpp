#include "shared.h"

using namespace render;

/*
    Описание реализации нативного окна
*/

namespace
{

Rect convert_rect (const syslib::Rect& rect)
{
  return Rect ((int)rect.left, (int)rect.top, rect.right - rect.left, rect.bottom - rect.top);
}

}

typedef stl::vector<INativeWindowListener*> ListenerArray;

struct NativeWindow::Impl: public xtl::trackable
{
  syslib::Window* window;
  ListenerArray   listeners;

/// Конструктор
  Impl (syslib::Window& in_window) : window (&in_window) {}

/// Получение окна
  syslib::Window& Window ()
  {
    if (!window)
      throw xtl::format_operation_exception ("render::NativeWindow::Impl::Window", "Window has been already destroyed");

    return *window;
  }

/// События
  void OnDestroy ()
  {
    window = 0;
  }

  void OnResize ()
  {
    size_t width = Window ().ClientWidth (), height = Window ().ClientHeight ();

    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
      (*iter)->OnSizeChanged (width, height);
  }

  void OnPaint ()
  {
    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
      (*iter)->OnPaint ();
  }

  void OnChangeHandle ()
  {
    void* handle = (void*)Window ().Handle ();

    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
      (*iter)->OnHandleChanged (handle);
  }

  void OnChangeViewport ()
  {
    Rect rect = convert_rect (Window ().Viewport ());

    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
      (*iter)->OnViewportChanged (rect);
  }
};

/*
    Конструктор / деструктор
*/

NativeWindow::NativeWindow (syslib::Window& window)
  : impl (new Impl (window))
{
  try
  {
    window.Trackable ().connect_tracker (xtl::bind (&Impl::OnDestroy, &*impl), *impl);
    impl->connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnSize, xtl::bind (&Impl::OnResize, &*impl)));
    impl->connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnChangeHandle, xtl::bind (&Impl::OnChangeHandle, &*impl)));
    impl->connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnPaint, xtl::bind (&Impl::OnPaint, &*impl)));
    impl->connect_tracker (window.RegisterEventHandler (syslib::WindowEvent_OnChangeViewport, xtl::bind (&Impl::OnChangeViewport, &*impl)));
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::NativeWindow::NativeWindow");
    throw;
  }
}

NativeWindow::~NativeWindow ()
{
}

/*
    Подсчет ссылок
*/

void NativeWindow::AddRef ()
{
  addref (this);
}

void NativeWindow::Release ()
{
  release (this);
}

/*
    Размеры окна
*/

size_t NativeWindow::GetWidth ()
{
  try
  {
    return impl->Window ().ClientWidth ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::NativeWindow::GetWidth");
    throw;
  }
}

size_t NativeWindow::GetHeight ()
{
  try
  {
    return impl->Window ().ClientHeight ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::NativeWindow::GetHeight");
    throw;
  }
}

/*
    Область вывода
*/

Rect NativeWindow::GetViewport ()
{
  try
  {   
    return convert_rect (impl->Window ().Viewport ());
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::NativeWindow::GetViewport");
    throw;
  }
}

/*
    Низкоуровневый дескриптор
*/

void* NativeWindow::GetHandle ()
{
  try
  {
    return (void*)impl->Window ().Handle ();
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::NativeWindow::GetHandle");
    throw;
  }
}

/*
    Установка слушателя событий
*/

void NativeWindow::AttachListener (INativeWindowListener* listener)
{
  try
  {
    impl->listeners.push_back (listener);    
  }
  catch (xtl::exception& e)
  {
    e.touch ("render::NativeWindow::AttachListener");
    throw;
  }
}

void NativeWindow::DetachListener (INativeWindowListener* listener)
{
  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

/*
    Создание обертки
*/

namespace render
{

INativeWindow* make_native_window (syslib::Window& window)
{
  return new NativeWindow (window);
}

}
