#ifndef MEDIALIB_RFX_SHARED_HEADER
#define MEDIALIB_RFX_SHARED_HEADER

#include <memory.h>
#include <stl/string>
#include <stl/vector>
#include <stl/bitset>
#include <xtl/visitor.h>
#include <common/exception.h>
#include <common/strwrap.h>
#include <media/rfx/common_shader.h>
#include <media/rfx/material_library.h>
#include <shared/shared_resource.h>

namespace medialib
{

namespace rfx
{

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
