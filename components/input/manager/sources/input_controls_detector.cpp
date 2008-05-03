#include "shared.h"

using namespace stl;
using namespace common;
using namespace input;

/*
   Фильтр
*/

class EventsFilter : public ControlsDetector::Filter
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
struct ControlsDetector::Impl : public xtl::reference_counter
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
    ControlsDetector::Iterator CreateIterator () const
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
      RaiseNotImplemented ("input::ControlsDetector::Load");
    }

    void Save (const char* file_name)
    {
      RaiseNotImplemented ("input::ControlsDetector::Save");
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
      DetectFunctor (ControlsDetector::Impl* in_detector_impl, const char* in_action, const ControlsDetector::EventHandler& in_handler) :
        detector_impl (in_detector_impl), action_hash (in_action), handler (in_handler) {}

      void operator () (const char* event)
      {
        pair<FilterMap::iterator,FilterMap::iterator> filters_range = detector_impl->filters.equal_range (action_hash);

        vector<string> event_components;

        split_event (event, event_components);

        for (; filters_range.first != filters_range.second; ++filters_range.first)
        {
          EventsFilter& filter = *filters_range.first->second;

          const char *event_mask  = filter.EventMask (),
                     *replacement = filter.Replacement (),
                     *action      = filter.Action ();

          vector<string> event_mask_components;

          split_event (event_mask, event_mask_components);

          if (event_components.size () != event_mask_components.size ())
            continue;

          if (event_components.empty () && event_mask_components.empty ())
          {
            handler (action, "", replacement);
            return;
          }

          size_t matches = 0;

          for (; matches < event_components.size (); matches++)
            if (!wcmatch (event_components [matches].c_str (), event_mask_components [matches].c_str ()))
              break;

          if (matches != event_components.size ())
            continue;

          string control_mask;

          control_mask.reserve (event_components [0].size () + strlen (event_mask) + 2); // 2 - кавычки вокруг имени контрола

          control_mask += '\'';
          control_mask += event_components [0];
          control_mask += '\'';

          for (size_t i = 1; i < event_mask_components.size (); i++)
          {
            control_mask += ' ';
            control_mask += event_mask_components [i];
          }

          handler (action, control_mask.c_str (), replacement);

          return;
        }
      }

      typedef xtl::intrusive_ptr<ControlsDetector::Impl> ControlsDetectorPtr;

      ControlsDetectorPtr            detector_impl;
      stl::hash_key<const char*>     action_hash;
      ControlsDetector::EventHandler handler;
    };

  private:
    FilterMap filters;
};


/*
   Конструкторы / деструктор / присваивание
*/

ControlsDetector::ControlsDetector  ()
  : impl (new Impl)
  {}

ControlsDetector::ControlsDetector (const char* file_name)
  : impl (new Impl)
{
  Load (file_name);
}

ControlsDetector::ControlsDetector (const ControlsDetector& source)
  : impl (source.impl)
{
  addref (impl);
}

ControlsDetector::~ControlsDetector ()
{
  release (impl);
}

ControlsDetector& ControlsDetector::operator = (const ControlsDetector& source)
{
  ControlsDetector (source).Swap (*this);

  return *this;
}

/*
   Регистрация соответствий
*/

void ControlsDetector::Add (const char* action, const char* input_event_mask, const char* replacement)
{
  static const char* METHOD_NAME = "input::ControlsDetector::Add";

  if (!action)
    RaiseNullArgument (METHOD_NAME, "action");

  if (!input_event_mask)
    RaiseNullArgument (METHOD_NAME, "input_event_mask");

  if (!replacement)
    RaiseNullArgument (METHOD_NAME, "replacement");

  impl->Add (action, input_event_mask, replacement);
}

void ControlsDetector::Remove (const char* action, const char* input_event_mask)
{
  static const char* METHOD_NAME = "input::ControlsDetector::Remove(const char*,const char*)";

  if (!action)
    RaiseNullArgument (METHOD_NAME, "action");

  if (!input_event_mask)
    RaiseNullArgument (METHOD_NAME, "input_event_mask");

  impl->Remove (action, input_event_mask);
}

void ControlsDetector::Remove (const Iterator& iter)
{
  impl->Remove (iter);
}

void ControlsDetector::Remove (const char* action)
{
  if (!action)
    RaiseNullArgument ("input::ControlsDetector::Remove(const char*)", "action");

  impl->Remove (action);
}

/*
   Перебор
*/

ControlsDetector::Iterator ControlsDetector::CreateIterator () const
{
  return impl->CreateIterator ();
}

/*
   Очистка
*/

void ControlsDetector::Clear ()
{
  impl->Clear ();
}
    
/*
   Определение события для указанного тэга в таблице трансляции
*/

xtl::connection ControlsDetector::Detect (const EventsSource& source, const char* action, const EventHandler& handler)
{
  if (!action)
    RaiseNullArgument ("input::ControlsDetector::Detect", "action");

  return impl->Detect (source, action, handler);
}

/*
   Сохранение / загрузка
*/

void ControlsDetector::Load (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("input::ControlsDetector::Load", "file_name");

  impl->Load (file_name);
}

void ControlsDetector::Save (const char* file_name)
{
  if (!file_name)
    RaiseNullArgument ("input::ControlsDetector::Save", "file_name");

  impl->Save (file_name);
}

/*
   Обмен
*/

void ControlsDetector::Swap (ControlsDetector& source)
{
  stl::swap (impl, source.impl);
}

namespace input
{

/*
   Обмен
*/
void swap (ControlsDetector& source1, ControlsDetector& source2)
{
  source1.Swap (source2);
}

}
