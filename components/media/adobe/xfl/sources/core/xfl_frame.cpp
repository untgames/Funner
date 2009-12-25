#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<FrameElement, INamedCollection<FrameElement> > FrameElementCollection;

/*
    Описание реализации кадра анимации
*/

struct Frame::Impl : public xtl::reference_counter
{
  size_t                 first_frame; //номер кадра (в глобальном времени), с которого начинается отображение данного кадра
  size_t                 duration;    //длительность отображения данного кадра в количестве кадров (в глобальном времени)
  FrameElementCollection elements;    //кадры анимации слоя
  AnimationCore          animation;   //анимация
  
  Impl ()
    : first_frame (0)
    , duration (0)
  { }
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
  Frame (source).Swap (*this);

  return *this;
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
