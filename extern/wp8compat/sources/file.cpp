#include <windows.h>

extern "C"
{

HANDLE WINAPI FindFirstFileA (LPCTSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData)
{
  return FindFirstFileExA (lpFileName, FindExInfoStandard, lpFindFileData, FindExSearchNameMatch, 0, 0);
}

HANDLE WINAPI FindFirstFileW (LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData)
{
  return FindFirstFileExW (lpFileName, FindExInfoStandard, lpFindFileData, FindExSearchNameMatch, 0, 0);
}

BOOL WINAPI MoveFileA (LPCSTR lpExistingFileName, LPCSTR lpNewFileName)
{
  return MoveFileExA (lpExistingFileName, lpNewFileName, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
}

BOOL WINAPI MoveFileW (LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName)
{
  return MoveFileExW (lpExistingFileName, lpNewFileName, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
}

DWORD WINAPI GetFileAttributesA (LPCSTR lpFileName)
{
  WIN32_FILE_ATTRIBUTE_DATA data;

  return GetFileAttributesExA (lpFileName, GetFileExInfoStandard, &data);
}

DWORD WINAPI GetFileAttributesW (LPCWSTR lpFileName)
{
  WIN32_FILE_ATTRIBUTE_DATA data;

  return GetFileAttributesExW (lpFileName, GetFileExInfoStandard, &data);
}

}
