#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<Frame, ICollection<Frame> > FrameCollection;

/*
    Описание реализации слоя анимации
*/

struct Layer::Impl : public xtl::reference_counter
{
  stl::string     name;   //имя слоя
  FrameCollection frames; //кадры
};

/*
    Конструкторы / деструктор / присваивание
*/

Layer::Layer ()
  : impl (new Impl)
  {}
  
Layer::Layer (const Layer& source)
  : impl (source.impl)
{
  addref (impl);
}

Layer::~Layer ()
{
  release (impl);
}

Layer& Layer::operator = (const Layer& source)
{
  addref (source.impl);
  release (impl);

  impl = source.impl;

  return *this;
}

/*
   Имя
*/

const char* Layer::Name () const
{
  return impl->name.c_str ();
}

void Layer::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Layer::SetName", "name");

  impl->name = name;
}

/*
   Кадры анимации слоя
*/

Layer::FrameList& Layer::Frames ()
{
  return const_cast<FrameList&> (const_cast<const Layer&> (*this).Frames ());
}

const Layer::FrameList& Layer::Frames () const
{
  return impl->frames;
}

Frame* Layer::FindFrame (const char* frame_name)
{
  return const_cast<Frame*> (const_cast<const Layer&> (*this).FindFrame (frame_name));
}

const Frame* Layer::FindFrame (const char* frame_name) const
{
  if (!frame_name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Layer::FindFrame", "frame_name");

  for (FrameCollection::ConstIterator iter = impl->frames.CreateIterator (); iter; ++iter)
    if (!xtl::xstrcmp (frame_name, iter->Name ()))
      return &(*iter);

  return 0;
}

/*
   Обмен
*/

void Layer::Swap (Layer& layer)
{
  stl::swap (impl, layer.impl);
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

void swap (Layer& layer1, Layer& layer2)
{
  layer1.Swap (layer2);
}

}

}

}
