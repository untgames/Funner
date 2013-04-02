#include <windows.h>

extern "C"
{

DWORD WINAPI GetModuleFileNameA (HMODULE hModule, LPSTR lpFilename, DWORD nSize)
{
  return 0;
}

DWORD WINAPI GetModuleFileNameW (HMODULE hModule, LPWSTR lpFilename,DWORD nSize)
{
  return 0;
}

}
