/*
    Сравнение строк
*/

inline int xstrcmp (const char* s1, const char* s2)
{
  int ret=0;

  while (!(ret=*(unsigned char*)s1 - *(unsigned char*)s2) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

inline int xstrncmp (const char* s1, const char* s2, size_t max_count)
{
  int ret=0;

  while (max_count-- && !(ret=*(unsigned char*)s1 - *(unsigned char*)s2) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

inline int xstricmp (const char* s1, const char* s2)
{
  int ret=0;

  while (!(ret=(unsigned char)tolower (*s1) - (unsigned char)tolower (*s2)) && *s2) ++s1, ++s2;

  return ret < 0 ? -1 : ret > 0 ? 1 : 0;
}

inline int xstrnicmp (const char* s1, const char* s2, size_t max_count)
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

inline int xsnprintf (char* buffer, size_t count, const char* format, ...)
{
  va_list list;

  va_start (list, format);

  return xvsnprintf (buffer, count, format, list);
}

#ifdef _MSC_VER

inline int xvsnprintf (char* buffer, size_t count, const char* format, va_list list)
{
  if (!buffer || !count)
    return count ? -1 : ::_vscprintf (format, list);

  int ret = ::_vsnprintf (buffer, count-1, format, list);

  buffer [count-1] = '\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

#else

inline int xvsnprintf (char* buffer, size_t count, const char* format, va_list list)
{
  if (!buffer || !count)
    return count ? -1 : ::vsnprintf (0, 0, format, list);

  int ret = ::vsnprintf (buffer, count, format, list);

  buffer [count-1] = '\0';

  return ret < 0 || (size_t)ret >= count ? -1 : ret;
}

#endif
