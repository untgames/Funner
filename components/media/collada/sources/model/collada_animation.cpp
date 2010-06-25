#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    Реализация Animation
*/

typedef media::CollectionImpl<Animation, ICollection<Animation> >               AnimationCollection;
typedef media::CollectionImpl<AnimationChannel, ICollection<AnimationChannel> > AnimationChannelCollection;

struct Animation::Impl: public xtl::reference_counter
{
  AnimationCollection        animations; //дочерние анимации
  AnimationChannelCollection channels;   //каналы анимации
  stl::string                id;         //идентификатор анимации
};

/*
    Конструктор
*/

Animation::Animation ()
  : impl (new Impl, false)
  {}

Animation::Animation (Impl* in_impl)
  : impl (in_impl, false)
  {}  
  
Animation::Animation (const Animation& animation)
  : impl (animation.impl)
  {}
  
Animation::~Animation ()
{
}

Animation& Animation::operator = (const Animation& animation)
{
  impl = animation.impl;
  
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
    Идентификатор анимации
*/

const char* Animation::Id () const
{
  return impl->id.c_str ();
}

void Animation::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::collada::Animation::SetId", "id");
    
  impl->id = id;
}

/*
    Список дочерних анимаций
*/

AnimationList& Animation::Animations ()
{
  return impl->animations;
}

const AnimationList& Animation::Animations () const
{
  return impl->animations;
}

/*
   Список каналов анимаций
*/

Animation::AnimationChannelList& Animation::Channels ()
{
  return impl->channels;
}

const Animation::AnimationChannelList& Animation::Channels () const
{
  return impl->channels;
}
