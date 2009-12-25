#include "shared.h"

using namespace media::adobe::xfl;

typedef media::CollectionImpl<Layer, INamedCollection<Layer> > LayerCollection;

/*
    Описание реализации анимации
*/

struct Timeline::Impl : public xtl::reference_counter
{
  stl::string     name;   //имя анимации
  LayerCollection layers; //слои
};

/*
    Конструкторы / деструктор / присваивание
*/

Timeline::Timeline ()
  : impl (new Impl)
  {}
  
Timeline::Timeline (const Timeline& source)
  : impl (source.impl)
{
  addref (impl);
}

Timeline::~Timeline ()
{
  release (impl);
}

Timeline& Timeline::operator = (const Timeline& source)
{
  Timeline (source).Swap (*this);

  return *this;
}

/*
   Имя
*/

const char* Timeline::Name () const
{
  return impl->name.c_str ();
}

void Timeline::SetName (const char* name)
{
  if (!name)
    throw xtl::make_null_argument_exception ("media::adobe::xfl::Timeline::SetName", "name");

  impl->name = name;
}

/*
   Слои анимации
*/

Timeline::LayerList& Timeline::Layers ()
{
  return const_cast<LayerList&> (const_cast<const Timeline&> (*this).Layers ());
}

const Timeline::LayerList& Timeline::Layers () const
{
  return impl->layers;
}

/*
   Обмен
*/

void Timeline::Swap (Timeline& timeline)
{
  stl::swap (impl, timeline.impl);
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

void swap (Timeline& timeline1, Timeline& timeline2)
{
  timeline1.Swap (timeline2);
}

}

}

}
