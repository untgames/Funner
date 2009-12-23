#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<PropertyAnimationKeyframe, ICollection<PropertyAnimationKeyframe> > KeyframeCollection;

/*
    Описание реализации анимируемого свойства
*/

struct PropertyAnimation::Impl : public xtl::reference_counter
{
  stl::string        id;        //идентификатор типа свойства
  bool               enabled;   //статус
  KeyframeCollection keyframes; //ключевые точки
};

/*
    Конструкторы / деструктор / присваивание
*/

PropertyAnimation::PropertyAnimation ()
  : impl (new Impl)
  {}
  
PropertyAnimation::PropertyAnimation (const PropertyAnimation& source)
  : impl (source.impl)
{
  addref (impl);
}

PropertyAnimation::~PropertyAnimation ()
{
  release (impl);
}

PropertyAnimation& PropertyAnimation::operator = (const PropertyAnimation& source)
{
  addref (source.impl);
  release (impl);

  impl = source.impl;

  return *this;
}

/*
   Идентификатор типа свойства
*/

const char* PropertyAnimation::Id () const
{
  return impl->id.c_str ();
}

void PropertyAnimation::SetId (const char* id)
{
  if (!id)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::PropertyAnimation::SetId", "id");

  impl->id = id;
}

/*
   Включена ли анимация для этого свойства
*/

bool PropertyAnimation::Enabled () const
{
  return impl->enabled;
}

void PropertyAnimation::SetEnabled (bool enabled)
{
  impl->enabled = enabled;
}

/*
   Анимации
*/

PropertyAnimation::KeyframeList& PropertyAnimation::Keyframes ()
{
  return const_cast<KeyframeList&> (const_cast<const PropertyAnimation&> (*this).Keyframes ());
}

const PropertyAnimation::KeyframeList& PropertyAnimation::Keyframes () const
{
  return impl->keyframes;
}

/*
   Обмен
*/

void PropertyAnimation::Swap (PropertyAnimation& animation)
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

void swap (PropertyAnimation& animation1, PropertyAnimation& animation2)
{
  animation1.Swap (animation2);
}

}

}

}
