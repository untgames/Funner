#include "shared.h"

#ifdef _MSC_VER
  #pragma warning (disable : 4355) //'this' : used in base member initializer list
#endif

using namespace media::animation;

/*
    Описание реализации блендера анимаций
*/

namespace
{

///Буфер событий
class EventBuffer: public xtl::noncopyable
{
  struct Event
  {
    float time;
    char  event [1];
  };
  public:
    static const size_t RESERVED_EVENTS_COUNT = 16;
    static const size_t AVERAGE_EVENT_SIZE    = 16;
    static const size_t RESERVED_BUFFER_SIZE  = RESERVED_EVENTS_COUNT * (sizeof (Event) - sizeof (char) + AVERAGE_EVENT_SIZE);

///Конструктор
    EventBuffer ()
      : event_handler (xtl::bind (&EventBuffer::AddEvent, this, _1, _2))
    {
      buffer.reserve (RESERVED_BUFFER_SIZE);
      event_offsets.reserve (RESERVED_EVENTS_COUNT);
    }
    
///Добавление события
    void AddEvent (float time, const char* event_string)
    {
      size_t event_size   = strlen (event_string) + 1 + sizeof (Event) - sizeof (char);
      size_t event_offset = buffer.size ();

      event_offsets.reserve (event_offsets.size () + 1);
      buffer.resize (buffer.size () + event_size);
      event_offsets.push_back (event_offset);
      
      Event* event = reinterpret_cast<Event*> (buffer.data () + event_offset);
      
      event->time = time;
      
      strcpy (event->event, event_string);
    }

    void AddEvents (float previous_time, float current_time, const EventTrack& events)
    {
      events.GetEvents (previous_time, current_time, event_handler);
    }
    
///Очистка буферов
    void Clear ()
    {
      buffer.resize (0u, false);
      event_offsets.clear ();
    }
    
///Получение событий
    void InvokeEvents (const AnimationBlender::EventTrackHandler& handler)
    {
      Sort ();
      
      for (stl::vector<size_t>::iterator iter=event_offsets.begin (), end=event_offsets.end (); iter!=end; ++iter)
      {
        const Event* event = reinterpret_cast<const Event*> (buffer.data () + *iter);

        handler (event->time, event->event);
      }
    }
    
  private:
///Сортировка
    struct SortPredicate
    {
      char* base;
      
      SortPredicate (char* in_base) : base (in_base) {}
      
      bool operator () (size_t offset1, size_t offset2) const
      {
        const Event *event1 = reinterpret_cast<const Event*> (base + offset1),
                    *event2 = reinterpret_cast<const Event*> (base + offset2);

        return event1->time < event2->time;
      }
    };

    void Sort ()
    {
      stl::sort (event_offsets.begin (), event_offsets.end (), SortPredicate (buffer.data ()));
    }
    
  private:
    xtl::uninitialized_storage<char>    buffer;        //буфер событий
    stl::vector<size_t>                 event_offsets; //смещения событий в буфере
    AnimationBlender::EventTrackHandler event_handler; //обработчик добавления события
};

///Цель анимации
struct Target: public xtl::reference_counter
{
  TargetBlender        blender;                     //блендер
  stl::string          name;                        //имя цели
  xtl::auto_connection on_empty_blender_connection; //соединение с сигналом опустошения блендера цели
};

///Анимация
struct AnimationSource: public xtl::reference_counter
{
  EventTrack               events;                     //события анимации
  AnimationStateImpl*      state;                      //состояние анимации
  float                    prev_time;                  //предыдущее время анимации
  xtl::auto_connection     on_state_remove_connection; //соединение с сигналом удаления анимационного состояния
  
  AnimationSource () : state (0), prev_time (0.0f) {}    
};

typedef xtl::intrusive_ptr<Target>                           TargetPtr;
typedef stl::hash_map<stl::hash_key<const char*>, TargetPtr> TargetMap;
typedef xtl::intrusive_ptr<AnimationSource>                  AnimationSourcePtr;
typedef stl::hash_map<size_t, AnimationSourcePtr>            AnimationSourceMap;

}

struct AnimationBlender::Impl: public xtl::reference_counter
{
  typedef xtl::signal<void (AnimationBlenderEvent, const char*, TargetBlender&)> Signal;

