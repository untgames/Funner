#ifndef WP8_COMPATIBILITY_WINDOWS_H_HEADER
#define WP8_COMPATIBILITY_WINDOWS_H_HEADER

#include <../../8.0/Include/windows.h>

#include <minwin/fileapi.h>
#include <minwin/synchapi.h>
#include <minwin/sysinfoapi.h>

#include <signal.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C"
{
#endif

__inline DWORD GetTickCount () { return (DWORD)GetTickCount64 (); }

__inline HANDLE GetModuleHandle () { return INVALID_HANDLE_VALUE; }

__inline HWND GetDesktopWindow () { return (HWND)0; }

__inline int _getpid () { return (int)GetCurrentProcessId (); }

DWORD WINAPI GetModuleFileNameA (HMODULE hModule, LPSTR lpFilename, DWORD nSize);
DWORD WINAPI GetModuleFileNameW (HMODULE hModule, LPWSTR lpFilename,DWORD nSize);

#ifdef UNICODE
#define GetModuleFileName  GetModuleFileNameW
#else
#define GetModuleFileName  GetModuleFileNameA
#endif // !UNICODE

#ifdef getenv
#undef getenv
#endif

__inline char* getenv (const char* name) { return 0; }

HANDLE WINAPI FindFirstFileA (LPCTSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);

HANDLE WINAPI FindFirstFileW (LPCWSTR lpFileName, LPWIN32_FIND_DATAW lpFindFileData);

#ifdef UNICODE
#define FindFirstFile  FindFirstFileW
#else
#define FindFirstFile  FindFirstFileA
#endif // !UNICODE

BOOL WINAPI MoveFileA (LPCSTR lpExistingFileName, LPCSTR lpNewFileName);
BOOL WINAPI MoveFileW (LPCWSTR lpExistingFileName, LPCWSTR lpNewFileName);

#ifdef UNICODE
#define MoveFile  MoveFileW
#else
#define MoveFile  MoveFileA
#endif // !UNICODE

DWORD WINAPI GetFileAttributesA (LPCSTR lpFileName);
DWORD WINAPI GetFileAttributesW (LPCWSTR lpFileName);

#ifdef UNICODE
#define GetFileAttributes  GetFileAttributesW
#else
#define GetFileAttributes  GetFileAttributesA
#endif // !UNICODE

__inline int _getch (void) { return 0; }

__inline HANDLE WINAPI GetStdHandle (DWORD nStdHandle) { return INVALID_HANDLE_VALUE; }

__inline BOOL WINAPI FlushConsoleInputBuffer (HANDLE hConsoleInput) { return TRUE; }

HANDLE WINAPI CreateMutexA (LPSECURITY_ATTRIBUTES lpMutexAttributes, BOOL bInitialOwner, LPCSTR lpName);

#ifdef UNICODE
#define CreateMutex  CreateMutexW
#else
#define CreateMutex  CreateMutexA
#endif // !UNICODE

DWORD WINAPI WaitForSingleObject (HANDLE hHandle, DWORD dwMilliseconds);
DWORD WINAPI WaitForMultipleObjects (DWORD nCount, const HANDLE *lpHandles, BOOL bWaitAll, DWORD dwMilliseconds);

HANDLE WINAPI CreateEventA (LPSECURITY_ATTRIBUTES lpEventAttributes, BOOL bManualReset, BOOL bInitialState, LPCSTR lpName);

BOOL WINAPI PulseEvent (HANDLE hEvent);

#ifdef UNICODE
#define CreateEvent  CreateEventW
#else
#define CreateEvent  CreateEventA
#endif // !UNICODE

#ifndef CREATE_SUSPENDED
#define CREATE_SUSPENDED 0x00000004
#endif

HANDLE WINAPI CreateThread(_In_opt_ LPSECURITY_ATTRIBUTES unusedThreadAttributes, _In_ SIZE_T unusedStackSize, _In_ LPTHREAD_START_ROUTINE lpStartAddress, _In_opt_ LPVOID lpParameter, _In_ DWORD dwCreationFlags, _Out_opt_ LPDWORD unusedThreadId);
DWORD WINAPI ResumeThread(_In_ HANDLE hThread);
BOOL WINAPI SetThreadPriority(_In_ HANDLE hThread, _In_ int nPriority);

VOID WINAPI Sleep(_In_ DWORD dwMilliseconds);

__inline DWORD SleepEx (DWORD dwMilliseconds, BOOL bAlertable) { Sleep (dwMilliseconds); return 0; }

DWORD WINAPI TlsAlloc();
BOOL WINAPI TlsFree(_In_ DWORD dwTlsIndex);
LPVOID WINAPI TlsGetValue(_In_ DWORD dwTlsIndex);
BOOL WINAPI TlsSetValue(_In_ DWORD dwTlsIndex, _In_opt_ LPVOID lpTlsValue);

void WINAPI TlsShutdown();

__inline BOOL WINAPI TerminateProcess (HANDLE hProcess, UINT uExitCode) { return FALSE; }

__inline VOID WINAPI ExitProcess (UINT uExitCode) { exit (uExitCode); }

__inline HMODULE WINAPI LoadLibraryA (LPCTSTR lpFileName) { return (HMODULE)0; }
__inline HMODULE WINAPI LoadLibraryW (LPCWSTR lpFileName) { return (HMODULE)0; }

#ifdef UNICODE
#define LoadLibrary LoadLibraryW
#else
#define LoadLibrary LoadLibraryA
#endif // !UNICODE

__inline UINT WINAPI GetACP (void) { return 65001; } //utf-8

__inline void _tzset (void) {}

DWORD WINAPI ExpandEnvironmentStrings (LPCTSTR lpSrc, LPTSTR lpDst, DWORD nSize);

#ifdef __cplusplus
}
#endif

#endif
