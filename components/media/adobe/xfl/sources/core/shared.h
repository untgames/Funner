#ifndef MEDIALIB_ADOBE_XFL_SHARED_HEADER
#define MEDIALIB_ADOBE_XFL_SHARED_HEADER

#include <stl/algorithm>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/iterator.h>
#include <xtl/reference_counter.h>

#include <common/component.h>

#include <media/collection.h>

#include <media/adobe/xfl.h>

namespace media
{

namespace adobe
{

namespace xfl
{

template <class T>
const char* get_name (const T& object)
{
  return object.Name ();
}

}

}

}

#endif
