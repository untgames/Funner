#include "shared.h"

using namespace media;
using namespace media::animation;

/*
   Реализация анимации
*/

typedef stl::vector<Channel>   ChannelsArray;
typedef stl::vector<Animation> AnimationsArray;

struct Animation::Impl : public xtl::reference_counter
{
  stl::string     name;             //имя анимации
  stl::string     target_name;      //имя анимируемого объекта
  EventTrack      event_track;      //трек событий
  ChannelsArray   channels;         //каналы анимации
  AnimationsArray sub_animations; //дочерние анимации

  Impl () {}

  Impl (const Impl& source)
    : name (source.name), target_name (source.target_name), event_track (source.event_track.Clone ())
  {
    channels.reserve (source.channels.size ());

    for (ChannelsArray::const_iterator iter = source.channels.begin (), end = source.channels.end (); iter != end; ++iter)
      channels.push_back (iter->Clone ());

    sub_animations.reserve (source.sub_animations.size ());

    for (AnimationsArray::const_iterator iter = source.sub_animations.begin (), end = source.sub_animations.end (); iter != end; ++iter)
      sub_animations.push_back (iter->Clone ());
  }

  void CheckAnimationBindToItself (const Animation& sub_animation, const Animation& parent_animation)
  {
    if (parent_animation.impl == sub_animation.impl)
      throw xtl::make_argument_exception ("media::animation::Animation::AddSubAnimation", "animation", "Attempt to add animation to one of it's sub animations");

    for (AnimationsArray::iterator iter = sub_animation.impl->sub_animations.begin (), end = sub_animation.impl->sub_animations.end (); iter != end; ++iter)
      iter->impl->CheckAnimationBindToItself (*iter, parent_animation);
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
   Имя анимируемого объекта
*/

void Animation::SetTargetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::animation::Animation::SetTargetName", "name");

  impl->target_name = name;
}

const char* Animation::TargetName () const
{
  return impl->target_name.c_str ();
}
    
/*
   Количество каналов
*/

size_t Animation::ChannelsCount () const
{
  return impl->channels.size ();
}

/*
   Перебор каналов
*/

const animation::Channel& Animation::Channel (size_t index) const
{
  if (index >= impl->channels.size ())
    throw xtl::make_range_exception ("media::animation::Animation::Channel", "index", index, 0u, impl->channels.size ());

  return impl->channels [index];
}

animation::Channel& Animation::Channel (size_t index)
{
  return const_cast<animation::Channel&> (const_cast<const Animation&> (*this).Channel (index));
}
          
/*
   Добавление/удаление каналов
*/

size_t Animation::AddChannel (const animation::Channel& channel)
{
  impl->channels.push_back (channel);

  return impl->channels.size () - 1;
}

void Animation::RemoveChannel (size_t channel_index)
{
  if (channel_index >= impl->channels.size ())
    return;

  impl->channels.erase (impl->channels.begin () + channel_index);
}

void Animation::RemoveAllChannels ()
{
  impl->channels.clear ();
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
   Количество вложенных анимаций
*/

size_t Animation::SubAnimationsCount () const
{
  return impl->sub_animations.size ();
}

/*
   Перебор анимаций
*/

const Animation& Animation::SubAnimation (size_t index) const
{
  if (index >= impl->sub_animations.size ())
    throw xtl::make_range_exception ("media::animation::Animation::SubAnimation", "index", index, 0u, impl->channels.size ());

  return impl->sub_animations [index];
}

Animation& Animation::SubAnimation (size_t index)
{
  return const_cast<animation::Animation&> (const_cast<const Animation&> (*this).SubAnimation (index));
}

/*
   Добавление/удаление анимаций
*/

size_t Animation::AddSubAnimation (const Animation& animation)
{
  impl->CheckAnimationBindToItself (animation, *this);

  impl->sub_animations.push_back (animation);

  return impl->sub_animations.size () - 1;
}

void Animation::RemoveSubAnimation (size_t animation_index)
{
  if (animation_index >= impl->sub_animations.size ())
    return;

  impl->sub_animations.erase (impl->sub_animations.begin () + animation_index);
}

void Animation::RemoveAllSubAnimations ()
{
  impl->sub_animations.clear ();
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
