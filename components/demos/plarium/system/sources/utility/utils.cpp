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

}

}
