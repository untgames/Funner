#include "shared.h"

using namespace media::animation;

/*
    ќписание реализации блендера анимаций
*/

namespace
{

///÷ель анимации
struct Target: public xtl::reference_counter
{
  TargetBlender        blender;                     //блендер
  stl::string          name;                        //им€ цели
  xtl::auto_connection on_empty_blender_connection; //соединение с сигналом опустошени€ блендера цели
};

///јнимаци€
struct AnimationSource: public xtl::reference_counter
{
  EventTrack           events;                     //событи€ анимации
  AnimationStateImpl*  state;                      //состо€ние анимации
  xtl::auto_connection on_state_remove_connection; //соединение с сигналом удалени€ анимационного состо€ни€
  
  AnimationSource () : state (0) {}
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
  size_t             source_channels_count;               //количество каналов-источников
  bool               need_update_source_channels_count;   //необходимо обновить количество каналов-источников
  
/// онструктор
  Impl ()
    : source_channels_count (0)
    , need_update_source_channels_count (false)
  {
  }
  
///ќповещение о возникновении событи€
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
  
///ќбновление количества источников
  void UpdateSourceChannelsCount ()
  {
    if (!need_update_source_channels_count)
      return;
      
    source_channels_count = 0;
      
    for (TargetMap::iterator iter=targets.begin (), end=targets.end (); iter!=end; ++iter)
      source_channels_count += iter->second->blender.SourcesCount ();

    need_update_source_channels_count = false;
  }
  
///ƒобавление анимации
  AnimationState AddSource (const Animation& animation, const PropertyNameMap* name_map)
  {
      //если анимаци€ уже находитс€ в блендере - возвращаем объект еЄ состо€ни€
    
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
            
            //оповещение о по€влении новой цели
            
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

///”даление анимации
  void RemoveAnimation (size_t id)
  {
    sources.erase (id);
  }
  
///”даление цели
  void RemoveTarget (const stl::hash_key<const char*>& key)
  {
    TargetMap::iterator iter = targets.find (key);
    
    if (iter == targets.end ())
      return;
      
    Notify (AnimationBlenderEvent_OnTargetRemoved, iter->second->name.c_str (), iter->second->blender);
      
    targets.erase (iter);
  }
};

/*
     онструкторы / деструктор / присваивание
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
    ќбщее количество источников
*/

size_t AnimationBlender::SourcesCount () const
{
  return impl->sources.size ();
}

size_t AnimationBlender::SourceChannelsCount () const
{
  impl->UpdateSourceChannelsCount ();

  return impl->source_channels_count;
}

/*
    ƒобавление анимации
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
  
  impl->need_update_source_channels_count = true;  
}

void AnimationBlender::RemoveAllSources ()
{
  impl->targets.clear ();
  impl->sources.clear ();
  
  impl->need_update_source_channels_count = true;
}

/*
     оличество анимируемых целей
*/

size_t AnimationBlender::TargetsCount () const
{
  return impl->targets.size ();
}

/*
    ѕеребор целей анимации
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
    ѕолучение имени цели
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
    ѕеречисление событий, прошедших в указанном промежутке времени
*/

void AnimationBlender::GetEvents (float previous_time, float current_time, const EventTrack::EventHandler& handler) const
{  
  try
  {
    for (AnimationSourceMap::iterator iter=impl->sources.begin (), end=impl->sources.end (); iter!=end; ++iter)
      iter->second->events.GetEvents (previous_time, current_time, handler);
  }
  catch (xtl::exception& e)
  {
    e.touch ("media::anmation::AnimationBlender::GetEvents");
    throw;
  }
}

/*
    ќбновление состо€ни€
*/

void AnimationBlender::Update ()
{
  try
  {
    stl::auto_ptr<stl::string> error_string;
    size_t                     errors_count = 0;

    for (TargetMap::iterator iter=impl->targets.begin (), end=impl->targets.end (); iter!=end; ++iter)
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
  }  
  catch (xtl::exception& e)
  {
    e.touch ("media::animation::AnimationBlender::Update");
    throw;
  }
}

/*
    ѕодписка на событи€ блендера анимаций
*/

xtl::connection AnimationBlender::RegisterEventHandler (AnimationBlenderEvent event, const EventHandler& handler)
{
  if (event < 0 || event >= AnimationBlenderEvent_Num)
    throw xtl::make_argument_exception ("media::animation::AnimationBlender::RegisterEventHandler", "event", event);

  return impl->signals [event].connect (handler);
}

/*
    ќбмен
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
