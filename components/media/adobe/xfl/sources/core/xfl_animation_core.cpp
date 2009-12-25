#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<PropertyAnimation, INamedCollection<PropertyAnimation> > PropertyAnimationCollection;

/*
   Описание реализации анимации кадра
*/

struct AnimationCore::Impl : public xtl::reference_counter
{
  float                       duration;       //длительность в секундах
  bool                        orient_to_path; //автоориентация по пути
  PropertyAnimationCollection properties;     //анимируемые свойства

  Impl () : duration (0.0f), orient_to_path (false) {}
};

/*
    Конструкторы / деструктор / присваивание
*/

AnimationCore::AnimationCore ()
  : impl (new Impl)
  {}
  
AnimationCore::AnimationCore (const AnimationCore& source)
  : impl (source.impl)
{
  addref (impl);
}

AnimationCore::~AnimationCore ()
{
  release (impl);
}

AnimationCore& AnimationCore::operator = (const AnimationCore& source)
{
  AnimationCore (source).Swap (*this);

  return *this;
}

/*
   Длительность в секундах
*/

float AnimationCore::Duration () const
{
  return impl->duration;
}

void AnimationCore::SetDuration (float duration)
{
  impl->duration = duration;
}

/*
   Должна ли происходить автоориентация по пути
*/

bool AnimationCore::OrientToPath () const
{
  return impl->orient_to_path;
}

void AnimationCore::SetOrientToPath (bool orient_to_path)
{
  impl->orient_to_path = orient_to_path;
}

/*
   Анимируемые свойства
*/

AnimationCore::PropertyAnimationList& AnimationCore::Properties ()
{
  return const_cast<PropertyAnimationList&> (const_cast<const AnimationCore&> (*this).Properties ());
}

const AnimationCore::PropertyAnimationList& AnimationCore::Properties () const
{
  return impl->properties;
}

/*
   Обмен
*/

void AnimationCore::Swap (AnimationCore& animation)
{
  stl::swap (impl, animation.impl);
}

namespace media
{

namespace adobe
{

namespace xfl
{

/*
   Обмен
*/

void swap (AnimationCore& animation1, AnimationCore& animation2)
{
  animation1.Swap (animation2);
}

}

}

}
