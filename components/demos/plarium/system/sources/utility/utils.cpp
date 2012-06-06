#include "shared.h"

namespace
{

std::string vformat (const char* format,va_list list)
{
  if (!format)
    return "";

  int size = vsnprintf (0,0,format,list);

  if (size == -1)
    return "";

  std::string str;

  str.resize (size);

  vsnprintf (&str [0],size+1,format,list);

  return str;
}

}

namespace plarium
{

namespace utility
{

std::string format (const char* format,...)
{
  va_list list;

  va_start (list,format);

  return vformat (format,list);
}

//Get current time
size_t milliseconds ()
{
#ifdef _MSC_VER
  return size_t (clock () * 1000.0 / CLOCKS_PER_SEC);
#elif defined (__APPLE__)
  static double launch_time = 0;

  if (!launch_time)
    launch_time = CFAbsoluteTimeGetCurrent ();

  return (CFAbsoluteTimeGetCurrent () - launch_time) * 1000;
#else
  timespec time;

  if (clock_gettime (CLOCK_MONOTONIC, &time) != 0)
    return 0;

  return size_t (time.tv_sec * 1000.0 + time.tv_nsec / 1000000.0);
#endif
}

}

}
