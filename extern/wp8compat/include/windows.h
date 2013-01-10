#ifndef WP8_COMPATIBILITY_WINDOWS_H_HEADER
#define WP8_COMPATIBILITY_WINDOWS_H_HEADER

#include <../../8.0/Include/windows.h>

__inline ULONGLONG GetTickCount () { return GetTickCount64 (); }

__inline HANDLE GetModuleHandle () { return (HANDLE)0; }

__inline HWND GetDesktopWindow () { return (HWND)0; }

#endif
