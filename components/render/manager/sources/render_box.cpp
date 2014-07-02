#include "shared.h"

using namespace render::manager;

Box::Box ()
{
}
  
Box::Box (const math::vec3f& in_min_extent, const math::vec3f& in_max_extent)
  : min_extent (in_min_extent)
  , max_extent (in_max_extent)
{
}

bool Box::operator == (const Box& area) const
{
  return min_extent == area.min_extent && max_extent == area.max_extent;
}

bool Box::operator != (const Box& area) const
{
  return !(*this == area);
}
