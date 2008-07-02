#ifndef MEDIALIB_RFX_SHARED_HEADER
#define MEDIALIB_RFX_SHARED_HEADER

#include <stl/bitset>
#include <stl/string>

#include <xtl/visitor.h>
#include <xtl/reference_counter.h>

#include <common/component.h>

#include <media/rfx/common_material.h>
#include <media/rfx/multipass_material.h>
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

template <class T, size_t Size>
inline math::vec<T, Size> clamp
 (const math::vec<T, Size>& value,
  const math::vec<T, Size>& min_value,
  const math::vec<T, Size>& max_value)
{
  math::vec<T, Size> result;

  for (size_t i=0; i<Size; i++)
    result [i] = clamp (value [i], min_value [i], max_value [i]);

  return result;    
}

}

}

#endif
