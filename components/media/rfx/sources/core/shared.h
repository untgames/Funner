#ifndef MEDIALIB_RFX_SHARED_HEADER
#define MEDIALIB_RFX_SHARED_HEADER

#include <stl/bitset>
#include <stl/string>

#include <xtl/visitor.h>
#include <xtl/reference_counter.h>

#include <common/component.h>

#include <media/rfx/sprite_material.h>
#include <media/rfx/material_library.h>

#include <media/resource_library.h>

namespace media
{

namespace rfx
{

/*
    Утилиты
*/

//отсечение по границе
inline float clamp (float value, float min_value, float max_value)
{
  if (value < min_value) return min_value;
  if (value > max_value) return max_value;
  
  return value;
}

}

}

#endif
