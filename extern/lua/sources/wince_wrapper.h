#ifndef LUA_WINCE_WRAPPER
#define LUA_WINCE_WRAPPER

#ifdef _WIN32_WCE

#include <io.h>
#include <string.h>

inline FILE* tmpfile ()
{
  return 0;
}

inline int system  (const char*)
{
  return 1;
}

inline int remove (const char*)
{
  return 1;
}

inline char* tmpnam (char*)
{
  return 0;
}

inline int strcoll (const char* s1, const char* s2)
{
  return strcmp (s1, s2);
}

#define L_tmpnam (16)

inline FILE* freopen (const char* name, const char* mode, FILE* file)
{
  if (file)
    fclose (file);
    
  return fopen (name, mode);
}

#endif

#endif
