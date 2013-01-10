#ifndef WP8_COMPATIBILITY_WINDOWS_H_HEADER
#define WP8_COMPATIBILITY_WINDOWS_H_HEADER

#include <../../8.0/Include/windows.h>

inline ULONGLONG GetTickCount () { return GetTickCount64 (); }

#endif
