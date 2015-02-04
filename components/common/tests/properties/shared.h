#ifndef COMMONLIB_PROPERTIES_TESTS_SHARED_HEADER
#define COMMONLIB_PROPERTIES_TESTS_SHARED_HEADER

#include <cstdio>
#include <exception>

#include <stl/string>

#include <xtl/bind.h>
#include <xtl/lexical_cast.h>
#include <xtl/ref.h>
#include <xtl/signal.h>
#include <xtl/uninitialized_storage.h>

#include <math/vector.h>
#include <math/matrix.h>

#include <common/property_map.h>
#include <common/property_binding_map.h>
#include <common/strlib.h>

using namespace common;

inline void dump (const PropertyLayout& layout, size_t correct_hash)
{
  printf ("layout: size=%u, buffer_size=%u, hash_correct=%d:\n", layout.Size (), layout.BufferSize (), layout.Hash () == correct_hash);
  
  for (size_t i=0; i<layout.Size (); i++)
  {
    printf ("  #%u: name='%s', type=%s, elements_count=%u\n", i, layout.PropertyName (i), get_name (layout.PropertyType (i)),
      layout.PropertyElementsCount (i));
  }
}

#endif
