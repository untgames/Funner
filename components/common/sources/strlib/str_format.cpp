#include <common/strlib.h>

namespace common
{

stl::string vformat (const char* format,va_list list)
{
  if (!format)
    return "";

  int size = string_wrappers::vsnprintf (0,0,format,list);

  if (size == -1)
    return "";

  stl::string str;

  str.fast_resize (size);

  string_wrappers::vsnprintf (&str [0],size+1,format,list);

  return str;
}

stl::wstring vformat (const wchar_t* format,va_list list)
{
  if (!format)
    return L"";

  int size = string_wrappers::vsnwprintf (0,0,format,list);

  if (size == -1)
    return L"";

  stl::wstring str;

  str.fast_resize (size);

  string_wrappers::vsnwprintf (&str [0],size+1,format,list);

  return str;
}

stl::string format (const char* format,...)
{
  va_list list;  

  va_start (list,format);

  return vformat (format,list);
}

stl::wstring format (const wchar_t* format,...)
{
  va_list list;  

  va_start (list,format);

  return vformat (format,list);
}

}
