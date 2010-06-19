#include "shared.h"

using namespace media::collada;
using namespace common;

/*
    Реализация AnimationChannel
*/

typedef xtl::uninitialized_storage <char> SamplesStorage;

struct AnimationChannel::Impl: public xtl::reference_counter
{
  stl::string              target;    //идентификатор анимируемого объекта
  stl::string              parameter; //идентификатор анимируемого параметра
  AnimationChannelSemantic semantic;  //семантика анимации
  SamplesStorage           samples;   //данные канала

  Impl (AnimationChannelSemantic inSemantic)
    : semantic (inSemantic)
  {
    switch (semantic)
    {
      case AnimationChannelSemantic_Transform:
        break;
      default:
        throw xtl::make_argument_exception ("media::collada::AnimationChannel::AnimationChannel", "semantic", semantic);
    }
  }

  size_t SampleSize ()
  {
    switch (semantic)
    {
      case AnimationChannelSemantic_Transform: return sizeof (AnimationSampleTransform);
      default:
        throw xtl::format_operation_exception ("media::collada::AnimationChannel::Impl::SampleSize", "Unsupported sample type");
    }
  }
};

/*
    Конструктор
*/

AnimationChannel::AnimationChannel (AnimationChannelSemantic semantic)
  : impl (new Impl (semantic), false)
  {}

AnimationChannel::AnimationChannel (Impl* in_impl)
  : impl (in_impl, false)
  {}  
  
AnimationChannel::AnimationChannel (const AnimationChannel& animation)
  : impl (animation.impl)
  {}
  
AnimationChannel::~AnimationChannel ()
{
}

AnimationChannel& AnimationChannel::operator = (const AnimationChannel& animation)
{
  impl = animation.impl;
  
  return *this;
}

/*
    Создание копии
*/

AnimationChannel AnimationChannel::Clone () const
{
  return AnimationChannel (new Impl (*impl));
}

/*
   Имя анимируемого объекта
*/

const char* AnimationChannel::TargetName () const
{
  return impl->target.c_str ();
}

void AnimationChannel::SetTargetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::collada::AnimationChannel::SetTargetName", "name");

  impl->target = name;
}

/*
   Имя анимируемого параметра
*/

const char* AnimationChannel::ParameterName () const
{
  return impl->parameter.c_str ();
}

void AnimationChannel::SetParameterName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::collada::AnimationChannel::SetParameterName", "name");

  impl->parameter = name;
}

/*
   Семантика анимации
*/

AnimationChannelSemantic AnimationChannel::Semantic () const
{
  return impl->semantic;
}

/*
   Анимационный трек
*/

void AnimationChannel::SetSamplesCount (size_t new_size)
{
  impl->samples.resize (new_size * impl->SampleSize (), false);
}

size_t AnimationChannel::SamplesCount () const
{
  return impl->samples.size () / impl->SampleSize ();
}

namespace media
{

namespace collada
{

template <>
const AnimationSampleTransform* AnimationChannel::Samples () const
{
  if (impl->semantic != AnimationChannelSemantic_Transform)
    return 0;

  return (const AnimationSampleTransform*)impl->samples.data ();
}

template <>
AnimationSampleTransform* AnimationChannel::Samples ()
{
  return const_cast<AnimationSampleTransform*> (const_cast<const AnimationChannel&> (*this).Samples<AnimationSampleTransform> ());
}

}

}
