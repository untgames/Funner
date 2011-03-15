#include "shared.h"

using namespace physics;

namespace
{

struct ShapeListEntry : public xtl::reference_counter
{
  Shape     shape;
  Transform transform;

  ShapeListEntry (const Shape& in_shape, const Transform& in_transform)
    : shape (in_shape), transform (in_transform)
    {}
};

typedef xtl::intrusive_ptr<ShapeListEntry> ShapeListEntryPtr;
typedef stl::vector<ShapeListEntryPtr>     ShapeListArray;

}

/*
   Описание составного тела
*/

struct ShapeList::Impl : public xtl::reference_counter
{
  ShapeListArray shapes;
};

ShapeList::ShapeList ()
  : impl (new Impl)
  {}

ShapeList::ShapeList (const ShapeList& source)
  : impl (source.impl)
{
  addref (impl);
}

ShapeList::~ShapeList ()
{
  release (impl);
}

ShapeList& ShapeList::operator = (ShapeList& source)
{
  ShapeList (source).Swap (*this);

  return *this;
}

/*
   Добавление/удаление тел
*/

void ShapeList::Add (const Shape& shape, const physics::Transform& transform)
{
  impl->shapes.push_back (ShapeListEntryPtr (new ShapeListEntry (shape, transform), false));
}

void ShapeList::Add (const Shape& shape, const math::vec3f& position, const math::quatf& orientation)
{
  Add (shape, physics::Transform (position, orientation));
}

void ShapeList::Add (const Shape& shape, const math::quatf& orientation)
{
  Add (shape, physics::Transform (orientation));
}

void ShapeList::Remove (size_t index)
{
  if (index >= impl->shapes.size ())
    return;

  impl->shapes.erase (impl->shapes.begin () + index);
}

void ShapeList::Clear ()
{
  impl->shapes.clear ();
}

/*
   Получение тел
*/

size_t ShapeList::Size () const
{
  return impl->shapes.size ();
}

Shape& ShapeList::operator [] (size_t index)
{
  if (index >= impl->shapes.size ())
    throw xtl::make_range_exception ("physics::ShapeList::operator []", "index", index, 0u, impl->shapes.size ());

  return impl->shapes [index]->shape;
}

const Shape& ShapeList::operator [] (size_t index) const
{
  return const_cast<ShapeList&> (*this) [index];
}

physics::Transform& ShapeList::Transform (size_t index)
{
  if (index >= impl->shapes.size ())
    throw xtl::make_range_exception ("physics::ShapeList::Transform", "index", index, 0u, impl->shapes.size ());

  return impl->shapes [index]->transform;
}

const physics::Transform& ShapeList::Transform (size_t index) const
{
  return const_cast<ShapeList&> (*this).Transform (index);
}

/*
   Обмен
*/

void ShapeList::Swap (ShapeList& shape_list)
{
  stl::swap (impl, shape_list.impl);
}

namespace physics
{

/*
   Обмен
*/

void swap (ShapeList& shape_list1, ShapeList& shape_list2)
{
  shape_list1.Swap (shape_list2);
}

}
