#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::egl;

/*
    Описание реализации контекста
*/

typedef stl::vector<IContextListener*> ListenerArray;
typedef xtl::com_ptr<Adapter>          AdapterPtr;

struct Context::Impl
{  
  Log                       log;                   //протокол драйвера отрисовки
  Output::Pointer           output;                //целевое устройство вывода
  AdapterPtr                adapter;               //целевой адаптер отрисовки  
  EGLDisplay                egl_display;           //целевой дисплей отрисовки
  EGLSurface                egl_surface;           //целевая поверхность отрисовки
  EGLContext                egl_context;           //целевой контекст отрисовки
  bool                      vsync;                 //включена ли вертикальная синхронизация
  ListenerArray             listeners;             //слушатели событий контекста
  xtl::trackable::slot_type on_destroy_swap_chain; //обработчик удаления цепочки обмена  
  PrimarySwapChain*         swap_chain;            //текущая цепочка обмена
  static Impl*              current_context;       //текущий контекст
  
///Конструктор
  Impl ()
    : egl_display (0)
    , egl_surface (0) 
    , egl_context (0)
    , vsync (false)
    , on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this))
    , swap_chain (0)
  {    
  }
  
///Сброс текущего контекста
  void ResetContext ()
  {
    if (current_context != this)
      return;

    LostCurrentNotify ();
      
    eglMakeCurrent (egl_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);

    current_context = 0;
  }

///Обработчик удаления цепочки обмена
  void OnDestroySwapChain ()
  {    
    if (current_context == this)
      ResetContext ();
    
    swap_chain  = 0;
    egl_display = 0;
    egl_surface = 0;    
  }

///Оповещение о потере контекста
  void LostCurrentNotify ()
  {
    try
    {
      for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
        (*iter)->OnLostCurrent ();
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }

///Оповещение об установке текущего контекста
  void SetCurrentNotify ()
  {
    try
    {
      for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
        (*iter)->OnSetCurrent ();
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
};

Context::Impl* Context::Impl::current_context = 0;

/*
    Конструктор / деструктор
*/

Context::Context (ISwapChain* in_swap_chain)
{
  try
  {
      //проверка корректности аргументов
      
    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    PrimarySwapChain* swap_chain = cast_object<PrimarySwapChain> (in_swap_chain, "", "swap_chain");    

      //создание реализации

    impl = new Impl;    

    impl->output  = cast_object<Output> (swap_chain->GetContainingOutput (), "", "output");
    impl->adapter = cast_object<Adapter> (swap_chain->GetAdapter (), "", "adapter");

      //создание контекста

    impl->log.Printf ("Create context (id=%d)...", GetId ());

    impl->egl_context = eglCreateContext (swap_chain->GetEglDisplay (), swap_chain->GetEglConfig (), 0, 0);        
    
    if (!impl->egl_context)
      raise_error ("::eglCreateContext");          
      
      //оповещение о создании контекста
    
    impl->log.Printf ("...context successfully created (handle=%08x)", impl->egl_context);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Context::Context");
    throw;
  }
}

Context::~Context ()
{
  try
  {
    impl->log.Printf ("Delete context (id=%d)...", GetId ());
    
      //отмена текущего контекста

    if (Impl::current_context == impl.get ())
      impl->ResetContext ();

      //удаление контекста

    impl->log.Printf ("...delete context (handle=%08X)", impl->egl_context);

    eglDestroyContext (impl->output->GetEglDisplay (), impl->egl_context);    

    impl->log.Printf ("...context successfully destroyed");
  }
  catch (...)
  {
    //подавление всех исключений
  }
}

/*
    Установка текущего контектса
*/

void Context::MakeCurrent (ISwapChain* swap_chain)
{
  try
  {
    if (!swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    if (swap_chain != impl->swap_chain)
    {
        //изменение текущей цепочки обмена

      PrimarySwapChain* casted_swap_chain = cast_object<PrimarySwapChain> (swap_chain, "", "swap_chain");

        //подписка на событие удаления цепочки обмена

      casted_swap_chain->RegisterDestroyHandler (impl->on_destroy_swap_chain);

      impl->swap_chain  = casted_swap_chain;
      impl->egl_display = impl->swap_chain->GetEglDisplay ();
      impl->egl_surface = impl->swap_chain->GetEglSurface ();

      SwapChainDesc desc;

      casted_swap_chain->GetDesc (desc);

      impl->vsync = desc.vsync;
    }
    
      //оповещение о потере текущего контекста
    
    if (Impl::current_context)
    {
      Impl::current_context->LostCurrentNotify ();    
      
      impl->current_context = 0;
    }

      //установка текущего контекста    
      
    if (!eglMakeCurrent (impl->egl_display, impl->egl_surface, impl->egl_surface, impl->egl_context))
      raise_error ("::eglMakeCurrent");

    if (!eglSwapInterval (impl->egl_display, impl->vsync))
      raise_error ("::eglSwapInterval");

    Impl::current_context = impl.get ();

      //оповещение об установке текущего контекста

    impl->SetCurrentNotify ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Context::MakeCurrent");
    throw;
  }
}

/*
    Проверка совместимости цепочки обмена с контекстом
*/

bool Context::IsCompatible (ISwapChain* in_swap_chain)
{
  try
  {
    PrimarySwapChain* swap_chain = cast_object<PrimarySwapChain> (in_swap_chain, "", "swap_chain");

    return swap_chain->GetEglDisplay () == impl->output->GetEglDisplay (); //скорее всего не корректно!!!
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::egl::Context::IsCompatible");
    throw;
  }
}

/*
    Получение интерфейса библиотеки OpenGL
*/

ILibrary& Context::GetLibrary ()
{
  return impl->adapter->GetLibrary ();
}

/*
    Подписка на события контекста
*/

void Context::AttachListener (IContextListener* listener)
{
  if (!listener)
    return;

  impl->listeners.push_back (listener);
}

void Context::DetachListener (IContextListener* listener)
{
  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}
