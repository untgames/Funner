#include "shared.h"

namespace
{

sgi_stl::string vformat (const char* format,va_list list)
{
  if (!format)
    return "";

  int size = vsnprintf (0,0,format,list);

  if (size == -1)
    return "";

  sgi_stl::string str;

  str.resize (size);

  vsnprintf (&str [0],size+1,format,list);

  return str;
}

}

namespace plarium
{

namespace utility
{

sgi_stl::string format (const char* format,...)
{
  va_list list;

  va_start (list,format);

  return vformat (format,list);
}

sgi_stl::vector<sgi_stl::string> split (const char* str, const char* delimiters, const char* brackets)
{
  sgi_stl::vector<sgi_stl::string> return_value;
  const char*              start;               //next word start
  bool                     start_found = false; //was start found
  const char*              active_bracket = 0;

  for (; *str; str++)
  {
    if (start_found && active_bracket)
    {
      if (*active_bracket == *str)
      {
        return_value.push_back (sgi_stl::string (start, str - start));
        start_found    = false;
        active_bracket = 0;
      }

      continue;
    }

    bool is_delimiter = false;

    for (const char* delimiter = delimiters; *delimiter; delimiter++)
    {
      if (*delimiter == *str)
      {
        is_delimiter = true;
        break;
      }
    }

    if (is_delimiter)
    {
      if (start_found)
      {
        return_value.push_back (sgi_stl::string (start, str - start));
        start_found = false;
      }
    }
    else
    {
      if (!start_found)
      {
        start = str;
        start_found = true;

        for (const char* bracket = brackets; *bracket; bracket++)
        {
          if (*bracket == *str)
          {
            active_bracket = bracket;
            start++;
            break;
          }
        }
      }
    }
  }

  if (start_found && *start)      //push last word
    return_value.push_back (sgi_stl::string (start));

  return return_value;
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
