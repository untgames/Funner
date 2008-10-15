#include "shared.h"

using namespace common;

size_t DefaultPlatform::GetMilliseconds ()
{
  return size_t (clock () * 1000.0f / CLOCKS_PER_SEC);
}
