#include "shared.h"

using namespace media;
using namespace media::animation;

/*
   Реализация анимации
*/

namespace
{

typedef stl::vector<Channel> ChannelsArray;

struct ChannelGroup : public xtl::reference_counter
{
  stl::string   target_name; //имя анимируемого объекта
  ChannelsArray channels;    //каналы

  ChannelGroup (const char* in_target_name)
    : target_name (in_target_name)
    {}
};

typedef xtl::intrusive_ptr<ChannelGroup> ChannelGroupPtr;
typedef stl::vector<ChannelGroupPtr>     ChannelGroupsArray;

}

struct Animation::Impl : public xtl::reference_counter
{
  stl::string        name;            //имя анимации
  EventTrack         event_track;     //трек событий
  ChannelGroupsArray channel_groups;  //каналы анимации

  Impl () {}

  Impl (const Impl& source)
    : name (source.name), event_track (source.event_track.Clone ())
  {
    channel_groups.reserve (source.channel_groups.size ());

    for (ChannelGroupsArray::const_iterator group_iter = source.channel_groups.begin (), group_end = source.channel_groups.end (); group_iter != group_end; ++group_iter)
    {
      ChannelGroupPtr new_group (new ChannelGroup ((*group_iter)->target_name.c_str ()), false);

      for (ChannelsArray::const_iterator iter = (*group_iter)->channels.begin (), end = (*group_iter)->channels.end (); iter != end; ++iter)
        new_group->channels.push_back (iter->Clone ());

      channel_groups.push_back (new_group);
    }
  }
  
  void GetTimeLimits (float& min_time, float& max_time, float& min_unwrapped_time, float& max_unwrapped_time)
  {
    bool has_result = false;

    min_time           = FLT_MAX;
    max_time           = FLT_MIN;
    min_unwrapped_time = FLT_MAX;
    max_unwrapped_time = FLT_MIN;
    
    for (ChannelGroupsArray::const_iterator group_iter = channel_groups.begin (), group_end = channel_groups.end (); group_iter != group_end; ++group_iter)
    {
      const ChannelGroup& group = **group_iter;

      for (ChannelsArray::const_iterator iter = group.channels.begin (), end = group.channels.end (); iter != end; ++iter)
      {
        const animation::Channel& channel = *iter;
        
        if (!channel.HasTrack ())
          continue;
          
        if (min_time > channel.MinTime ())                    min_time           = channel.MinTime ();
        if (min_unwrapped_time > channel.MinUnwrappedTime ()) min_unwrapped_time = channel.MinUnwrappedTime ();          
        if (max_time < channel.MaxTime ())                    max_time           = channel.MaxTime ();                  
        if (max_unwrapped_time < channel.MaxUnwrappedTime ()) max_unwrapped_time = channel.MaxUnwrappedTime ();        
          
        has_result = true;
      }
    }
    
    if (event_track.Size ())
    {
      if (min_time > event_track.MinTime ())                    min_time           = event_track.MinTime ();
      if (min_unwrapped_time > event_track.MinUnwrappedTime ()) min_unwrapped_time = event_track.MinUnwrappedTime ();
      if (max_time < event_track.MaxTime ())                    max_time           = event_track.MaxTime ();
      if (max_unwrapped_time < event_track.MaxUnwrappedTime ()) max_unwrapped_time = event_track.MaxUnwrappedTime ();

      has_result = true;
    }

    if (!has_result)
    {
      min_time           = 0.0f;
      max_time           = 0.0f;
      min_unwrapped_time = 0.0f;
      max_unwrapped_time = 0.0f;
    }
  }
};

/*
   Конструкторы / деструктор / присваивание
*/

Animation::Animation ()
  : impl (new Impl)
{
}

Animation::Animation (const Animation& source)
  : impl (source.impl)
{
  addref (impl);
}

Animation::Animation (Impl* in_impl)
  : impl (in_impl)
{
}

Animation::~Animation ()
{
  release (impl);
}

Animation& Animation::operator = (const Animation& source)
{
  Animation (source).Swap (*this);

  return *this;
}
    
/*
   Создание копии
*/

Animation Animation::Clone () const
{
  return Animation (new Impl (*impl));
}

/*
   Идентификатор
*/

size_t Animation::Id () const
{
  return (size_t)impl;
}

/*
   Имя анимации
*/

const char* Animation::Name () const
{
  return impl->name.c_str ();
}

void Animation::Rename (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::animation::Animation::Rename", "name");

  impl->name = name;
}
    
/*
   Перебор анимируемых объектов
*/

unsigned int Animation::TargetsCount () const
{
  return (unsigned int)impl->channel_groups.size ();
}

const char* Animation::TargetName (unsigned int target_index) const
{
  if (target_index >= impl->channel_groups.size ())
    throw xtl::make_range_exception ("media::animation::Animation::TargetName", "target_index", target_index, 0u, impl->channel_groups.size ());

  return impl->channel_groups [target_index]->target_name.c_str ();
}

int Animation::FindTarget (const char* target_name) const
{
  if (!target_name)
    throw xtl::make_null_argument_exception ("media::animation::Animation::FindTarget", "target_name");

  for (int i = 0, count = (int)impl->channel_groups.size (); i < count; i++)
    if (impl->channel_groups [i]->target_name == target_name)
      return i;

  return -1;
}

/*
   Количество каналов
*/

