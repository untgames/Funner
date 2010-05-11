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

size_t Animation::TargetsCount () const
{
  return impl->channel_groups.size ();
}

const char* Animation::TargetName (size_t target_index) const
{
  if (target_index >= impl->channel_groups.size ())
    throw xtl::make_range_exception ("media::animation::Animation::TargetName", "target_index", target_index, 0u, impl->channel_groups.size ());

  return impl->channel_groups [target_index]->target_name.c_str ();
}

int Animation::FindTarget (const char* target_name) const
{
  if (!target_name)
    throw xtl::make_null_argument_exception ("media::animation::Animation::FindTarget", "target_name");

  for (size_t i = 0, count = impl->channel_groups.size (); i < count; i++)
    if (impl->channel_groups [i]->target_name == target_name)
      return i;

  return -1;
}

/*
   Количество каналов
*/

size_t Animation::ChannelsCount (size_t target_index) const
{
  if (target_index >= impl->channel_groups.size ())
    throw xtl::make_range_exception ("media::animation::Animation::ChannelsCount", "target_index", target_index, 0u, impl->channel_groups.size ());

  return impl->channel_groups [target_index]->channels.size ();
}

/*
   Перебор каналов
*/

const animation::Channel& Animation::Channel (size_t target_index, size_t channel_index) const
{
  static const char* METHOD_NAME = "media::animation::Animation::ChannelsCount";

  if (target_index >= impl->channel_groups.size ())
    throw xtl::make_range_exception (METHOD_NAME, "target_index", target_index, 0u, impl->channel_groups.size ());

  ChannelGroupPtr group = impl->channel_groups [target_index];

  if (channel_index >= group->channels.size ())
    throw xtl::make_range_exception (METHOD_NAME, "channel_index", channel_index, 0u, group->channels.size ());

  return group->channels [channel_index];
}

animation::Channel& Animation::Channel (size_t target_index, size_t channel_index)
{
  return const_cast<animation::Channel&> (const_cast<const Animation&> (*this).Channel (target_index, channel_index));
}
          
/*
   Добавление/удаление каналов
*/

void Animation::AddChannel (size_t target_index, const animation::Channel& channel)
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
    impl->channel_groups.push_back (group);
  }
  else
    group = impl->channel_groups [target_index];

  group->channels.push_back (channel);
}

void Animation::RemoveChannel (size_t target_index, size_t channel_index)
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

void Animation::RemoveAllChannels (size_t target_index)
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

const EventTrack Animation::Events () const
{
  return impl->event_track;
}

EventTrack Animation::Events ()
{
  return impl->event_track;
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
