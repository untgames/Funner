#include "shared.h"

using namespace input;

/*
    Описание реализации курсора
*/

typedef stl::list<ICursorListener*> ListenerList;

struct Cursor::Impl: public xtl::reference_counter
{
  math::vec2f  position;   //положение курсора
  ListenerList listeners;  //слушатели событий курсора
  bool         visible;    //видим ли курсор
  
  Impl () : visible (true) {}
  
  ~Impl ()
  {
    for (ListenerList::iterator iter=listeners.begin (), end=listeners.end (); iter!=end; ++iter)
    {
      try
      {
        (*iter)->OnDestroy ();
      }
      catch (...)
      {
        //подавление всех исключений
      }
    }
  }  
};

/*
    Конструкторы / деструктор / присваивание
*/

Cursor::Cursor ()
  : impl (new Impl)
{
}

Cursor::Cursor (const Cursor& cursor)
  : impl (cursor.impl)
{
  addref (impl);
}

Cursor::~Cursor ()
{
  release (impl);
}

Cursor& Cursor::operator = (const Cursor& cursor)
{
  Cursor (cursor).Swap (*this);
  
  return *this;
}

/*
    Установка положения курсора
*/

void Cursor::SetPosition (float x, float y)
{
  SetPosition (math::vec2f (x, y));
}

void Cursor::SetPosition (const math::vec2f& position)
{
  if (position == impl->position)
    return;

  impl->position = position;
  
  for (ListenerList::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try
    {
      (*iter)->OnChangePosition (position);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
}

math::vec2f& Cursor::Position () const
{
  return impl->position;
}

float Cursor::GetX () const
{
  return impl->position.x;
}

float Cursor::GetY () const
{
  return impl->position.y;
}

/*
    Управление видимостью курсора
*/

bool Cursor::IsVisible () const
{
  return impl->visible;
}

void Cursor::SetVisible (bool state)
{
  if (state == impl->visible)
    return;

  impl->visible = state;
  
  for (ListenerList::iterator iter=impl->listeners.begin (), end=impl->listeners.end (); iter!=end; ++iter)
  {
    try    
    {
      (*iter)->OnChangeVisible (state);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }  
}

/*
    Добавление слушателей событий курсора
*/

void Cursor::Attach (ICursorListener* listener)
{
  impl->listeners.push_back (listener);
}

void Cursor::Detach (ICursorListener* listener)
{
  impl->listeners.remove (listener);
}

/*
    Обмен
*/

void Cursor::Swap (Cursor& cursor)
{
  stl::swap (impl, cursor.impl);
}

namespace input
{

void swap (Cursor& cursor1, Cursor& cursor2)
{
  cursor1.Swap (cursor2);
}

}
