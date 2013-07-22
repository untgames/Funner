#include "shared.h"

using namespace media::rms;

namespace
{

enum ResourceState
{
  ResourceState_Ignore,
  ResourceState_Unloaded,
  ResourceState_Prefetched,
  ResourceState_Loaded,
};

struct EventDesc: public xtl::reference_counter
{
  stl::string   name;
  ResourceState state;

  EventDesc (const char* in_name, ResourceState in_state) : name (in_name), state (in_state) {}
};

typedef xtl::intrusive_ptr<EventDesc>                                  EventPtr;
typedef stl::list<EventPtr>                                            EventList;
typedef stl::hash_map<stl::hash_key<const char*>, EventList::iterator> EventMap;

}

/*
    Описание реализации упаковщика событий
*/

struct EventBatcher::Impl: public xtl::reference_counter
{
  EventMap  event_map;
  EventList event_list;

/// Обновление
  void UpdateEvent (const char* name, ResourceState state)
  {
    EventMap::iterator iter = event_map.find (name);

    if (iter == event_map.end ())
    {
      EventPtr event = EventPtr (new EventDesc (name, state), false);

      EventList::iterator iter = event_list.insert (event_list.end (), event);

      event_map.insert_pair (name, iter);
    }
    else
    {
      EventPtr event = *iter->second;

      if (state == event->state)
        return;

      if (event->state == ResourceState_Ignore)
      {
        event_list.splice (event_list.end (), event_list, iter->second);

        event->state = state;

        return;
      }

      switch (state)
      {
        case ResourceState_Unloaded:
          event->state = ResourceState_Ignore;
          break;
        case ResourceState_Prefetched:
          if (event->state != ResourceState_Unloaded)
            return;

          event_list.splice (event_list.end (), event_list, iter->second);

          event->state = state;          

          break;
        case ResourceState_Loaded:
          if (event->state == ResourceState_Unloaded) event->state = ResourceState_Ignore;
          else 
          {
            event_list.splice (event_list.end (), event_list, iter->second);

            event->state = state;
          }

          break;
        default:
          break;
      }
    }    
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

EventBatcher::EventBatcher ()
  : impl (new Impl)
{
}

EventBatcher::EventBatcher (const EventBatcher& batcher)
  : impl (batcher.impl)
{
}

EventBatcher::~EventBatcher ()
{
  release (impl);
}

EventBatcher& EventBatcher::operator = (const EventBatcher& batcher)
{
  EventBatcher tmp = batcher;

  stl::swap (impl, tmp.impl);

  return *this;
}

/*
    Операции над ресурсом
*/

void EventBatcher::PrefetchResource (const char* resource_name)
{
  if (!resource_name)
    throw xtl::make_null_argument_exception ("media::rms::EventBatcher::PrefetchResource", "resource_name");

  impl->UpdateEvent (resource_name, ResourceState_Prefetched);  
}

void EventBatcher::LoadResource (const char* resource_name)
{
  if (!resource_name)
    throw xtl::make_null_argument_exception ("media::rms::EventBatcher::LoadResource", "resource_name");

  impl->UpdateEvent (resource_name, ResourceState_Loaded);
}

void EventBatcher::UnloadResource (const char* resource_name)
{
  if (!resource_name)
    throw xtl::make_null_argument_exception ("media::rms::EventBatcher::UnloadResource", "resource_name");

  impl->UpdateEvent (resource_name, ResourceState_Unloaded);
}

/*
    Опустошение
*/

void EventBatcher::Flush (ICustomServer& server)
{
  while (!impl->event_list.empty ())
  {
    EventPtr event = impl->event_list.front ();

    impl->event_list.pop_front ();

    try
    {
      switch (event->state)
      {
        case ResourceState_Unloaded:
          server.UnloadResource (event->name.c_str ());
          break;
        case ResourceState_Prefetched:
          server.PrefetchResource (event->name.c_str ());
          break;
        case ResourceState_Loaded:
          server.LoadResource (event->name.c_str ());
          break;
        default:
          break;
      }
    }
    catch (...)
    {
    }
  }

  impl->event_map.clear ();
}
