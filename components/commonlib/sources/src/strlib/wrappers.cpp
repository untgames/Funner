#include <common/strlib.h>
#include <wchar.h>

using namespace common;

/*
    Сравнение строк
*/

int common::strcmp (const char* s1,const char* s2)
{
  int ret=0;

  while (!(ret=*(unsigned char*)s1 - *(unsigned char*)s2) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

int common::strncmp (const char* s1,const char* s2,size_t max_count)
{
  int ret=0;

  while (max_count-- && !(ret=*(unsigned char*)s1 - *(unsigned char*)s2) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

int common::stricmp (const char* s1,const char* s2)
{
  int ret=0;

  while (!(ret=(unsigned char)tolower (*s1) - (unsigned char)tolower (*s2)) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

int common::strnicmp (const char* s1,const char* s2,size_t max_count)
{
  int ret = 0;

  while (max_count-- && !(ret=(unsigned char)tolower (*s1) - (unsigned char)tolower (*s2)) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

/*
    Форматированный вывод в строку
*/

/*
    Печать в строку
*/

#ifdef _MSC_VER

int common::vsnprintf (char* buffer,size_t count,const char* format,va_list list)
{
  if (!buffer)
    return count ? -1 : ::_vscprintf (format,list);

  int ret = ::_vsnprintf (buffer,count-1,format,list);

  buffer [count-1] = '\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

int common::vsnwprintf (wchar_t* buffer,size_t count,const wchar_t* format,va_list list)
{
  if (!buffer)
    return count ? -1 : ::_vscwprintf (format,list);

  int ret = ::_vsnwprintf (buffer,count-1,format,list);

  buffer [count-1] = L'\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

stl::string common::vformat (const char* format,va_list list)
{
  if (!format)
    return "";

  int size = ::_vscprintf (format,list);

  if (size == -1)
    return "";

  stl::string str;

  str.fast_resize (size);

  ::_vsnprintf (&str [0],size,format,list);

  return str;
}

stl::wstring common::vformat (const wchar_t* format,va_list list)
{
  if (!format)
    return L"";

  int size = ::_vscwprintf (format,list);

  if (size == -1)
    return L"";

  stl::wstring str;

  str.fast_resize (size);

  ::_vsnwprintf (&str [0],size,format,list);

  return str;
}

#else

int common::vsnprintf (char* buffer,size_t count,const char* format,va_list list)
{
  if (!buffer)
    return count ? -1 : ::vsnprintf (NULL,0,format,list);

  int ret = ::vsnprintf (buffer,count,format,list);

  buffer [count-1] = '\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

int common::vsnwprintf (wchar_t* buffer,size_t count,const wchar_t* format,va_list list)
{
  if (!buffer)
    return count ? -1 : (int)common::vformat (format,list).size ();

#ifdef __MINGW32__
  int ret = ::vsnwprintf (buffer,count,format,list);
#else
  int ret = ::vswprintf (buffer,count,format,list);
#endif

  buffer [count-1] = L'\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

stl::string common::vformat (const char* format,va_list list)
{
  if (!format)
    return "";

  int size = ::vsnprintf (NULL,0,format,list);

  if (size == -1)
    return "";

  stl::string str;

  str.fast_resize (size);

  ::vsnprintf (&str [0],size+1,format,list);

  str [size] = '\0';

  return str;
}

#ifdef __MINGW32__

stl::wstring common::vformat (const wchar_t* format,va_list list)
{
  if (!format)
    return L"";

  int size = ::vsnwprintf (NULL,0,format,list);

  if (size == -1)
    return L"";

  stl::wstring str;

  str.fast_resize (size);

  ::vsnwprintf (&str [0],size+1,format,list);

  str [size] = L'\0';

  return str;
}

#else

stl::wstring common::vformat (const wchar_t* format,va_list list)
{
  if (!format)
    return L"";

  stl::wstring str;        
  size_t       size = str.capacity ();

  for (;;size*=2)
  {
    str.fast_resize (size);

    int ret = ::vsnwprintf (&str [0],str.size ()+1,format,list);

    str [str.size ()] = L'\0';

    if (ret != -1)
    {
      if   ((size_t)ret == str.size ())   return str;
      else if ((size_t)ret < str.size ()) return str.resize (ret), str;
      else                                return L"";

      return L"";
    }

    str.clear ();
  }
}

#endif

#endif

int common::snprintf (char* buffer,size_t count,const char* format,...)
{
  va_list list;

  va_start (list,format);

  return common::vsnprintf (buffer,count,format,list);
}

int common::snwprintf (wchar_t* buffer,size_t count,const wchar_t* format,...)
{
  va_list list;

  va_start (list,format);

  return common::vsnwprintf (buffer,count,format,list);
}

stl::string common::format (const char* format,...)
{
  va_list list;  

  va_start (list,format);

  return vformat (format,list);
}

stl::wstring common::format (const wchar_t* format,...)
{
  va_list list;  

  va_start (list,format);

  return vformat (format,list);
}
