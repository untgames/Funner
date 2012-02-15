#include "shared.h"

using namespace media::animation;

namespace
{

/*
    Константы
*/

const size_t RESERVED_CHANNELS_COUNT = 4; //резервируемое число каналов

}

/*
    Описание реализации блендер каналов
*/

struct ChannelBlenderBase::Impl: public xtl::reference_counter
{
  typedef stl::vector<Source>                        SourceArray;
  typedef xtl::intrusive_ptr<detail::IEvaluatorBase> EvaluatorPtr;
  
  struct SourceImpl: public xtl::reference_counter
  {
    EvaluatorPtr         evaluator;
    xtl::auto_connection state_tracker;
  };
  
  typedef xtl::intrusive_ptr<SourceImpl>          SourceImplPtr;
  typedef stl::vector<SourceImplPtr>              SourceImplArray;  
  typedef xtl::signal<void (ChannelBlenderEvent)> Signal;

  const std::type_info* value_type;                        //тип значений
  SourceArray           sources;                           //массив дескрипторов каналов
  SourceImplArray       source_impls;                      //каналы
  Signal                signals [ChannelBlenderEvent_Num]; //сигналы
  
///Конструктор
  Impl (const std::type_info& in_value_type)
    : value_type (&in_value_type)
  {
    sources.reserve (RESERVED_CHANNELS_COUNT);
    source_impls.reserve (RESERVED_CHANNELS_COUNT);
  }
  
///Оповещение о возникновении события
  void Notify (ChannelBlenderEvent event)
  {
    if (event < 0 || event >= ChannelBlenderEvent_Num)
      return;
      
    try
    {
      signals [event](event);
    }
    catch (...)
    {
      //подавление всех исключений
    }
  }
  
///Удаление каналов
  void RemoveSourcesByState (AnimationStateImpl* state)
  {
    bool removed = false;
    
    for (SourceArray::iterator iter=sources.begin (); iter!=sources.end ();)
      if (iter->state == state)
      {
        sources.erase (iter);
        source_impls.erase (source_impls.begin () + (iter - sources.begin ()));                        
        
        removed = true;
      }
      else ++iter;
      
    if (removed)
      Notify (ChannelBlenderEvent_OnSourcesRemoved);
  }
  
  void RemoveSourcesByChannel (const Channel& channel)
  {
    detail::IEvaluatorBase* evaluator = channel.TrackCore ();
    
    if (!evaluator)
      return;
      
    bool removed = false;
    
    for (SourceArray::iterator iter=sources.begin (); iter!=sources.end ();)
      if (iter->evaluator == evaluator)
      {
        sources.erase (iter);
        source_impls.erase (source_impls.begin () + (iter - sources.begin ()));

        removed = true;
      }
      else ++iter;
      
    if (removed)
      Notify (ChannelBlenderEvent_OnSourcesRemoved);
  }  
};

/*
    Конструкторы / деструктор / присваивание
*/

ChannelBlenderBase::ChannelBlenderBase (const std::type_info& value_type)
  : impl (new Impl (value_type))
{
}

ChannelBlenderBase::ChannelBlenderBase (const ChannelBlenderBase& blender)
  : impl (blender.impl)
{
  addref (impl);
}

ChannelBlenderBase::~ChannelBlenderBase ()
{
  release (impl);
}

ChannelBlenderBase& ChannelBlenderBase::operator = (const ChannelBlenderBase& blender)
{
  ChannelBlenderBase (blender).Swap (*this);
  
  return *this;
}

/*
    Тип значений
*/

const std::type_info& ChannelBlenderBase::ValueType () const
{
  return *impl->value_type;
}

/*
    Количество каналов
*/

size_t ChannelBlenderBase::SourcesCount () const
{
  return impl->sources.size ();
}

/*
    Получение каналов и анимационных состояний
*/

const ChannelBlenderBase::Source* ChannelBlenderBase::Sources () const
{
  if (impl->sources.empty ())
    return 0;

  return &impl->sources [0];
}

/*
    Добавление и удаление каналов
*/

void ChannelBlenderBase::AddSource (const media::animation::AnimationState& state, const media::animation::Channel& channel)
{
  static const char* METHOD_NAME = "media::animation::ChannelBlenderBase::AddSource";

    //проверка корректности аргументов

  Source desc;
  
  desc.state     = &state.Impl ();
  desc.evaluator = channel.TrackCore ();
  
  if (!desc.evaluator)
    throw xtl::format_operation_exception (METHOD_NAME, "Can't add empty animation channel to blender");
  
  if (&desc.evaluator->ValueType () != impl->value_type)
    throw xtl::format_operation_exception (METHOD_NAME, "Attempt to add animation channel with value type '%s' to blender with value type '%s'", desc.evaluator->ValueType ().name (),
      impl->value_type->name ());
      
    //добавление в карту
    
  Impl::SourceImplPtr channel_impl (new Impl::SourceImpl, false);
  
  channel_impl->evaluator     = desc.evaluator;
  channel_impl->state_tracker = state.GetTrackable ().connect_tracker (xtl::bind (&Impl::RemoveSourcesByState, impl, &state.Impl ()));
    
  impl->sources.reserve (impl->sources.size () + 1);
  impl->source_impls.reserve (impl->source_impls.size () + 1);
  
  impl->sources.push_back (desc);
  impl->source_impls.push_back (channel_impl);

    //оповещени о возникновении события

  impl->Notify (ChannelBlenderEvent_OnSourcesAdded);
}

void ChannelBlenderBase::RemoveSources (const media::animation::AnimationState& state)
{
  impl->RemoveSourcesByState (&state.Impl ());
}

void ChannelBlenderBase::RemoveSources (const media::animation::Channel& channel)
{
  impl->RemoveSourcesByChannel (channel);
}

void ChannelBlenderBase::RemoveAllSources ()
{
  impl->sources.clear ();
  impl->source_impls.clear ();
  
  impl->Notify (ChannelBlenderEvent_OnSourcesRemoved);
}

/*
    Подписка на события блендера каналов
*/

xtl::connection ChannelBlenderBase::RegisterEventHandler (ChannelBlenderEvent event, const EventHandler& handler) const
{
  if (event < 0 || event >= ChannelBlenderEvent_Num)
    throw xtl::make_argument_exception ("media::animation::ChannelBlenderBase::RegisterEventHandler", "event", event);
    
  return impl->signals [event].connect (handler);
}

/*
    Обмен
*/

void ChannelBlenderBase::Swap (ChannelBlenderBase& blender)
{
  stl::swap (impl, blender.impl);
}

namespace media
{

namespace animation
{

void swap (ChannelBlenderBase& b1, ChannelBlenderBase& b2)
{
  b1.Swap (b2);
}

}

}
