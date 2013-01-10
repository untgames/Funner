#include <windows.h>

extern "C"
{

HANDLE WINAPI FindFirstFileA (LPCTSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData)
{
  return FindFirstFileEx (lpFileName, FindExInfoStandard, lpFindFileData, FindExSearchNameMatch, 0, 0);
}

}
