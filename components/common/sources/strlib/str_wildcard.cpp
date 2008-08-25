#include <common/strlib.h>

bool common::wcmatch (const char* s,const char* pattern)
{
  while (*s)
    switch (*pattern)
    {
      case '\0':
        return false;
      case '*':
        do pattern++; while (*pattern == '*' || *pattern == '?');
        for (s=strchr (s,*pattern);s && !wcmatch (s,pattern);s=strchr (s+1,*pattern));

        return s != NULL;
      case '?':
        return wcmatch (s,pattern+1) || wcmatch (s+1,pattern+1);
      default:
        if (*pattern++ != *s++)
          return false;

        break;
    }
   
  while (*pattern)
    switch (*pattern++)
    {
      case '*':
      case '?': break;
      default:  return false;     
    }

  return true;
}

inline const char* _strichr (const char* s,char _c)
{
  char c = tolower (_c);
  
  while (*s && tolower (*s) != c) s++;
  
  return tolower (*s) == c ? s : NULL;
}

bool common::wcimatch (const char* s,const char* pattern)
{
  while (*s)
    switch (*pattern)
    {
      case '\0':
        return false;
      case '*':
        do pattern++; while (*pattern == '*' || *pattern == '?');
        for (s=_strichr (s,*pattern);s && !wcimatch (s,pattern);s=_strichr (s+1,*pattern));

        return s != NULL;
      case '?':
        return wcimatch (s,pattern+1) || wcimatch (s+1,pattern+1);
      default:
        if (tolower (*pattern++) != tolower (*s++))
          return false;

        break;
    }
   
  while (*pattern)
    switch (*pattern++)
    {
      case '*':
      case '?': break;
      default:  return false;     
    }

  return true;
}

bool common::is_wildcard (const char* string)
{
  if (strstr (string, "*") || strstr (string, "?"))
    return true;

  return false;
}
