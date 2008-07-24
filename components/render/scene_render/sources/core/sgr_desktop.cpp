#include "shared.h"

using namespace render;

namespace
{

/*
    Константы
*/

const size_t VIEWPORT_ARRAY_RESERVE_SIZE = 4;   //резервируемый размер массива областей вывода
const size_t LISTENER_ARRAY_RESERVE_SIZE = 4;   //резервируемый размер массива слушателей

}

/*
    Описание реализации рабочего стола
*/

typedef stl::vector<Viewport>           ViewportArray;
typedef xtl::com_ptr<IDesktopListener>  ListenerPtr;
typedef stl::vector<ListenerPtr>        ListenerArray;

struct Desktop::Impl: public xtl::reference_counter
{
  stl::string   name;             //имя рабочего стола
  math::vec4f   background_color; //цвет фона
  bool          has_background;   //есть ли фон
  ViewportArray viewports;        //области вывода
  ListenerArray listeners;        //слушатели

  Impl () : has_background (true)
  {
    viewports.reserve (VIEWPORT_ARRAY_RESERVE_SIZE);
    listeners.reserve (LISTENER_ARRAY_RESERVE_SIZE);
  }
  
  ~Impl ()
  {
    DestroyNotify ();
  }
  
  template <class Fn>
  void Notify (Fn fn)
  {
    for (ListenerArray::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
    {
      try
      {
        fn (*iter);
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }        
  }  
  
  void ChangeNameNotify ()
  {
    Notify (xtl::bind (&IDesktopListener::OnChangeName, _1, name.c_str ()));
  }
  
  void ChangeBackgroundNotify ()
  {
    Notify (xtl::bind (&IDesktopListener::OnChangeBackground, _1, has_background, xtl::cref (background_color)));
  }
  
  void AttachViewportNotify (render::Viewport& viewport)
  {
    Notify (xtl::bind (&IDesktopListener::OnAttachViewport, _1, xtl::ref (viewport)));
  }
  
  void DetachViewportNotify (render::Viewport& viewport)
  {
    Notify (xtl::bind (&IDesktopListener::OnDetachViewport, _1, xtl::ref (viewport)));
  }  

  void DestroyNotify ()
  {
    Notify (xtl::bind (&IDesktopListener::OnDestroy, _1));    
  }  
};

/*
    Конструкторы / деструктор / присваивание
*/

Desktop::Desktop ()
  : impl (new Impl)
{
}

Desktop::Desktop (const Desktop& desktop)
  : impl (desktop.impl)
{
  addref (impl);
}

Desktop::~Desktop ()
{
  release (impl);
}

Desktop& Desktop::operator = (const Desktop& desktop)
{
  Desktop (desktop).Swap (*this);

  return *this;
}

/*
    Идентификатор рабочего стола
*/

size_t Desktop::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    Имя
*/

void Desktop::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::Desktop::SetName", "name");
    
  if (name == impl->name)
    return;
    
  impl->name = name;
  
  impl->ChangeNameNotify ();
}

const char* Desktop::Name () const
{
  return impl->name.c_str ();
}

/*
    Управление фона
*/

void Desktop::SetBackgroundColor (const math::vec4f& color)
{
  if (color == impl->background_color)
    return;

  impl->background_color = color;

  impl->ChangeBackgroundNotify ();
}

void Desktop::SetBackgroundColor (float red, float green, float blue, float alpha)
{
  SetBackgroundColor (math::vec4f (red, green, blue, alpha));
}

const math::vec4f& Desktop::BackgroundColor () const
{
  return impl->background_color;
}

void Desktop::SetBackgroundState (bool state)
{
  if (state == impl->has_background)
    return;

  impl->has_background = state;

  impl->ChangeBackgroundNotify ();
}

bool Desktop::HasBackground () const
{
  return impl->has_background;
}

/*
    Добавление / удаление областей вывода
*/

void Desktop::Attach (const render::Viewport& viewport)
{
  size_t viewport_id = viewport.Id ();

  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    if (iter->Id () == viewport_id)
      return; //область вывода уже добавлена

  impl->viewports.push_back (viewport);

  impl->AttachViewportNotify (impl->viewports.back ());
}

void Desktop::Detach (const render::Viewport& viewport)
{
  size_t viewport_id = viewport.Id ();

  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end;)
    if (iter->Id () == viewport_id)
    {
      ViewportArray::iterator next = iter;

      ++next;

      impl->DetachViewportNotify (*iter);

      impl->viewports.erase (iter);

      iter = next;
    }
    else ++iter;
}

void Desktop::DetachAllViewports ()
{
  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    impl->DetachViewportNotify (*iter);

  impl->viewports.clear ();
}

/*
    Количество областей вывода
*/

size_t Desktop::ViewportsCount () const
{
  return impl->viewports.size ();
}

/*
    Получение области вывода
*/

render::Viewport& Desktop::Viewport (size_t index)
{
  if (index >= impl->viewports.size ())
    throw xtl::make_range_exception ("render::Desktop::Viewport", "index", index, impl->viewports.size ());

  return impl->viewports [index];
}

const render::Viewport& Desktop::Viewport (size_t index) const
{
  return const_cast<Desktop&> (*this).Viewport (index);
}

/*
    Работа со слушателями
*/

void Desktop::AttachListener (IDesktopListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::Desktop::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Desktop::DetachListener (IDesktopListener* listener)
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Desktop::DetachAllListeners ()
{
  impl->listeners.clear ();
}

/*
    Обмен
*/

void Desktop::Swap (Desktop& desktop)
{
  stl::swap (impl, desktop.impl);
}

namespace render
{

void swap (Desktop& desktop1, Desktop& desktop2)
{
  desktop1.Swap (desktop2);
}

}
