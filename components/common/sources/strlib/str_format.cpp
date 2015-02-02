#include <common/strlib.h>
#include <xtl/string.h>

namespace common
{

stl::string vformat (const char* format,va_list list)
{
  if (!format)
    return "";

  va_list list_copy;

  va_copy (list_copy, list);

  int size = xtl::xvsnprintf (0,0,format,list_copy);

  va_end (list_copy);

  if (size == -1)
    return "";

  stl::string str;

  str.fast_resize (size);

  xtl::xvsnprintf (&str [0],size+1,format,list);

  return str;
}

stl::string format (const char* format,...)
{
  va_list list;  

  va_start (list,format);

  return vformat (format,list);
}

}
