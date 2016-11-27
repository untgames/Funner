#include "shared.h"

using namespace scene_graph;
using namespace input;

/*
===================================================================================================
    InputEventListener::List::Iterator
===================================================================================================
*/

class InputEventListener::List::Iterator: public xtl::noncopyable
{
  public:
    ///Конструктор
    Iterator (List* in_list)
      : list (in_list)
      , item (in_list->first)
      , next_iterator (list->first_iterator)
    {
      list->first_iterator = this;
    }
    
    ///Деструктор
    ~Iterator ()
    {
      list->first_iterator = next_iterator;
    }

    ///Получение текущего элемента и перемемещение к следующему
    InputEventListener* Next ()
    {
      if (!item)
        return 0;
        
      InputEventListener* result = item;

      item = item->next;

      return result;
    }
    
    ///Следующий итератор
    Iterator* NextIterator () { return next_iterator; }
    
    ///Оповещение об удалении слушателя
    void OnRemove (InputEventListener* entry)
    {
      if (entry != item)
        return;

      Next ();
    }
    
    ///Оповещение о добавлении слушателя
    void OnAdd (InputEventListener* entry)
    {
      if (item || entry != list->last)
        return;
        
      item = entry;
    }

  private:  
    InputEventListener::List* list;
    InputEventListener*       item;
    Iterator*                 next_iterator;
};

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
  , first_iterator ()
{
}

InputEventListener::List::~List ()
{
  DetachAll ();
}

/*
    Оповещение
*/

void InputEventListener::List::BroadcastTouch (InputPort& input_port, const TouchEvent& event, const math::vec3f& touch_world_position)
{
  Iterator iter (this);

  while (InputEventListener* entry = iter.Next ())
  {
    try
    {
      entry->OnBroadcastTouch (input_port, event, touch_world_position);
    }
    catch (...)
    {
    }
  }
}

/*
    Удаление всех нажатий, связанных с указанной областью ввода
*/

void InputEventListener::List::RemoveAllTouches (InputPort& input_port)
{
  Iterator iter (this);

  while (InputEventListener* entry = iter.Next ())
  {
    try
    {
      entry->RemoveAllTouches (input_port);
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
  , prev ()
  , next ()  
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
  
  for (List::Iterator* i=list->first_iterator; i; i=i->NextIterator ())
    i->OnAdd (this);  
}

void InputEventListener::Detach ()
{
  if (!list)
    return;

  for (List::Iterator* i=list->first_iterator; i; i=i->NextIterator ())
    i->OnRemove (this);

  if (prev) prev->next  = next;
  else      list->first = next;
  
  if (next) next->prev = prev;
  else      list->last = prev;
  
  prev = next = 0;
  list = 0;
}
