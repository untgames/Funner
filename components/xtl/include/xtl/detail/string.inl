/*
    Сравнение строк
*/

inline int xtl_strcmp (const char* s1, const char* s2)
{
  int ret=0;

  while (!(ret=*(unsigned char*)s1 - *(unsigned char*)s2) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

inline int xtl_strncmp (const char* s1, const char* s2, size_t max_count)
{
  int ret=0;

  while (max_count-- && !(ret=*(unsigned char*)s1 - *(unsigned char*)s2) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

inline int xtl_stricmp (const char* s1, const char* s2)
{
  int ret=0;

  while (!(ret=(unsigned char)tolower (*s1) - (unsigned char)tolower (*s2)) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

inline int xtl_strnicmp (const char* s1, const char* s2, size_t max_count)
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

inline int xtl_snprintf (char* buffer, size_t count, const char* format, ...)
{
  va_list list;

  va_start (list, format);

  return xtl_vsnprintf (buffer, count, format, list);
}

inline int xtl_snwprintf (wchar_t* buffer, size_t count, const wchar_t* format, ...)
{
  va_list list;

  va_start (list, format);

  return xtl_vsnwprintf (buffer, count, format, list);
}

#ifdef _MSC_VER

inline int xtl_vsnprintf (char* buffer, size_t count, const char* format, va_list list)
{
  if (!buffer || !count)
    return count ? -1 : ::_vscprintf (format, list);

  int ret = ::_vsnprintf (buffer, count-1, format, list);

  buffer [count-1] = '\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

inline int xtl_vsnwprintf (wchar_t* buffer, size_t count, const wchar_t* format, va_list list)
{
  if (!buffer || !count)
    return count ? -1 : ::_vscwprintf (format, list);

  int ret = ::_vsnwprintf (buffer, count-1, format, list);

  buffer [count-1] = L'\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

#else

inline int xtl_vsnprintf (char* buffer, size_t count, const char* format, va_list list)
{
  if (!buffer || !count)
    return count ? -1 : ::vsnprintf (0, 0, format, list);

  int ret = ::vsnprintf (buffer, count, format, list);

  buffer [count-1] = '\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

/*

inline int xtl_vsnwprintf (wchar_t* buffer, size_t count, const wchar_t* format, va_list list)
{
#ifdef __MINGW32__
  if (!buffer || !count)
    return count ? -1 : vsnwprintf (0, 0, format, list);

  int ret = ::vsnwprintf (buffer, count, format, list);
#else
  if (!buffer || !count)
    return count ? -1 : vswprintf (0, 0, format, list);

  int ret = ::vswprintf (buffer, count, format, list);
#endif

  buffer [count-1] = L'\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}*/

#endif

