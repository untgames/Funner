#include <stdio.h>
#include <wchar.h>
#include <memory>
#include <string>

#include <windows.h>
#include <Subauth.h>

#include <wrl/client.h>

typedef STRING ANSI_STRING, *PANSI_STRING;

std::string tostring (const wchar_t* string, int length)
{
  if (length == -1)
    length = wcslen (string);

  std::string result;

  result.resize (length * 4);

  int result_size = wcsrtombs (&result [0], &string, length, 0);

  if (result_size < 0)
    return "(common::tostring error)";

  result.resize (result_size);

  return result;
}

#pragma comment (linker, "/defaultlib:ntdll.lib")

FORCEINLINE VOID pRtlInitUnicodeString(PUNICODE_STRING destString, PWSTR sourceString)
{
  destString->Buffer = sourceString;
  destString->MaximumLength = destString->Length =
  wcslen(sourceString) * sizeof(WCHAR);
};

FORCEINLINE VOID pRtlInitString(PANSI_STRING destString, PSTR sourceString)
{
  destString->Buffer = sourceString;
  destString->MaximumLength = destString->Length =
  strlen(sourceString) * sizeof(CHAR);
};

extern "C"
{
NTSYSAPI
NTSTATUS
NTAPI
LdrLoadDll (
IN PWCHAR PathToFile OPTIONAL,
// In later versions of windows this is a PULONG
IN ULONG Flags OPTIONAL,
IN PUNICODE_STRING ModuleFileName,
OUT PHANDLE ModuleHandle );
 
NTSYSAPI
NTSTATUS
NTAPI
LdrGetProcedureAddress (
IN HMODULE ModuleHandle,
IN PANSI_STRING FunctionName OPTIONAL,
IN WORD Oridinal OPTIONAL,
OUT PVOID *FunctionAddress );
}

[Platform::MTAThread]
int main(Platform::Array<Platform::String^>^)	
{
  auto localAppDataParh = Windows::Storage::ApplicationData::Current->LocalFolder->Path;

  std::wstring path( localAppDataParh->Data() );
 
  path += L"\\mylog";

  FILE* file = _wfopen (path.c_str (), L"wt");

  if (file)
  {
    fprintf (file, "Hello world!!!!!\n");
  }
  else return 0;

  std::wstring dll_path( localAppDataParh->Data() );

  dll_path += L"\\funner.extern.devil.dll";

  //
  // Example usage of LdrLoadDll
  //
  UNICODE_STRING unicodeStr;
  HANDLE handle = 0;
  NTSTATUS ntstatus = 0;
   
//  pRtlInitUnicodeString ( &unicodeStr, L"ws2_32" );
  pRtlInitUnicodeString ( &unicodeStr, (wchar_t*)dll_path.c_str () );
   
  ntstatus = LdrLoadDll (NULL, 0, &unicodeStr, &handle );
   
  fprintf (file, "%08.08x: %08.08x\n", ntstatus, handle );
   
  //
  // Example usage of LdrGetProcedureAddress
  //
  HMODULE hMod = (HMODULE) handle;
  ANSI_STRING ansiStr;
  DWORD lpfnSend = 0;
   
//  pRtlInitString ( &ansiStr, "send" );
  pRtlInitString ( &ansiStr, "_ilInit@0" );
   
  ntstatus = LdrGetProcedureAddress (
  hMod, &ansiStr, 0, (PVOID *)&lpfnSend );
   
  fprintf (file, "%08.08x: %08.08x\n", ntstatus, lpfnSend);
   
  fclose (file);

  return 0;
}