unsigned int Animation::ChannelsCount (unsigned int target_index) const
{
  if (target_index >= impl->channel_groups.size ())
    throw xtl::make_range_exception ("media::animation::Animation::ChannelsCount", "target_index", target_index, 0u, impl->channel_groups.size ());

  return (unsigned int)impl->channel_groups [target_index]->channels.size ();
}

/*
   Перебор каналов
*/

const animation::Channel& Animation::Channel (unsigned int target_index, unsigned int channel_index) const
{
  static const char* METHOD_NAME = "media::animation::Animation::ChannelsCount";

  if (target_index >= impl->channel_groups.size ())
    throw xtl::make_range_exception (METHOD_NAME, "target_index", target_index, 0u, impl->channel_groups.size ());

  ChannelGroupPtr group = impl->channel_groups [target_index];

  if (channel_index >= group->channels.size ())
    throw xtl::make_range_exception (METHOD_NAME, "channel_index", channel_index, 0u, group->channels.size ());

  return group->channels [channel_index];
}

animation::Channel& Animation::Channel (unsigned int target_index, unsigned int channel_index)
{
  return const_cast<animation::Channel&> (const_cast<const Animation&> (*this).Channel (target_index, channel_index));
}
          
/*
   Добавление/удаление каналов
*/

void Animation::AddChannel (unsigned int target_index, const animation::Channel& channel)
{
  if (target_index >= impl->channel_groups.size ())
    throw xtl::make_range_exception ("media::animation::Animation::AddChannel", "target_index", target_index, 0u, impl->channel_groups.size ());

  impl->channel_groups [target_index]->channels.push_back (channel);
}

void Animation::AddChannel (const char* target_name, const animation::Channel& channel)
{
  static const char* METHOD_NAME = "media::animation::Animation::AddChannel";

  if (!target_name)
    throw xtl::make_null_argument_exception (METHOD_NAME, "target_name");

  int target_index = FindTarget (target_name);

  ChannelGroupPtr group;

  if (target_index < 0)
  {
    group = ChannelGroupPtr (new ChannelGroup (target_name), false);

    if (impl->channel_groups.size () >= INT_MAX)
      throw xtl::format_operation_exception (METHOD_NAME, "Can't add channel, channel groups count limit exceeded");

    impl->channel_groups.push_back (group);
  }
  else
    group = impl->channel_groups [target_index];

  group->channels.push_back (channel);
}

void Animation::RemoveChannel (unsigned int target_index, unsigned int channel_index)
{
  if (target_index >= impl->channel_groups.size ())
    return;

  ChannelGroupPtr group = impl->channel_groups [target_index];

  if (channel_index >= group->channels.size ())
    return;

  group->channels.erase (group->channels.begin () + channel_index);

  if (group->channels.empty ())
    impl->channel_groups.erase (impl->channel_groups.begin () + target_index);
}

void Animation::RemoveAllChannels (unsigned int target_index)
{
  if (target_index >= impl->channel_groups.size ())
    return;

  impl->channel_groups.erase (impl->channel_groups.begin () + target_index);
}

void Animation::RemoveAllChannels ()
{
  impl->channel_groups.clear ();
}

/*
   Очередь событий
*/

const EventTrack& Animation::Events () const
{
  return impl->event_track;
}

EventTrack& Animation::Events ()
{
  return impl->event_track;
}

/*
    Получение временных лимитов
*/

void Animation::GetTimeLimits (float& min_time, float& max_time) const
{
  float min_unwrapped_time = 0.0f, max_unwrapped_time = 0.0f;
  
  impl->GetTimeLimits (min_time, max_time, min_unwrapped_time, max_unwrapped_time);
}

void Animation::GetTimeLimits (float& min_time, float& max_time, float& min_unwrapped_time, float& max_unwrapped_time) const
{
  impl->GetTimeLimits (min_time, max_time, min_unwrapped_time, max_unwrapped_time);  
}

float Animation::MinTime () const
{
  float min_time = 0.0f, max_time = 0.0f, min_unwrapped_time = 0.0f, max_unwrapped_time = 0.0f;

  impl->GetTimeLimits (min_time, max_time, min_unwrapped_time, max_unwrapped_time);  
  
  return min_time;
}

float Animation::MaxTime () const
{
  float min_time = 0.0f, max_time = 0.0f, min_unwrapped_time = 0.0f, max_unwrapped_time = 0.0f;

  impl->GetTimeLimits (min_time, max_time, min_unwrapped_time, max_unwrapped_time);  
  
  return max_time;
}

/*
    Минимальное / максимальное неотсеченное время (-INF/INF в случае открытого диапазона)
*/

float Animation::MinUnwrappedTime () const
{
  float min_time = 0.0f, max_time = 0.0f, min_unwrapped_time = 0.0f, max_unwrapped_time = 0.0f;

  impl->GetTimeLimits (min_time, max_time, min_unwrapped_time, max_unwrapped_time);  
  
  return min_unwrapped_time;
}

float Animation::MaxUnwrappedTime () const
{
  float min_time = 0.0f, max_time = 0.0f, min_unwrapped_time = 0.0f, max_unwrapped_time = 0.0f;

  impl->GetTimeLimits (min_time, max_time, min_unwrapped_time, max_unwrapped_time);  
  
  return max_unwrapped_time;
}

/*
   Обмен
*/

void Animation::Swap (Animation& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace animation
{

/*
   Обмен
*/

void swap (Animation& animation1, Animation& animation2)
{
  animation1.Swap (animation2);
}

}

}
