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
    Получение длины строки
*/

inline size_t xstrlen (const char* s)
{
  return s ? strlen (s) : 0;
}

inline size_t xstrlen (const wchar_t* s)
{
  if (!s)
    return 0;

  const wchar_t* i = s;

  while (*i++);

  return size_t (i - s - 1);
}

/*
    Копирование строк
*/

inline char* xstrncpy (char* dst, const char* src, int length)
{
  if (!dst || !src)
    return dst;
    
  return strncpy (dst, src, length);  
}

inline wchar_t* xstrncpy (wchar_t* dst, const wchar_t* src, int length)
{
  if (!dst || !src)
    return dst;

  wchar_t* start = dst;

  while (length && (*dst++ = *src++)) length--;

  if (length)
  {
    while (--length) *dst++ = L'\0';
  }

  return start; 
}

/*
    Форматированный вывод в строку
*/

inline int xsnprintf (char* buffer, size_t count, const char* format, ...)
{
  va_list list;

  va_start (list, format);

  return xvsnprintf (buffer, count, format, list);
}

#ifdef _MSC_VER

#ifdef _WIN32_WCE

inline int _try_vscprintf (const char* format, va_list list, size_t size)
{
  char* buffer = (char*)_alloca (size);
  
  return xvsnprintf (buffer, size, format, list);
}

inline int _vscprintf (const char* format, va_list list)
{
  if (!format)
    return 0;
    
  size_t size = 1024;
    
  for (;;)
  {
    int result = _try_vscprintf (format, list, size);
    
    if (result != -1)
      return result;
      
    size *= 2;
  }
}

#endif

inline int xvsnprintf (char* buffer, size_t count, const char* format, va_list list)
{
  if (!buffer || !count)
    return count ? -1 : _vscprintf (format, list);

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
