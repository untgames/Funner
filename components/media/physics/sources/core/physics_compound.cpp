#include "shared.h"

using namespace media::physics;
using namespace media::physics::shapes;

namespace
{

struct CompoundShapeElement : public xtl::reference_counter
{
  Shape                    shape;
  Compound::ShapeTransform transform;

  CompoundShapeElement () {}

  CompoundShapeElement (const Shape& in_shape, const Compound::ShapeTransform& in_transform)
    : shape (in_shape), transform (in_transform)
    {}
};

typedef xtl::intrusive_ptr<CompoundShapeElement> CompoundShapeElementPtr;
typedef stl::vector<CompoundShapeElementPtr>     ShapesArray;

}

/*
   Положение элемента тела
*/

Compound::ShapeTransform::ShapeTransform ()
  {}

Compound::ShapeTransform::ShapeTransform (math::vec3f in_position, math::quatf in_orientation)
  : position (in_position), orientation (in_orientation)
  {}

/*
   Реализация описания тела в виде набора тел
*/

struct Compound::Impl : public xtl::reference_counter
{
  ShapesArray shapes;

  Impl () {}

  Impl (const Impl& source)
  {
    shapes.reserve (source.shapes.size ());

    for (ShapesArray::const_iterator iter = source.shapes.begin (), end = source.shapes.end (); iter != end; ++iter)
      shapes.push_back (CompoundShapeElementPtr (new CompoundShapeElement ((*iter)->shape.Clone (), (*iter)->transform), false));
  }
};

/*
   Конструкторы / деструктор / присваивание
*/

Compound::Compound ()
  : impl (new Impl)
{
}

Compound::Compound (const Compound& source)
  : impl (source.impl)
{
  addref (impl);
}

Compound::Compound (Impl* in_impl)
  : impl (in_impl)
{
}

Compound::~Compound ()
{
  release (impl);
}

Compound& Compound::operator = (const Compound& source)
{
  Compound (source).Swap (*this);

  return *this;
}
    
/*
   Создание копии
*/

Compound Compound::Clone () const
{
  return Compound (new Impl (*impl));
}

/*
   Идентификатор
*/

size_t Compound::Id () const
{
  return (size_t)impl;
}

/*
   Добавление / удаление тел
*/

size_t Compound::Size () const
{
  return impl->shapes.size ();
}

size_t Compound::Capacity () const
{
  return impl->shapes.capacity ();
}

void Compound::Reserve (size_t size)
{
  impl->shapes.reserve (size);
}

void Compound::Attach (const media::physics::Shape& shape, const math::vec3f& position, const math::quatf& orientation)
{
  Attach (shape, ShapeTransform (position, orientation));
}

void Compound::Attach (const media::physics::Shape& shape, const ShapeTransform& transform)
{
  impl->shapes.push_back (CompoundShapeElementPtr (new CompoundShapeElement (shape, transform), false));
}

void Compound::Detach (const media::physics::Shape& shape)
{
  for (size_t i = 0; i < impl->shapes.size ();)
  {
    if (impl->shapes [i]->shape.Id () == shape.Id ())
      impl->shapes.erase (impl->shapes.begin () + i);
    else
      i++;
  }
}

void Compound::Detach (size_t index)
{
  if (index >= impl->shapes.size ())
    return;

  impl->shapes.erase (impl->shapes.begin () + index);
}

void Compound::DetachAll ()
{
  impl->shapes.clear ();
}

/*
   Получение тел
*/

const media::physics::Shape& Compound::Shape (size_t index) const
{
  return const_cast<Compound&> (*this).Shape (index);
}

media::physics::Shape& Compound::Shape (size_t index)
{
  if (index > impl->shapes.size ())
    throw xtl::make_range_exception ("media::physics::shapes::Compound::Shape", "index", index, 0u, impl->shapes.size ());

  return impl->shapes [index]->shape;
}

const Compound::ShapeTransform& Compound::Transform (size_t index) const
{
  return const_cast<Compound&> (*this).Transform (index);
}

Compound::ShapeTransform& Compound::Transform (size_t index)
{
  if (index > impl->shapes.size ())
    throw xtl::make_range_exception ("media::physics::shapes::Compound::Transform", "index", index, 0u, impl->shapes.size ());

  return impl->shapes [index]->transform;
}

/*
   Обмен
*/

void Compound::Swap (Compound& source)
{
  stl::swap (impl, source.impl);
}

namespace media
{

namespace physics
{

namespace shapes
{

/*
   Обмен
*/

void swap (Compound& compound1, Compound& compound2)
{
  compound1.Swap (compound2);
}

}

}

}
