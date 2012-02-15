#include "shared.h"

using namespace media::animation;

/*
    Описание реализации анимационного состояния
*/

struct AnimationState::Impl: public xtl::reference_counter, public xtl::trackable, public IAnimationState
{
  float time;   //текущее время
  float weight; //вес

///Конструктор
  Impl () : time (0.0f), weight (1.0f) {}
  
///Получение времени
  float Time () { return time; }
  
///Получение веса
  float Weight () { return weight; }
  
///Получение анимационного состояния
  AnimationState State () { return this; }
};

/*
    Конструкторы / деструктор / присваивание
*/

AnimationState::AnimationState ()
  : impl (new Impl)
{
}

//for IAnimationState::State
AnimationState::AnimationState (Impl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

AnimationState::AnimationState (const AnimationState& s)
  : impl (s.impl)
{
  addref (impl);
}

AnimationState::~AnimationState ()
{
  release (impl);
}

AnimationState& AnimationState::operator = (const AnimationState& s)
{
  AnimationState (s).Swap (*this);

  return *this;
}

/*
    Идентификатор
*/

size_t AnimationState::Id () const
{
  return reinterpret_cast<size_t> (impl);
}

/*
    Текущее время
*/

void AnimationState::SetTime (float time)
{
  impl->time = time;
}

float AnimationState::Time () const
{
  return impl->time;
}

/*
    Вес анимации
*/

void AnimationState::SetWeight (float weight)
{
  if (weight < 0.0f)
    weight = 0.0f;

  impl->weight = weight;
}

float AnimationState::Weight () const
{
  return impl->weight;
}

/*
    Получение интерфейса состояния анимации (общий для всех копий AnimationState)
*/

IAnimationState& AnimationState::AnimationStateCore () const
{
  return *impl;
}

/*
    Получение объекта оповещения об удалении
*/

xtl::trackable& AnimationState::GetTrackable () const
{
  return *impl;
}

namespace media
{

namespace animation
{

xtl::trackable& get_trackable (const AnimationState& s)
{
  return s.GetTrackable ();
}

}

}

/*
    Обмен
*/

void AnimationState::Swap (AnimationState& s)
{
  stl::swap (impl, s.impl);
}

namespace media
{

namespace animation
{

void swap (AnimationState& s1, AnimationState& s2)
{
  s1.Swap (s2);
}

}

}
