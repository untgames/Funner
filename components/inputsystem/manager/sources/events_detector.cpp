#include "shared.h"

using namespace stl;
using namespace common;
using namespace input;

/*
   Фильтр
*/

class EventsFilter : public EventsDetector::Filter
{
  public:
    EventsFilter (const char* in_action, const char* in_event_mask, const char* in_replacement) :
      action (in_action), event_mask (in_event_mask), replacement (in_replacement) {}

    const char* Action ()
    {
      return action.c_str ();
    }

    const char* EventMask ()
    {
      return event_mask.c_str ();
    }

    const char* Replacement ()
    {
      return replacement.c_str ();
    }

  private:
    string action;
    string event_mask;
    string replacement;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///Определитель событий
///////////////////////////////////////////////////////////////////////////////////////////////////
struct EventsDetector::Impl : public xtl::reference_counter
{
  public:
///////////////////////////////////////////////////////////////////////////////////////////////////
///Конструкторы / деструктор / присваивание
///////////////////////////////////////////////////////////////////////////////////////////////////
   Impl  () {}
   Impl  (const char* file_name) {Load (file_name);}
   ~Impl () {}

///////////////////////////////////////////////////////////////////////////////////////////////////
///Регистрация соответствий
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Add (const char* action, const char* input_event_mask, const char* replacement)
    {
      filters.insert_pair (action, EventsFilterPtr (new EventsFilter (action, input_event_mask, replacement)));
    }

    void Remove (const char* action, const char* input_event_mask)
    {
      FilterMap::iterator filter = filters.find (action);

      if (filter == filters.end ())
        return;

      for (hash_key<const char*> key (action); filter->first == key; ++filter)
      {
        if (!strcmp (input_event_mask, filter->second->EventMask ()))
          filters.erase (filter);
      }
    }

    void Remove (const Iterator& iter)
    {
      const FilterMap::iterator* erase_iterator = iter.target<FilterMap::iterator> ();

      if (!erase_iterator)
        return;

      filters.erase (*erase_iterator);
    }

    void Remove (const char* action)
    {
      filters.erase (action);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Перебор
///////////////////////////////////////////////////////////////////////////////////////////////////
    EventsDetector::Iterator CreateIterator () const
    {
      return Iterator (filters.begin (), filters.begin (), filters.end (), FilterSelector ());
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Очистка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Clear ()
    {
      filters.clear ();
    }
    
///////////////////////////////////////////////////////////////////////////////////////////////////
///Определение события для указанного тэга в таблице трансляции
///////////////////////////////////////////////////////////////////////////////////////////////////
    xtl::connection Detect (const EventsSource& source, const char* action, const EventHandler& handler)
    {
      DetectFunctor functor (this, action, handler);
      return source.RegisterHandler (functor);
    }

///////////////////////////////////////////////////////////////////////////////////////////////////
///Сохранение / загрузка
///////////////////////////////////////////////////////////////////////////////////////////////////
    void Load (const char* file_name)
    {
      RaiseNotImplemented ("input::EventsDetector::Load");
    }

    void Save (const char* file_name)
    {
      RaiseNotImplemented ("input::EventsDetector::Save");
    }

  private:
    typedef xtl::shared_ptr<EventsFilter>                         EventsFilterPtr;
    typedef hash_multimap<hash_key<const char*>, EventsFilterPtr> FilterMap;

    struct FilterSelector
    {
      Filter& operator () (const FilterMap::value_type& value) { return *value.second; }
    };

    struct DetectFunctor
    {
      DetectFunctor (EventsDetector::Impl* in_detector_impl, const char* in_action, const EventsDetector::EventHandler& in_handler) :
        detector_impl (in_detector_impl), action (in_action), handler (in_handler) {}

      void operator () (const char* event)
      {
        FilterMap::iterator filter = detector_impl->filters.find (action.c_str ());

        if (filter == detector_impl->filters.end ())
          return;

        for (hash_key<const char*> key (action.c_str ()); filter->first == key; ++filter)
        {
          vector<string> event_components = split (event, " ");
          vector<string> event_mask       = split (filter->second->EventMask (), " ");

          if (event_components.size () != event_mask.size ())
            continue;

          size_t matches = 0;

          for (; matches < event_components.size (); matches++)
            if (!wcmatch (event_components[matches].c_str (), event_mask[matches].c_str ()))
              break;

          if (matches == event_components.size ())
            handler (action.c_str (), event, filter->second->Replacement ());
        }
      }

      EventsDetector::Impl*        detector_impl;
      string                       action;
      EventsDetector::EventHandler handler;
    };

  private:
    FilterMap filters;
};


/*
   Конструкторы / деструктор / присваивание
*/

EventsDetector::EventsDetector  ()
  : impl (new Impl)
  {}

EventsDetector::EventsDetector (const char* file_name)
  : impl (new Impl)
{
  Load (file_name);
}

EventsDetector::EventsDetector (const EventsDetector& source)
  : impl (source.impl)
{
  addref (impl);
}

EventsDetector::~EventsDetector ()
{
  release (impl);
}

EventsDetector& EventsDetector::operator = (const EventsDetector& source)
{
  EventsDetector (source).Swap (*this);

  return *this;
}

/*
   Регистрация соответствий
*/

void EventsDetector::Add (const char* action, const char* input_event_mask, const char* replacement)
{
  static const char* METHOD_NAME = "input::EventsDetector::Add";

  if (!action)
    RaiseNullArgument (METHOD_NAME, "action");

  if (!input_event_mask)
    RaiseNullArgument (METHOD_NAME, "input_event_mask");

  if (!replacement)
    RaiseNullArgument (METHOD_NAME, "replacement");

  impl->Add (action, input_event_mask, replacement);
}

void EventsDetector::Remove (const char* action, const char* input_event_mask)
{
  static const char* METHOD_NAME = "input::EventsDetector::Remove(const char*,const char*)";

  if (!action)
    RaiseNullArgument (METHOD_NAME, "action");

  if (!input_event_mask)
    RaiseNullArgument (METHOD_NAME, "input_event_mask");

  impl->Remove (action, input_event_mask);
}

void EventsDetector::Remove (const Iterator& iter)
{
  impl->Remove (iter);
}

void EventsDetector::Remove (const char* action)
{
  if (!action)
    RaiseNullArgument ("input::EventsDetector::Remove(const char*)", "action");

  impl->Remove (action);
}

/*
   Перебор
*/

EventsDetector::Iterator EventsDetector::CreateIterator () const
{
  return impl->CreateIterator ();
}

/*
   Очистка
*/

void EventsDetector::Clear ()
{
  impl->Clear ();
}
    
/*
   Определение события для указанного тэга в таблице трансляции
*/

xtl::connection EventsDetector::Detect (const EventsSource& source, const char* action, const EventHandler& handler)
{
  if (!action)
    RaiseNullArgument ("input::EventsDetector::Detect", "action");

  return impl->Detect (source, action, handler);
}

/*
   Сохранение / загрузка
*/

void EventsDetector::Load (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("input::EventsDetector::Load", "file_name");

  impl->Load (file_name);
}

void EventsDetector::Save (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("input::EventsDetector::Save", "file_name");

  impl->Save (file_name);
}

/*
   Обмен
*/

void EventsDetector::Swap (EventsDetector& source)
{
  stl::swap (impl, source.impl);
}

namespace input
{

/*
   Обмен
*/
void swap (EventsDetector& source1, EventsDetector& source2)
{
  source1.Swap (source2);
}

}
