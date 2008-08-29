#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::windows;

/*
    Описание реализации контекста OpenGL
*/

typedef xtl::com_ptr<Adapter>          AdapterPtr;
typedef stl::vector<IContextListener*> ListenerArray;

struct Context::Impl: public IContextListener
{
  AdapterPtr                  adapter;               //адаптер
  HGLRC                       context;               //контекст OpenGL
  int                         pixel_format;          //формат пикселей
  ISwapChain*                 swap_chain;            //текущая цепочка обмена
  HDC                         device_context;        //контекст устройства текущей цепочки обмена
  bool                        vsync;                 //включена ли вертикальная синхронизация
  const WglExtensionEntries*  wgl_extension_entries; //таблица WGL-расширений
  xtl::trackable::slot_type   on_destroy_swap_chain; //обработчик удаления цепочки обмена
  ListenerArray               listeners;             //слушатели событий контекста

  Impl () : context (0), pixel_format (0), swap_chain (0), device_context (0), vsync (false), wgl_extension_entries (0),
    on_destroy_swap_chain (xtl::bind (&Impl::OnDestroySwapChain, this)) {}

///Установка вертикальной синхронизации
  void SetVSync ()
  {
    if (wgl_extension_entries->SwapIntervalEXT)
      wgl_extension_entries->SwapIntervalEXT (vsync);
  }

///Обработчик удаления цепочки обмена
  void OnDestroySwapChain ()
  {
    swap_chain            = 0;
    device_context        = 0;
    vsync                 = false;
    wgl_extension_entries = 0;
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

///Обработчик события потери текущего контекста
  void OnLostCurrent ()
  {
    adapter->SetContextListener (0);

    LostCurrentNotify ();
  }  
};

/*
    Конструктор / деструктор
*/

Context::Context (ISwapChain* in_swap_chain)
  : impl (new Impl)
{
  try
  {
      //проверка корректности аргументов
      
    if (!in_swap_chain)
      throw xtl::make_null_argument_exception ("", "swap_chain");      

    ISwapChainImpl* swap_chain = cast_object<ISwapChainImpl> (in_swap_chain, "", "swap_chain");
    
      //инициализация адаптера
      
    impl->adapter = swap_chain->GetAdapterImpl ();
    
      //получение контекста устройства отрисовки

    HDC dc = swap_chain->GetDC ();    

      //получение формата пикселей

    impl->pixel_format = swap_chain->GetPixelFormat ();

    if (!impl->pixel_format)
      throw xtl::format_operation_exception ("", "Null pixel format");

      //создание контекста

    impl->context = impl->adapter->CreateContext (dc);

    if (!impl->context)
      raise_error ("wglCreateContext");
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::Context::Context");
    throw;
  }
}

Context::~Context ()
{
  try
  {
      //отмена текущего контекста
    
    if (impl->adapter->GetCurrentContext () == impl->context)
      impl->adapter->MakeCurrent (0, 0);

      //удаление контекста

    impl->adapter->DeleteContext (impl->context);
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

      impl->swap_chain            = swap_chain;
      impl->device_context        = casted_swap_chain->GetDC ();
      impl->vsync                 = casted_swap_chain->HasVSync ();
      impl->wgl_extension_entries = &casted_swap_chain->GetWglExtensionEntries ();

        //подписка на событие удаления цепочки обмена

      casted_swap_chain->RegisterDestroyHandler (impl->on_destroy_swap_chain);
    }

      //установка текущего контекста

    impl->adapter->MakeCurrent (impl->device_context, impl->context);

      //установка вертикальной синхронизации

    impl->SetVSync ();

      //подписка на событие потери контекста

    impl->adapter->SetContextListener (&*impl);

      //оповещение об установке текущего контекста

    impl->SetCurrentNotify ();
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("render::low_level::opengl::windows::Context::MakeCurrent");
    throw;
  }
}

/*
   Проверка совместимости цепочки обмена с контекстом
*/

bool Context::IsCompatible (ISwapChain* in_swap_chain)
{
  if (!in_swap_chain)
    return false;

  ISwapChainImpl* swap_chain = dynamic_cast<ISwapChainImpl*> (in_swap_chain);

  if (!swap_chain)
    return false;

  return swap_chain->GetPixelFormat () == impl->pixel_format;
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
