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
typedef xtl::com_ptr<IScreenListener>  ListenerPtr;
typedef stl::vector<ListenerPtr>        ListenerArray;

struct Screen::Impl: public xtl::reference_counter
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
    Notify (xtl::bind (&IScreenListener::OnChangeName, _1, name.c_str ()));
  }
  
  void ChangeBackgroundNotify ()
  {
    Notify (xtl::bind (&IScreenListener::OnChangeBackground, _1, has_background, xtl::cref (background_color)));
  }
  
  void AttachViewportNotify (render::Viewport& viewport)
  {
    Notify (xtl::bind (&IScreenListener::OnAttachViewport, _1, xtl::ref (viewport)));
  }
  
  void DetachViewportNotify (render::Viewport& viewport)
  {
    Notify (xtl::bind (&IScreenListener::OnDetachViewport, _1, xtl::ref (viewport)));
  }  

  void DestroyNotify ()
  {
    Notify (xtl::bind (&IScreenListener::OnDestroy, _1));    
  }  
};

/*
    Конструкторы / деструктор / присваивание
*/

Screen::Screen ()
  : impl (new Impl)
{
}

Screen::Screen (const Screen& Screen)
  : impl (Screen.impl)
{
  addref (impl);
}

Screen::~Screen ()
{
  release (impl);
}

Screen& Screen::operator = (const Screen& screen)
{
  Screen (screen).Swap (*this);

  return *this;
}

/*
    Идентификатор рабочего стола
*/

size_t Screen::Id () const
{
  return reinterpret_cast<size_t> (get_pointer (impl));
}

/*
    Имя
*/

void Screen::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("render::Screen::SetName", "name");
    
  if (name == impl->name)
    return;
    
  impl->name = name;
  
  impl->ChangeNameNotify ();
}

const char* Screen::Name () const
{
  return impl->name.c_str ();
}

/*
    Управление фона
*/

void Screen::SetBackgroundColor (const math::vec4f& color)
{
  if (color == impl->background_color)
    return;

  impl->background_color = color;

  impl->ChangeBackgroundNotify ();
}

void Screen::SetBackgroundColor (float red, float green, float blue, float alpha)
{
  SetBackgroundColor (math::vec4f (red, green, blue, alpha));
}

const math::vec4f& Screen::BackgroundColor () const
{
  return impl->background_color;
}

void Screen::SetBackgroundState (bool state)
{
  if (state == impl->has_background)
    return;

  impl->has_background = state;

  impl->ChangeBackgroundNotify ();
}

bool Screen::BackgroundState () const
{
  return impl->has_background;
}

/*
    Добавление / удаление областей вывода
*/

void Screen::Attach (const render::Viewport& viewport)
{
  size_t viewport_id = viewport.Id ();

  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    if (iter->Id () == viewport_id)
      return; //область вывода уже добавлена

  impl->viewports.push_back (viewport);

  impl->AttachViewportNotify (impl->viewports.back ());
}

void Screen::Detach (const render::Viewport& viewport)
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

void Screen::DetachAllViewports ()
{
  for (ViewportArray::iterator iter=impl->viewports.begin (), end=impl->viewports.end (); iter!=end; ++iter)
    impl->DetachViewportNotify (*iter);

  impl->viewports.clear ();
}

/*
    Количество областей вывода
*/

size_t Screen::ViewportsCount () const
{
  return impl->viewports.size ();
}

/*
    Получение области вывода
*/

render::Viewport& Screen::Viewport (size_t index)
{
  if (index >= impl->viewports.size ())
    throw xtl::make_range_exception ("render::Screen::Viewport", "index", index, impl->viewports.size ());

  return impl->viewports [index];
}

const render::Viewport& Screen::Viewport (size_t index) const
{
  return const_cast<Screen&> (*this).Viewport (index);
}

/*
    Работа со слушателями
*/

void Screen::AttachListener (IScreenListener* listener)
{
  if (!listener)
    throw xtl::make_null_argument_exception ("render::Screen::AttachListener", "listener");

  impl->listeners.push_back (listener);
}

void Screen::DetachListener (IScreenListener* listener)
{
  if (!listener)
    return;

  impl->listeners.erase (stl::remove (impl->listeners.begin (), impl->listeners.end (), listener), impl->listeners.end ());
}

void Screen::DetachAllListeners ()
{
  impl->listeners.clear ();
}

/*
    Обмен
*/

void Screen::Swap (Screen& screen)
{
  stl::swap (impl, screen.impl);
}

namespace render
{

void swap (Screen& screen1, Screen& screen2)
{
  screen1.Swap (screen2);
}

}
