#include <time.h>

#include <platform/platform.h>

using namespace common;

size_t UnistdPlatform::GetMilliseconds ()
{
  timespec time;
  
  if (!clock_gettime (CLOCK_REALTIME, &time))
    return 0;
    
  return size_t (time.tv_sec * 1000.0 + time.tv_nsec / 1000000.0);
}