  TargetMap          targets;                             //цели анимации
  AnimationSourceMap sources;                             //исходные анимации
  Signal             signals [AnimationBlenderEvent_Num]; //сигналы  
  EventBuffer        event_buffer;                        //буфер событий
  
///Конструктор
  Impl ()
  {
  }
  
///Оповещение о возникновении события
  void Notify (AnimationBlenderEvent event, const char* target_name, TargetBlender& blender)
  {
    if (event < 0 || event >= AnimationBlenderEvent_Num)
      return;
      
    try
    {
      signals [event](event, target_name, blender);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
  
///Добавление анимации
  AnimationState AddSource (const Animation& animation, const PropertyNameMap* name_map)
  {
      //если анимация уже находится в блендере - возвращаем объект её состояния
    
    AnimationSourceMap::iterator iter = sources.find (animation.Id ());
    
    if (iter != sources.end ())
      return AnimationState (*iter->second->state);

      //добавление аимации в блендер

    AnimationState state;

    AnimationSourcePtr source (new AnimationSource, false);

    source->events                     = animation.Events ();
    source->state                      = &state.Impl ();
    source->on_state_remove_connection = get_trackable (state).connect_tracker (xtl::bind (&Impl::RemoveAnimation, this, animation.Id ()));
    
      //сохранение анимации

    sources.insert_pair (animation.Id (), source);

      //размещение целей анимации
      
    for (size_t i=0, count=animation.TargetsCount (); i<count; i++)
    {
      const char* target_name    = animation.TargetName (i);
      size_t      channels_count = animation.ChannelsCount (i);
      
      if (!target_name)
        throw xtl::format_operation_exception ("", "Internal error: null animation target name");
        
        //поиск или создание цели
      
      TargetMap::iterator iter = targets.find (target_name);
      
      TargetPtr target;
      
      if (iter != targets.end ())
      {
        target = iter->second;
        
          //добавление каналов
          
        for (size_t j=0; j<channels_count; j++)
          if (name_map) target->blender.AddSource (state, animation.Channel (i, j), *name_map);
          else          target->blender.AddSource (state, animation.Channel (i, j));
      }
      else
      {
          //создание цели

        target = TargetPtr (new Target, false);

        target->name                        = target_name;
        target->on_empty_blender_connection = target->blender.RegisterEventHandler (TargetBlenderEvent_OnSourcesEmpty, xtl::bind (&Impl::RemoveTarget, this, stl::hash_key<const char*> (target_name)));

        targets.insert_pair (target_name, target);

        try
        {
            //добавление каналов
          
          for (size_t j=0; j<channels_count; j++)
            if (name_map) target->blender.AddSource (state, animation.Channel (i, j), *name_map);
            else          target->blender.AddSource (state, animation.Channel (i, j));
            
            //оповещение о появлении новой цели
            
          Notify (AnimationBlenderEvent_OnTargetAdded, target_name, target->blender);
        }
        catch (...)
        {
          targets.erase (target_name);
          throw;
        }
      }      
    }
      
    return state;
  }

///Удаление анимации
  void RemoveAnimation (size_t id)
  {
    sources.erase (id);
  }
  
///Удаление цели
  void RemoveTarget (const stl::hash_key<const char*>& key)
  {
    TargetMap::iterator iter = targets.find (key);
    
    if (iter == targets.end ())
      return;
      
    Notify (AnimationBlenderEvent_OnTargetRemoved, iter->second->name.c_str (), iter->second->blender);
      
    targets.erase (iter);
  }
  
///Обновление
  void Update (const EventTrackHandler* event_handler)
  {
    stl::auto_ptr<stl::string> error_string;
    size_t                     errors_count = 0;

    for (TargetMap::iterator iter=targets.begin (), end=targets.end (); iter!=end; ++iter)
    {
      try
      {        
        iter->second->blender.Update ();
      }
      catch (std::exception& e)
      {
        if (!error_string)
          error_string.reset (new stl::string);

        *error_string += common::format (" %u) exception at update target '%s': %s", ++errors_count, iter->second->name.c_str (), e.what ());
      }
      catch (...)
      {
        if (!error_string)
          error_string.reset (new stl::string);

        *error_string += common::format (" %u) unknown exception at update property '%s'\n", ++errors_count, iter->second->name.c_str ());
      }      
    }
    
    if (error_string)
      throw xtl::format_operation_exception ("", "Animation blending update exception. Errors:\n%s", error_string->c_str ());
      
    if (event_handler)
    {
      if (sources.size () == 1)
      {
        AnimationSource& source = *sources.begin ()->second;
        
        float time = get_time (*source.state);
        
        if (time < source.prev_time)
          source.prev_time = 0.0f;
        
        source.events.GetEvents (source.prev_time, time, *event_handler);
        
        source.prev_time = time;
      }
      else
      {
        try
        {
            //формирование списка событий
          
          for (AnimationSourceMap::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
          {
            AnimationSource& source = *iter->second;

            float time = get_time (*source.state);

            if (time < source.prev_time)
              source.prev_time = 0.0f;            

            event_buffer.AddEvents (source.prev_time, time, source.events);

            source.prev_time = time;
          }
          
            //сортировка событий по времени и вызов обработчика

          event_buffer.InvokeEvents (*event_handler);
          event_buffer.Clear ();
        }
        catch (...)
        {
          event_buffer.Clear ();
          throw;
        }                
      }
    }
    else
    {
      for (AnimationSourceMap::iterator iter=sources.begin (), end=sources.end (); iter!=end; ++iter)
      {
        AnimationSource& source = *iter->second;
        
        source.prev_time = get_time (*source.state);
      }            
    }
  }
};

/*
    Конструкторы / деструктор / присваивание
*/

AnimationBlender::AnimationBlender ()
  : impl (new Impl)
{
}

AnimationBlender::AnimationBlender (const AnimationBlender& blender)
  : impl (blender.impl)
{
  addref (impl);
}

AnimationBlender::~AnimationBlender ()
{
  release (impl);
}

AnimationBlender& AnimationBlender::operator = (const AnimationBlender& blender)
{
  AnimationBlender (blender).Swap (*this);
  return *this;
}

/*
    Общее количество источников
*/

size_t AnimationBlender::SourcesCount () const
{
  return impl->sources.size ();
}

size_t AnimationBlender::SourceChannelsCount () const
{
  size_t source_channels_count = 0;
    
  for (TargetMap::iterator iter=impl->targets.begin (), end=impl->targets.end (); iter!=end; ++iter)
    source_channels_count += iter->second->blender.SourcesCount ();

  return source_channels_count;
}

/*
    Добавление анимации
*/

AnimationState AnimationBlender::AddSource (const Animation& animation, const PropertyNameMap& name_map)
{
  try
  {
    return impl->AddSource (animation, &name_map);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::animation::AnimationBlender::AddSource(const Animation&, const PropertyNameMap&)");
    throw;
  }
}

AnimationState AnimationBlender::AddSource (const Animation& animation)
{
  try
  {
    return impl->AddSource (animation, 0);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::animation::AnimationBlender::AddSource(const Animation&)");
    throw;
  }
}

void AnimationBlender::RemoveTarget (const char* target_name)
{
  if (!target_name)
    return;
    
  impl->targets.erase (target_name);
}

void AnimationBlender::RemoveAllSources ()
{
  impl->targets.clear ();
  impl->sources.clear ();
}

/*
    Количество анимируемых целей
*/

size_t AnimationBlender::TargetsCount () const
{
  return impl->targets.size ();
}

/*
    Перебор целей анимации
*/

namespace
{

struct TargetSelector
{
  template <class T> TargetBlender& operator () (T& value) const { return value.second->blender; }
};

}

AnimationBlender::TargetIterator AnimationBlender::CreateTargetIterator ()
{
  return TargetIterator (impl->targets.begin (), impl->targets.begin (), impl->targets.end (), TargetSelector ());
}

AnimationBlender::ConstTargetIterator AnimationBlender::CreateTargetIterator () const
{
  return ConstTargetIterator (impl->targets.begin (), impl->targets.begin (), impl->targets.end (), TargetSelector ());
}

/*
    Получение имени цели
*/

const char* AnimationBlender::TargetId (const ConstTargetIterator& i) const
{
  const TargetMap::const_iterator* citer = i.target<TargetMap::const_iterator> ();
  
  if (citer)
    return (*citer)->second->name.c_str ();

  const TargetMap::iterator* iter = i.target<TargetMap::iterator> ();

  if (!iter)
    throw xtl::make_argument_exception ("media::anmation::AnimationBlender::TargetId", "iterator", "wrong-type");

  return (*iter)->second->name.c_str ();
}

/*
    Обновление состояния
*/

void AnimationBlender::Update ()
{
  try
  {
    impl->Update (0);
  }  
  catch (xtl::exception& e)
  {
    e.touch ("media::animation::AnimationBlender::Update()");
    throw;
  }
}

void AnimationBlender::Update (const EventTrack::EventHandler& event_handler)
{
  try
  {
    impl->Update (&event_handler);
  }  
  catch (xtl::exception& e)
  {
    e.touch ("media::animation::AnimationBlender::Update(const EventTrack::EventHandler&)");
    throw;
  }
}

/*
    Подписка на события блендера анимаций
*/

xtl::connection AnimationBlender::RegisterEventHandler (AnimationBlenderEvent event, const EventHandler& handler)
{
  if (event < 0 || event >= AnimationBlenderEvent_Num)
    throw xtl::make_argument_exception ("media::animation::AnimationBlender::RegisterEventHandler", "event", event);

  return impl->signals [event].connect (handler);
}

/*
    Обмен
*/

void AnimationBlender::Swap (AnimationBlender& blender)
{
  stl::swap (impl, blender.impl);
}

namespace media
{

namespace animation
{

void swap (AnimationBlender& blender1, AnimationBlender& blender2)
{
  blender1.Swap (blender2);
}

}

}
