#include <windows.h>

extern "C"
{

DWORD WINAPI ExpandEnvironmentStrings (LPCTSTR lpSrc, LPTSTR lpDst, DWORD nSize)
{
  if (!nSize)
    return 0;

  lpDst [nSize-1] = 0;

  strncpy (lpDst, lpSrc, nSize);  

  return strlen (lpDst);
} 

}
