#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::macosx;

/*
    OpenGL context implementation
*/

typedef xtl::com_ptr<Library>          LibraryPtr;
typedef stl::vector<IContextListener*> ListenerArray;

struct Context::Impl
{
  Log                         log;                   //log
  NSOpenGLContext*            context;               //cocoa OpenGL context
  NSOpenGLPixelFormat*        pixel_format;          //pixel format
  ISwapChain*                 swap_chain;            //текущая цепочка обмена
  xtl::trackable::slot_type   on_destroy_swap_chain; //обработчик удаления цепочки обмена
  ListenerArray               listeners;             //слушатели событий контекста
  LibraryPtr                  opengl_library;        //библиотека фунций OpenGL
  static Impl*                current_context;       //текущий контекст

  Impl (Library* in_library)
    : context (0), pixel_format (0), swap_chain (0),
      on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this)), opengl_library (in_library)
    {}

///Обработчик удаления цепочки обмена
  void OnDestroySwapChain ()
  {
    swap_chain = 0;
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

Context::Context (ISwapChain* in_swap_chain, Library* library)
  : impl (new Impl (library))
{
  try
  {
      //проверка корректности аргументов

    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");

    if (!library)
      throw xtl::make_null_argument_exception ("", "library");

    ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");

      //получение формата пикселей

    impl->pixel_format = swap_chain->GetPixelFormat ();

    if (!impl->pixel_format)
      throw xtl::format_operation_exception ("", "Null pixel format");

      //создание контекста

    impl->context = [[NSOpenGLContext alloc] initWithFormat:impl->pixel_format shareContext:nil];

    impl->log.Printf ("Create context (id=%u)...", GetId ());

    if (!impl->context)
      throw xtl::format_operation_exception ("", "Can't create NSOpenGLContext");

    impl->log.Printf ("...context created successfull (handle=%08X)", impl->context);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::Context::Context");
    throw;
  }
}

Context::~Context ()
{
  try
  {
    impl->log.Printf ("Destroy context (id=%u)...", GetId ());

      //отмена текущего контекста

    if (Impl::current_context == impl.get ())
    {
      impl->LostCurrentNotify ();
      Impl::current_context = 0;
    }

      //удаление контекста

    impl->log.Printf ("...delete context (handle=%08X)", impl->context);

    [impl->context release];

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

      ISwapChainImpl* casted_swap_chain = cast_object<ISwapChainImpl> (swap_chain, "", "swap_chain");

      casted_swap_chain->SetContext (this);

        //подписка на событие удаления цепочки обмена

      casted_swap_chain->RegisterDestroyHandler (impl->on_destroy_swap_chain);

      impl->swap_chain = swap_chain;
    }

    if (Impl::current_context)
      Impl::current_context->LostCurrentNotify ();

    Impl::current_context = 0;

      //установка текущего контекста

    [impl->context makeCurrentContext];

    Impl::current_context = impl.get ();

      //оповещение об установке текущего контекста

    impl->SetCurrentNotify ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::Context::MakeCurrent");
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
    ISwapChainImpl* casted_swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "in_swap_chain");

    return impl->pixel_format == casted_swap_chain->GetPixelFormat ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::macosx::Context::IsCompatible");
    throw;
  }
}

/*
    Получение интерфейса библиотеки OpenGL
*/

ILibrary& Context::GetLibrary ()
{
  return *impl->opengl_library;
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

/*
   Returns NSOpenGLContext
*/

NSOpenGLContext* Context::GetNSOpenGLContext ()
{
  return impl->context;
}

