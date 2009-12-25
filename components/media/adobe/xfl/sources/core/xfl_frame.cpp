#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<FrameElement, ICollection<FrameElement> > FrameElementCollection;

/*
    Описание реализации кадра анимации
*/

struct Frame::Impl : public xtl::reference_counter
{
  stl::string            name;        //имя кадра
  size_t                 first_frame; //номер кадра (в глобальном времени), с которого начинается отображение данного кадра
  size_t                 duration;    //длительность отображения данного кадра в количестве кадров (в глобальном времени)
  FrameElementCollection elements;    //кадры анимации слоя
  AnimationCore          animation;   //анимация
};

/*
    Конструкторы / деструктор / присваивание
*/

Frame::Frame ()
  : impl (new Impl)
  {}
  
Frame::Frame (const Frame& source)
  : impl (source.impl)
{
  addref (impl);
}

Frame::~Frame ()
{
  release (impl);
}

Frame& Frame::operator = (const Frame& source)
{
  addref (source.impl);
  release (impl);

  impl = source.impl;

  return *this;
}

/*
   Имя
*/

const char* Frame::Name () const
{
  return impl->name.c_str ();
}

void Frame::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Frame::SetName", "name");

  impl->name = name;
}

/*
   Тайминги
*/

size_t Frame::FirstFrame () const
{
  return impl->first_frame;
}

size_t Frame::Duration () const
{
  return impl->duration;
}

void Frame::SetFirstFrame (size_t first_frame)
{
  impl->first_frame = first_frame;
}

void Frame::SetDuration (size_t duration)
{
  impl->duration = duration;
}

/*
   Кадры анимации слоя
*/

Frame::FrameElementList& Frame::Elements ()
{
  return const_cast<FrameElementList&> (const_cast<const Frame&> (*this).Elements ());
}

const Frame::FrameElementList& Frame::Elements () const
{
  return impl->elements;
}

FrameElement* Frame::FindElement (const char* element_name)
{
  return const_cast<FrameElement*> (const_cast<const Frame&> (*this).FindElement (element_name));
}

const FrameElement* Frame::FindElement (const char* element_name) const
{
  if (!element_name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Frame::FindElement", "element_name");

  for (FrameElementCollection::ConstIterator iter = impl->elements.CreateIterator (); iter; ++iter)
    if (!xtl::xstrcmp (element_name, iter->Name ()))
      return &(*iter);

  return 0;
}

/*
   Анимация кадра
*/

const AnimationCore& Frame::Animation () const
{
  return impl->animation;
}

void Frame::SetAnimation (const AnimationCore& animation)
{
  impl->animation = animation;
}

/*
   Обмен
*/

void Frame::Swap (Frame& frame)
{
  stl::swap (impl, frame.impl);
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

void swap (Frame& frame1, Frame& frame2)
{
  frame1.Swap (frame2);
}

}

}

}
