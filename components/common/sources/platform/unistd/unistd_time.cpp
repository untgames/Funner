#include <time.h>
#include <stdio.h>

#include <platform/platform.h>

using namespace common;

size_t UnistdPlatform::GetMilliseconds ()
{
#if defined (BEAGLEBOARD) || defined (__APPLE__) || defined (ANDROID) || defined (MEEGO)
  return DefaultPlatform::GetMilliseconds ();  

#else
  timespec time;
  
  if (clock_gettime (CLOCK_REALTIME, &time) != 0)
    return 0;
    
  return size_t (time.tv_sec * 1000.0 + time.tv_nsec / 1000000.0);
#endif
}
