#include "shared.h"

using namespace render::low_level;
using namespace render::low_level::opengl;
using namespace render::low_level::opengl::glx;

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
  bool                      vsync;                 //включена ли вертикальная синхронизация
  ListenerArray             listeners;             //слушатели событий контекста
  xtl::trackable::slot_type on_destroy_swap_chain; //обработчик удаления цепочки обмена
  PrimarySwapChain*         swap_chain;            //текущая цепочка обмена
  static Impl*              current_context;       //текущий контекст
  
///Конструктор
  Impl ()
    : vsync (false)
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

    current_context = 0;
  }

///Обработчик удаления цепочки обмена
  void OnDestroySwapChain ()
  {    
    if (current_context == this)
      ResetContext ();
    
    swap_chain  = 0;
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
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Context::Context");
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
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Context::MakeCurrent");
}

/*
    Проверка совместимости цепочки обмена с контекстом
*/

bool Context::IsCompatible (ISwapChain* in_swap_chain)
{
  throw xtl::make_not_implemented_exception ("render::low_level::opengl::glx::Context::IsCompatible");
}

/*
    Получение интерфейса библиотеки OpenGL
*/

IAdapterLibrary& Context::GetLibrary ()
{
  return *impl->adapter->GetLibrary ();
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
