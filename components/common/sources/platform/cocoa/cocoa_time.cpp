#include <CoreFoundation/CFDate.h>

#include <platform/platform.h>

using namespace common;

namespace
{

double launch_time = 0;

}

size_t CocoaPlatform::GetMilliseconds ()
{
  if (!launch_time)
    launch_time = CFAbsoluteTimeGetCurrent ();

  return (CFAbsoluteTimeGetCurrent () - launch_time) * 1000;
}
