#ifndef PHYSICS_LOW_LEVEL_COMMON_HEADER
#define PHYSICS_LOW_LEVEL_COMMON_HEADER

#include <math/vector.h>
#include <math/quat.h>

namespace physics
{

namespace low_level
{

///////////////////////////////////////////////////////////////////////////////////////////////////
///Описание положения объекта
///////////////////////////////////////////////////////////////////////////////////////////////////
struct Transform
{
  math::vec3f position;
  math::quatf orientation;
};

}

}

#endif
