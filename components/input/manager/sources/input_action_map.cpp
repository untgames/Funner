#include "shared.h"

using namespace stl;
using namespace common;
using namespace input;

///////////////////////////////////////////////////////////////////////////////////////////////////
///Реализация источника событий ввода
///////////////////////////////////////////////////////////////////////////////////////////////////
struct ActionMap::Impl : public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструксторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
    Impl  () {}
    ~Impl () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Добавление/удаление обработчиков клиентских событий
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add (const char* event_id, const EventHandler& handler)
    {
      HandlerMap::iterator iter = handlers.find (event_id);

      if (iter != handlers.end ())
        throw xtl::make_argument_exception ("input::ActionMap::Add", "event_id", event_id,
                              "Event handler for this event id has been already added");

      handlers.insert_pair (event_id, handler);
    }

    void Remove (const char* event_id)
    {
      handlers.erase (event_id);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка таблицы
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear  ()
    {
      handlers.clear ();
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Обработка события (формат события - "event_id arg1_value arg2_value ... argN_value")
///////////////////////////////////////////////////////////////////////////////////////////////////
    void ProcessEvent (const char* event) const
    {
      StringArray event_components;
      
      split_event (event, event_components);
      
      if (event_components.IsEmpty ())
        return;

      HandlerMap::const_iterator iter = handlers.find (event_components [0]);

      if (iter == handlers.end ())
        return;
      
      vector<const char*> args;

      args.reserve (event_components.Size () - 1);

      for (size_t i = 1; i < event_components.Size (); i++)
        args.push_back (event_components [i]);
      
      iter->second (event_components [0], event_components.Size () - 1, &args[0]);
    }

  private:
    typedef stl::hash_map<stl::string, EventHandler> HandlerMap;

  private:
    HandlerMap handlers;  //обработчики событий
};

/*
   Конструксторы / деструктор / присваивание
*/

ActionMap::ActionMap ()
  : impl (new Impl)
  {}

ActionMap::ActionMap (const ActionMap& source)
  : impl (source.impl)
{
  addref (impl);
}

ActionMap::~ActionMap ()
{
  release (impl);
}

ActionMap& ActionMap::operator = (const ActionMap& source)
{
  ActionMap (source).Swap (*this);

  return *this;
}

/*
   Добавление/удаление обработчиков клиентских событий
*/

void ActionMap::Add (const char* event_id, const EventHandler& handler)
{
  if (!event_id)
    throw xtl::make_null_argument_exception ("input::ActionMap::Add", "event_id");

  impl->Add (event_id, handler);
}

void ActionMap::Remove (const char* event_id)
{
  if (!event_id)
    throw xtl::make_null_argument_exception ("input::ActionMap::Remove", "event_id");

  impl->Remove (event_id);
}

/*
   Очистка таблицы
*/

void ActionMap::Clear ()
{
  impl->Clear ();
}

/*
   Обработка события
*/

void ActionMap::ProcessEvent (const char* event) const
{
  if (!event)
    throw xtl::make_null_argument_exception ("input::ActionMap::ProcessEvent", "event");

  impl->ProcessEvent (event);
}

/*
   Обмен
*/

void ActionMap::Swap (ActionMap& source)
{
  stl::swap (impl, source.impl);
}

namespace input
{

/*
   Обмен
*/

void swap (ActionMap& source1, ActionMap& source2)
{
  source1.Swap (source2);
}

}
