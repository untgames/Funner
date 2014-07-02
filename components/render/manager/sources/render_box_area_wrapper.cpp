#include "shared.h"

using namespace render::manager;

BoxArea::BoxArea ()
  : impl (new BoxAreaImpl)
{
}

BoxArea::BoxArea (const render::manager::Box& in_box)
  : impl (new BoxAreaImpl (in_box))
{
}

BoxArea::BoxArea (BoxAreaImpl* in_impl)
  : impl (in_impl)
{
  addref (impl);
}

BoxArea::~BoxArea ()
{
  release (impl);
}

BoxArea& BoxArea::operator = (const BoxArea& area)
{
  BoxArea (area).Swap (*this);
  
  return *this;
}

void BoxArea::SetBox (const render::manager::Box& box)
{
  impl->SetBox (box);
}

void BoxArea::SetBox (const math::vec3f& min_extent, const math::vec3f& max_extent)
{
  impl->SetBox (render::manager::Box (min_extent, max_extent));
}

const render::manager::Box& BoxArea::Box () const
{
  return impl->Box ();
}

const math::vec3f& BoxArea::Min () const
{
  return impl->Box ().min_extent;
}

const math::vec3f& BoxArea::Max () const
{
  return impl->Box ().max_extent;
}

void BoxArea::Swap (BoxArea& area)
{
  stl::swap (impl, area.impl);
}

namespace render
{

namespace manager
{

void swap (BoxArea& area1, BoxArea& area2)
{
  area1.Swap (area2);
}

}

}
