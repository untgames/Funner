#include <cstdio>
#include <exception>

#include <stl/string>

#include <xtl/iterator.h>
#include <xtl/string.h>

#include <common/log.h>
#include <common/property_map.h>
#include <common/strlib.h>

#include <media/particles/particle_system_library.h>

using namespace media::particles;
using namespace common;

struct MyRand
{
  uint32_t next;

  MyRand () : next (0) {}

  float rand ()
  {
    static const size_t MAX_RAND = 10000;

    next = next * 1103515245 + 12345;

    return ((size_t)(next / 65536) % MAX_RAND) / (float)MAX_RAND * 2 - 1;
  }

  void operator () (size_t count, float* values)
  {
    for (size_t i = 0; i < count; i++)
    {
      values [i] = rand ();
    }
  }
};

