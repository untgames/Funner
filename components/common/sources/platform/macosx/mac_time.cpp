#include <platform/platform.h>

#include <Timer.h>

using namespace common;

size_t MacOsXPlatform::GetMilliseconds ()
{
  UnsignedWide microseconds;

  Microseconds (&microseconds);

  return static_cast<size_t> ((*reinterpret_cast<unsigned long long*> (&microseconds) + 500.0) / 1000.0);
}
