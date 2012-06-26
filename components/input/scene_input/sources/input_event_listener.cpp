#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
===================================================================================================
    InputEventListener::List
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

InputEventListener::List::List ()
  : first ()
  , last ()
{
}

InputEventListener::List::~List ()
{
  DetachAll ();
}

/*
    Оповещение
*/

void InputEventListener::List::BroadcastTouch (const TouchEvent& event, const math::vec3f& touch_world_position)
{
  for (InputEventListener* i=first; i; i=i->next)
  {
    try
    {
      i->OnBroadcastTouch (event, touch_world_position);
    }
    catch (...)
    {
    }
  }
}

/*
    Отсоединение всех
*/

void InputEventListener::List::DetachAll ()
{
  while (first)
    first->Detach ();
}

/*
===================================================================================================
    InputEventListener
===================================================================================================
*/

/*
    Конструктор / деструктор
*/

InputEventListener::InputEventListener ()
  : list ()
  , next ()
  , prev ()
{
}

InputEventListener::~InputEventListener ()
{
  Detach ();
}

/*
    Присоединение слушателя к списку
*/

void InputEventListener::Attach (List& new_list)
{
  if (list)
    Detach ();
    
  list       = &new_list;  
  prev       = list->last;
  next       = 0;
  list->last = this;
  
  if (prev) prev->next  = this;
  else      list->first = this;
}

void InputEventListener::Detach ()
{
  if (!list)
    return;

  if (prev) prev->next  = next;
  else      list->first = next;
  
  if (next) next->prev = prev;
  else      list->last = prev;
}
