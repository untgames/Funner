#ifndef MEDIALIB_RFX_SHARED_HEADER
#define MEDIALIB_RFX_SHARED_HEADER

#include <memory.h>

#include <stl/string>
#include <stl/vector>
#include <stl/bitset>

#include <xtl/visitor.h>
#include <xtl/reference_counter.h>

#include <common/exception.h>
#include <common/strwrap.h>
#include <common/singleton.h>

#include <media/rfx/common_shader.h>
#include <media/rfx/material_library.h>

#include <shared/resource_holder.h>
#include <shared/clone.h>
#include <shared/resource_manager.h>

namespace media
{

namespace rfx
{

/*
    Система управления материалами
*/

class MaterialSystemImpl: public ResourceManager<MaterialSystem::LoadHandler, MaterialSystem::SaveHandler>
{
  public:
    MaterialSystemImpl ();
};

typedef common::Singleton<MaterialSystemImpl> MaterialSystemSingleton;

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
