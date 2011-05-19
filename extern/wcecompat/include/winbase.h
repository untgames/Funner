//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
//
// Use of this source code is subject to the terms of the Microsoft end-user
// license agreement (EULA) under which you licensed this SOFTWARE PRODUCT.
// If you did not accept the terms of the EULA, you are not authorized to use
// this source code. For a copy of the EULA, please see the LICENSE.RTF on your
// install media.
//

/*++


Module Name: winbase.h

Purpose:This header file declares the Base APIs.

--*/

#ifndef __WINBASE_H__
#define __WINBASE_H__
#define _WINBASE_

//
// Define API decoration for direct importing of DLL references.
//

#if defined (_M_CEE)
#	if defined(_ADVAPI32_) || defined(COREDLL)
#		pragma message (__FILE__ ": CEF not supported for building COREDLL contents")
#	endif
#endif	// _M_CEE UNDER_CE

#if !defined(_ADVAPI32_)
#define WINADVAPI  DECLSPEC_IMPORT
#else
#define WINADVAPI
#endif

#if !defined(COREDLL)
#define WINBASEAPI DECLSPEC_IMPORT
#else
#define WINBASEAPI
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <windef.h>
#include <winver.h>

#define INVALID_HANDLE_VALUE (HANDLE)-1
#define INVALID_FILE_SIZE (DWORD)0xFFFFFFFF
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)

#define FILE_BEGIN           0
#define FILE_CURRENT         1
#define FILE_END             2
//
//  File System time stamps are represented with the following structure:
//

typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;

typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *LPSYSTEMTIME;

#ifdef MIPS_R4000 /* or above */

#define InterlockedIncrement _InterlockedIncrement
#define InterlockedDecrement _InterlockedDecrement
#define InterlockedExchange _InterlockedExchange

LONG
WINAPI
InterlockedIncrement(
    LPLONG lpAddend
    );

LONG
WINAPI
InterlockedDecrement(
    LPLONG lpAddend
    );

LONG
WINAPI
InterlockedExchange(
    LPLONG Target,
    LONG Value
    );

#pragma intrinsic(_InterlockedIncrement)
#pragma intrinsic(_InterlockedDecrement)
#pragma intrinsic(_InterlockedExchange)

#define InterlockedCompareExchange(ptr, newval, oldval) \
	((PVOID)InterlockedTestExchange((LPLONG)ptr, (LONG)oldval, (LONG)newval))

#elif defined(_X86_)
//
// On x86, the compiler emits the lock prefix for the intrinsic Interlocked* functions so that
// they will be multiprocessor-safe. The overhead of the bus-locking protocol initiated by the
// lock prefix is significant enough that it warrants special treatment in Windows CE since it
// is currently single proc only.
// (For instance using the compiler's intrinsic Interlocked* functions caused a 10% decrease
// in network throughput!)
//

#pragma warning(disable:4035)               // re-enable below

  __inline
  LONG
  WINAPI
  InterlockedIncrement(
      IN PLONG Addend
      )
  {
      __asm {
          mov     eax, 1
          mov     ecx, Addend
          xadd    [ecx], eax
          inc     eax
      }
  }

  __inline
  LONG
  WINAPI
  InterlockedDecrement(
      IN PLONG Addend
      )
  {
      __asm {
          mov     eax, -1
          mov     ecx, Addend
          xadd    [ecx], eax
          dec     eax
      }
  }

  __inline
  LONG
  WINAPI
  InterlockedExchange(
      IN OUT PLONG Target,
      IN LONG Value
      )
  {
      __asm {
          mov     eax, Value
          mov     ecx, Target
          xchg 	[ecx], eax
      }
  }

  __inline
  LONG
  WINAPI
  InterlockedTestExchange(
  	  LPLONG Target,
  	  LONG oldValue,
  	  LONG newValue
  	  )
  {
      __asm {
          mov     eax, oldValue
          mov     ecx, Target
          mov     edx, newValue
          cmpxchg [ecx], edx
      }
  }

  __inline
  LONG
  WINAPI
  InterlockedCompareExchange(
      LPLONG Target,
      LONG Exchange,
      LONG Comperand
      )
  {
      __asm {
          mov     eax, Comperand
          mov     ecx, Target
          mov     edx, Exchange
          cmpxchg [ecx], edx
      }
  }

  __inline
  LONG
  WINAPI
  InterlockedExchangeAdd(
      IN PLONG Addend,
      IN LONG  Value
      )
  {
      __asm {
          mov     ecx, Addend
          mov     eax, Value
          xadd    [ecx], eax
      }
  }

#pragma warning(default:4035)

#define InterlockedTestExchange(Target, oldValue, newValue) \
    InterlockedCompareExchange((Target), (newValue), (oldValue))
    

#else // not _X86_

LONG
WINAPI
InterlockedIncrement(
    LPLONG lpAddend
    );


LONG
WINAPI
InterlockedDecrement(
    LPLONG lpAddend
    );


LONG
WINAPI
InterlockedExchange(
    LPLONG Target,
    LONG Value
    );

LONG
WINAPI
InterlockedCompareExchange(
    IN OUT LPLONG Destination,
    LONG Exchange,
    LONG Comperand
    );

LONG
WINAPI
InterlockedExchangeAdd(
    LPLONG Addend,
    LONG Value
    );


#define InterlockedTestExchange(Target, oldValue, newValue) \
    InterlockedCompareExchange((Target), (newValue), (oldValue))

#endif

#ifndef InterlockedExchangePointer
#define InterlockedExchangePointer(Target, Value) \
    ((PVOID)InterlockedExchange((PLONG)(Target), (LONG)(Value)))
#endif

#ifndef InterlockedCompareExchangePointer
#define InterlockedCompareExchangePointer(Destination, ExChange, Comperand) \
    ((PVOID)InterlockedCompareExchange((PLONG)(Destination), (LONG)(ExChange), (LONG)(Comperand)))
#endif

#define MAXINTATOM 0xC000
#define MAKEINTATOM(i)  (LPTSTR)((DWORD)((WORD)(i)))
#define INVALID_ATOM ((ATOM)0)

DWORD
WINAPI
Random();

int
WINAPI
WinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
#ifdef UNDER_CE
    LPWSTR lpCmdLine,
#else
    LPSTR lpCmdLine,
#endif
    int nShowCmd
    );

LONG WINAPI CompareFileTime(const FILETIME *lpft1, const FILETIME *lpft2);
BOOL WINAPI FileTimeToSystemTime(const FILETIME *lpft, LPSYSTEMTIME lpst);
BOOL WINAPI SystemTimeToFileTime(const SYSTEMTIME *lpst, LPFILETIME lpft);
BOOL WINAPI FileTimeToLocalFileTime(const FILETIME *lpft, LPFILETIME lpftLocal);
BOOL WINAPI LocalFileTimeToFileTime(const FILETIME *lpftLocal, LPFILETIME lpft);

// Internal API for the file system
VOID GetCurrentFT(LPFILETIME lpFileTime);

/* Derived from process.h file from Windows NT */
BOOL
WINAPI
DllMain (
    HANDLE hinstDLL,
    DWORD dwReason,
    LPVOID lpvReserved
    );

#ifdef UNDER_CE
#define GetProcAddress GetProcAddressW

WINBASEAPI
FARPROC
WINAPI
GetProcAddressW(
    __in HMODULE hModule,
    __in LPCWSTR lpProcName
    );

WINBASEAPI
FARPROC
WINAPI
GetProcAddressA(
    __in HMODULE hModule,
    __in LPCSTR lpProcName
    );

#else

WINBASEAPI
FARPROC
WINAPI
GetProcAddress(
    __in HMODULE hModule,
    __in LPCSTR lpProcName
    );

#endif

VOID
WINAPI
GetLocalTime (
    __out LPSYSTEMTIME lpSystemTime
    );

VOID
WINAPI
GetSystemTime (
    __out LPSYSTEMTIME lpSystemTime
    );

typedef struct _TIME_ZONE_INFORMATION {
    LONG Bias;
    WCHAR StandardName[ 32 ];
    SYSTEMTIME StandardDate;
    LONG StandardBias;
    WCHAR DaylightName[ 32 ];
    SYSTEMTIME DaylightDate;
    LONG DaylightBias;
} TIME_ZONE_INFORMATION, *LPTIME_ZONE_INFORMATION;

DWORD
WINAPI
GetTimeZoneInformation (
    __out LPTIME_ZONE_INFORMATION lpTimeZoneInformation
    );

BOOL
WINAPI
SetLocalTime (
    __in CONST SYSTEMTIME *lpSystemTime
    );

BOOL
WINAPI
SetSystemTime (
    __in CONST SYSTEMTIME *lpSystemTime
    );

void
WINAPI
SetDaylightTime (
    __in DWORD dst
    );

BOOL
WINAPI
SetTimeZoneInformation (
    __in CONST TIME_ZONE_INFORMATION *lpTimeZoneInformation
    );

/*
	@doc BOTH EXTERNAL
	
	@func LPTSTR | lstrcat| Appends one string to another.
    @parm LPTSTR | lpszString1 | address of buffer for concatenated strings
    @parm LPTSTR | lpszString2 | address of string to add to string1

	@comm Follows the Win32 reference description with these restrictions:
	@comm Supports only the Unicode version of this function.

*/
WINBASEAPI
LPSTR
WINAPI
lstrcatA(
    __inout LPSTR lpString1,
    __in LPCSTR lpString2
    );

#define lstrcatW wcscat
#ifdef UNICODE
#define lstrcat lstrcatW
#else
#define lstrcat lstrcatA
#endif

/*
	@doc BOTH EXTERNAL
	
	@func int | lstrcmp| Compares two character strings. The comparison is case sensitive.
    @parm LPTSTR | lpszString1 | address of first string
    @parm LPTSTR | lpszString2 | address of second string

	@comm Follows the Win32 reference description with these restrictions:
	@comm Supports only the Unicode version of this function.

*/

WINBASEAPI
int
WINAPI
lstrcmpA(
    __in LPCSTR lpString1,
    __in LPCSTR lpString2
    );
WINBASEAPI
int
WINAPI
lstrcmpW(
    __in LPCWSTR lpString1,
    __in LPCWSTR lpString2
    );
#ifdef UNICODE
#define lstrcmp  lstrcmpW
#else
#define lstrcmp  lstrcmpA
#endif // !UNICODE

/*
	@doc BOTH EXTERNAL
	
	@func int | lstrcmpi| Compares two character strings. The comparison is
	not case sensitive.
    @parm LPTSTR | lpszString1 | address of first string
    @parm LPTSTR | lpszString2 | address of second string

	@comm Follows the Win32 reference description with these restrictions:
	@comm Supports only the Unicode version of this function.

*/

WINBASEAPI
int
WINAPI
lstrcmpiA(
    __in LPCSTR lpString1,
    LPCSTR lpString2
    );
WINBASEAPI
int
WINAPI
lstrcmpiW(
    __in LPCWSTR lpString1,
    __in LPCWSTR lpString2
    );
#ifdef UNICODE
#define lstrcmpi  lstrcmpiW
#else
#define lstrcmpi  lstrcmpiA
#endif // !UNICODE

/*
	@doc BOTH EXTERNAL
	
	@func LPTSTR | lstrcpy| Copies a string to a buffer.
    @parm LPTSTR | lpszString1 | address of buffer
    @parm LPTSTR | lpszString2 | address of string to copy

	@comm Follows the Win32 reference description with these restrictions:
	@comm Supports only the Unicode version of this function.

*/
WINBASEAPI
LPSTR
WINAPI
lstrcpyA(
    __out LPSTR lpString1,
    __in LPCSTR lpString2
    );
#define lstrcpyW wcscpy
#ifdef UNICODE
#define lstrcpy lstrcpyW
#else
#define lstrcpy lstrcpyA
#endif

/*
	@doc BOTH EXTERNAL
	
	@func int | lstrlen| The lstrlen function returns the length, in characters, of
	the specified string (not including the terminating null character).
    @parm LPTSTR | lpszString | address of string to count

	@comm Follows the Win32 reference description with these restrictions:
	@comm Supports only the Unicode version of this function.

*/
WINBASEAPI
int
WINAPI
lstrlenA(
    __in LPCSTR lpString
    );
#define lstrlenW wcslen
#ifdef UNICODE
#define lstrlen lstrlenW
#else
#define lstrlen lstrlenA
#endif

DWORD
WINAPI
CharLowerBuffA (
    __inout_ecount(cchLength) LPSTR lpsz,
    __in DWORD cchLength
    );
DWORD
WINAPI
CharLowerBuffW (
    __inout_ecount(cchLength) LPWSTR lpsz,
    __in DWORD cchLength
    );
#ifdef UNICODE
#define CharLowerBuff CharLowerBuffW
#else
#define CharLowerBuff CharLowerBuffA
#endif

DWORD
WINAPI
CharUpperBuffA (
    __inout_ecount(cchLength) LPSTR lpsz,
    __in DWORD cchLength
    );
DWORD
WINAPI
CharUpperBuffW (
    __inout_ecount(cchLength) LPWSTR lpsz,
    __in DWORD cchLength
    );
#ifdef UNICODE
#define CharUpperBuff CharUpperBuffW
#else
#define CharUpperBuff CharUpperBuffA
#endif

LPSTR WINAPI CharLowerA(__inout LPSTR lpsz);
LPWSTR WINAPI CharLowerW(__inout LPWSTR lpsz);

#ifdef UNICODE
#define CharLower CharLowerW
#else
#define CharLower CharLowerA
#endif

LPSTR WINAPI CharUpperA(__inout LPSTR lpsz);
LPWSTR WINAPI CharUpperW(__inout LPWSTR lpsz);

#ifdef UNICODE
#define CharUpper CharUpperW
#else
#define CharUpper CharUpperA
#endif

LPSTR
WINAPI
CharPrevA(
    __in LPCSTR lpszStart,
    __in LPCSTR lpszCurrent);

LPWSTR
WINAPI
CharPrevW(
    __in LPCWSTR lpszStart,
    __in LPCWSTR lpszCurrent);

#ifdef UNICODE
#define CharPrev  CharPrevW
#else
#define CharPrev  CharPrevA
#endif // !UNICODE

LPSTR
WINAPI
CharNextA(
    LPCSTR __in lpsz);

LPWSTR
WINAPI
CharNextW(
    LPCWSTR __in lpsz);

#ifdef UNICODE
#define CharNext  CharNextW
#else
#define CharNext  CharNextA
#endif // !UNICODE



/*
	@doc BOTH EXTERNAL
	
	@func BOOL | IsCharAlpha| Determines whether a character is an alphabetic character.
    @parm TCHAR | ch | character to test

	@comm Follows the Win32 reference description without restrictions or modifications.
*/

#define IsCharAlphaA isalpha
#define IsCharAlphaW iswalpha
#ifdef UNICODE
#define IsCharAlpha IsCharAlphaW
#else
#define IsCharAlpha IsCharAlphaA
#endif


/*
	@doc BOTH EXTERNAL
	
	@func BOOL | IsCharAlphaNumericW| Determines whether a character is either an
	alphabetic or a numeric character.

	@comm Follows the Win32 reference description without restrictions or modifications.
*/

#define IsCharAlphaNumericA isalnum
#define IsCharAlphaNumericW iswalnum
#ifdef UNICODE
#define IsCharAlphaNumeric IsCharAlphaNumericW
#else
#define IsCharAlphaNumeric IsCharAlphaNumericA
#endif

/*
	@doc BOTH EXTERNAL
	
	@func BOOL | IsCharLower | Determines whether a character is lowercase.
    @parm TCHAR | ch | character to test

	@comm Follows the Win32 reference description with these restrictions:
	@comm Supports only the Unicode version of this function.
*/

#define IsCharLowerA islower
#define IsCharLowerW iswlower
#ifdef UNICODE
#define IsCharLower IsCharLowerW
#else
#define IsCharLower IsCharLowerA
#endif

/*
	@doc BOTH EXTERNAL
	
	@func BOOL | IsCharUpper | Determines whether a character is uppercase.
    @parm TCHAR | ch | character to test

	@comm Follows the Win32 reference description with these restrictions:
	@comm Supports only the Unicode version of this function.
*/

#define IsCharUpperA isupper
#define IsCharUpperW iswupper
#ifdef UNICODE
#define IsCharUpper IsCharUpperW
#else
#define IsCharUpper IsCharUpperA
#endif

#include <stdarg.h>

int
WINAPIV
wsprintfA (
    __out LPSTR,
    __in __format_string LPCSTR,
    ...
    );
int
WINAPIV
wsprintfW (
    __out LPWSTR,
    __in __format_string LPCWSTR,
    ...
    );

#ifdef UNICODE
#define wsprintf wsprintfW
#else
#define wsprintf wsprintfA
#endif

int
WINAPI
wvsprintfA (
    __out LPSTR,
    __in __format_string LPCSTR,
    __in va_list arglist
    );

int
WINAPI
wvsprintfW (
    __out LPWSTR,
    __in __format_string LPCWSTR,
    __in va_list arglist
    );

#ifdef UNICODE
#define wvsprintf wvsprintfW
#else
#define wvsprintf wvsprintfA
#endif

// @CESYSGEN IF COREDLL_LMEM

#define LMEM_FIXED          0x0000
#define LMEM_MOVEABLE       0x0002
#define LMEM_NOCOMPACT      0x0010       /**** Used for Moveable Memory  ***/
#define LMEM_NODISCARD      0x0020       /**** Ignored *****/
#define LMEM_ZEROINIT       0x0040
#define LMEM_MODIFY         0x0080       /*** Used only in LocalReAlloc() **/
#define LMEM_DISCARDABLE    0x0F00       /**** Ignored ****/
#define LMEM_VALID_FLAGS    0x0F72
#define LMEM_INVALID_HANDLE 0x8000
#define LMEM_DDESHARE		0x0000		 /**** Ignored ****/
#define LMEM_LOWER			0x0000		 /**** Ignored ****/
#define LMEM_NOT_BANKED		0x0000		 /**** Ignored ****/
#define LMEM_NOTIFY			0x0000		 /**** Ignored ****/
#define LMEM_SHARE			0x0000		 /**** Ignored ****/

#define LHND                (LMEM_MOVEABLE | LMEM_ZEROINIT)
#define LPTR                (LMEM_FIXED | LMEM_ZEROINIT)

#define NONZEROLHND         (LMEM_MOVEABLE)
#define NONZEROLPTR         (LMEM_FIXED)

/* Flags returned by LocalFlags (in addition to LMEM_DISCARDABLE) */
#define LMEM_DISCARDED      0x4000
#define LMEM_LOCKCOUNT      0x00FF

#ifdef UNDER_CE
#define GMEM_FIXED          LMEM_FIXED
#define GMEM_MOVEABLE       LMEM_MOVEABLE
#define GPTR                LPTR
#define GHND                LHND
#define GMEM_DDESHARE       LMEM_DDESHARE
#define GMEM_DISCARDABLE    LMEM_DISCARDABLE
#define GMEM_LOWER          LMEM_LOWER
#define GMEM_NOCOMPACT      LMEM_NOCOMPACT
#define GMEM_NODISCARD      LMEM_NODISCARD
#define GMEM_NOT_BANKED     LMEM_NOT_BANKED
#define GMEM_NOTIFY         LMEM_NOTIFY
#define GMEM_SHARE          LMEM_SHARE
#define GMEM_ZEROINIT       LMEM_ZEROINIT
#endif

// @CESYSGEN ENDIF

typedef struct _MEMORYSTATUS {
    DWORD dwLength;
    DWORD dwMemoryLoad;
    DWORD dwTotalPhys;
    DWORD dwAvailPhys;
    DWORD dwTotalPageFile;
    DWORD dwAvailPageFile;
    DWORD dwTotalVirtual;
    DWORD dwAvailVirtual;
} MEMORYSTATUS, *LPMEMORYSTATUS;

VOID
WINAPI
GlobalMemoryStatus(
    __inout LPMEMORYSTATUS lpBuffer
    );

// @CESYSGEN IF COREDLL_LMEM

#ifdef UNDER_CE
#define LocalLock(X) ((LPVOID)(X))
#define LocalUnlock(X) (0)
#define LocalHandle(X) ((HLOCAL)(X))
#define LocalFlags(X) (0)
#else
WINBASEAPI
LPVOID
WINAPI
LocalLock(
    __in HLOCAL hMem
    );

WINBASEAPI
BOOL
WINAPI
LocalUnlock(
    __in HLOCAL hMem
    );

WINBASEAPI
HLOCAL
WINAPI
LocalHandle(
    __in LPCVOID pMem
    );

WINBASEAPI
UINT
WINAPI
LocalFlags(
    __in HLOCAL hMem
    );
#endif

HLOCAL
WINAPI
LocalAlloc (
    UINT fuFlags,
    UINT cbBytes
    );
HLOCAL
WINAPI
LocalAllocTrace (
    UINT fuFlags,
    UINT cbBytes,
    UINT cLineNum,
    PCHAR szFilename
    );

#if defined(DEBUG) || defined(LMEM_DEBUG)
#define LocalAlloc(f,b) LocalAllocTrace(f,b,__LINE__, __FILE__)
#endif

HLOCAL
WINAPI
LocalFree (
    HLOCAL hMem
    );

HLOCAL
WINAPI
LocalReAlloc (
    HLOCAL hMem,
    UINT cbBytes,
    UINT fuFlags
    );

UINT
WINAPI
LocalSize (
    HLOCAL hMem
    );

#ifdef UNDER_CE
#define GlobalAlloc(flags, cb)				LocalAlloc(flags, cb)
#define GlobalFree(handle)					LocalFree(handle)
#define GlobalReAlloc(handle, cb, flags)	LocalReAlloc(handle, cb, LMEM_MOVEABLE)
#define GlobalLock(lp)						LocalLock(lp)
#define GlobalHandle(lp)					LocalHandle(lp)
#define GlobalUnlock(hMem)					LocalUnlock(hMem)
#define GlobalSize(hMem)					LocalSize(hMem)
#define GlobalFlags(X)						LocalFlags(X)
#define LocalPtrHandle(lp)					((HLOCAL)LocalHandle(lp))
#define LocalLockPtr(lp)					((BOOL)LocalLock(LocalPtrHandle(lp)))
#define LocalUnlockPtr(lp)					LocalUnlock(LocalPtrHandle(lp))
#define LocalFreePtr(lp)					(LocalUnlockPtr(lp), (BOOL)LocalFree(LocalPtrHandle(lp)))
#endif

// @CESYSGEN ENDIF

LPVOID
WINAPI
VirtualAlloc(
    LPVOID lpAddress,
    DWORD dwSize,
    DWORD flAllocationType,
    DWORD flProtect
    );

BOOL
WINAPI
VirtualFree(
    LPVOID lpAddress,
    DWORD dwSize,
    DWORD dwFreeType
    );

BOOL
WINAPI
VirtualProtect(
    LPVOID lpAddress,
    DWORD dwSize,
    DWORD flNewProtect,
    PDWORD lpflOldProtect
    );

DWORD
WINAPI
VirtualQuery(
    LPCVOID lpAddress,
    PMEMORY_BASIC_INFORMATION lpBuffer,
    DWORD dwLength
    );

// @CESYSGEN IF COREDLL_LMEM

HANDLE WINAPI GetProcessHeap(VOID);

HANDLE
WINAPI
HeapCreate(
    DWORD flOptions,
    DWORD dwInitialSize,
    DWORD dwMaximumSize
    );

BOOL
WINAPI
HeapDestroy(
    HANDLE hHeap
    );

LPVOID
WINAPI
HeapAlloc(
    HANDLE hHeap,
    DWORD dwFlags,
    DWORD dwBytes
    );

LPVOID
WINAPI
HeapAllocTrace(
    HANDLE hHeap,
    DWORD dwFlags,
    DWORD dwBytes,
    DWORD dwLineNum,
    PCHAR szFilename
    );

#if defined(DEBUG) || defined(LMEM_DEBUG)
#define HeapAlloc(h,f,b) HeapAllocTrace(h,f,b,__LINE__, __FILE__)
#endif

LPVOID
WINAPI
HeapReAlloc(
    HANDLE hHeap,
    DWORD dwFlags,
    LPVOID lpMem,
    DWORD dwBytes
    );

BOOL
WINAPI
HeapFree(
    HANDLE hHeap,
    DWORD dwFlags,
    LPVOID lpMem
    );

DWORD
WINAPI
HeapSize(
    HANDLE hHeap,
    DWORD dwFlags,
    LPCVOID lpMem
    );

BOOL
WINAPI
HeapValidate(
    HANDLE hHeap,
    DWORD dwFlags,
    LPCVOID lpMem
    );

UINT 
WINAPI 
HeapCompact(
    HANDLE hHeap,
    DWORD dwFlags
    );

//
// CE Heap extension: Create Heap with custom allocator/deallocator
//
typedef LPVOID (* PFN_AllocHeapMem) (LPVOID pAddr, DWORD cbSize, DWORD fdwAction, LPDWORD pdwUserData);
typedef BOOL (* PFN_FreeHeapMem) (LPVOID pAddr, DWORD cbSize, DWORD fdwAction, DWORD dwUserData);

HANDLE
WINAPI
CeHeapCreate(
    DWORD flOptions,
    DWORD dwInitialSize,
    DWORD dwMaximumSize,
    PFN_AllocHeapMem pfnAlloc,
    PFN_FreeHeapMem pfnFree
    );


// @CESYSGEN ENDIF

VOID
WINAPI
OutputDebugStringA(
    LPCSTR lpOutputString
    );

VOID
WINAPI
OutputDebugStringW(
    LPCWSTR lpOutputString
    );
#ifdef UNICODE
#define OutputDebugString  OutputDebugStringW
#else
#define OutputDebugString  OutputDebugStringA
#endif // !UNICODE

#if defined (_M_CEE) || !defined (UNDER_CE)
WINBASEAPI
VOID
WINAPI
DebugBreak(
    VOID
    );
#endif

VOID SignalStarted(DWORD dw);

BOOL
WINAPI
QueryPerformanceCounter(
    LARGE_INTEGER *lpPerformanceCount
    );

BOOL
WINAPI
QueryPerformanceFrequency(
    LARGE_INTEGER *lpFrequency
    );

#define VER_PLATFORM_WIN32s             0
#define VER_PLATFORM_WIN32_WINDOWS      1
#define VER_PLATFORM_WIN32_NT           2
#define VER_PLATFORM_WIN32_HH           3
#define VER_PLATFORM_WIN32_CE           3

typedef struct _OSVERSIONINFOA {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    CHAR   szCSDVersion[ 128 ];       // Maintenance string for PSS usage
} OSVERSIONINFOA, *POSVERSIONINFOA, *LPOSVERSIONINFOA;
typedef struct _OSVERSIONINFOW {
    DWORD dwOSVersionInfoSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformId;
    WCHAR  szCSDVersion[ 128 ];       // Maintenance string for PSS usage
} OSVERSIONINFOW, *POSVERSIONINFOW, *LPOSVERSIONINFOW;
#ifdef UNICODE
typedef OSVERSIONINFOW OSVERSIONINFO;
typedef POSVERSIONINFOW POSVERSIONINFO;
typedef LPOSVERSIONINFOW LPOSVERSIONINFO;
#else
typedef OSVERSIONINFOA OSVERSIONINFO;
typedef POSVERSIONINFOA POSVERSIONINFO;
typedef LPOSVERSIONINFOA LPOSVERSIONINFO;
#endif // UNICODE

BOOL
WINAPI
GetVersionExA(
    LPOSVERSIONINFOA lpVersionInformation
    );
BOOL
WINAPI
GetVersionExW(
    LPOSVERSIONINFOW lpVersionInformation
    );
#ifdef UNICODE
#define GetVersionEx  GetVersionExW
#else
#define GetVersionEx  GetVersionExA
#endif // !UNICODE

typedef struct _SYSTEM_INFO {
    union {
        DWORD dwOemId;          // Obsolete field...do not use
        struct {
            WORD wProcessorArchitecture;
            WORD wReserved;
        };
    };
    DWORD dwPageSize;
    LPVOID lpMinimumApplicationAddress;
    LPVOID lpMaximumApplicationAddress;
    DWORD dwActiveProcessorMask;
    DWORD dwNumberOfProcessors;
    DWORD dwProcessorType;
    DWORD dwAllocationGranularity;
    WORD wProcessorLevel;
    WORD wProcessorRevision;
} SYSTEM_INFO, *LPSYSTEM_INFO;

HINSTANCE
WINAPI
LoadLibraryA(
    LPCSTR lpLibFileName
    );

HINSTANCE
WINAPI
LoadLibraryW(
    LPCWSTR lpLibFileName
    );
#ifdef UNICODE
#define LoadLibrary  LoadLibraryW
#else
#define LoadLibrary  LoadLibraryA
#endif // !UNICODE

WINBASEAPI
HMODULE
WINAPI
LoadLibraryExA(
    IN LPCSTR lpLibFileName,
    IN HANDLE hFile,
    IN DWORD dwFlags
    );
WINBASEAPI
HMODULE
WINAPI
LoadLibraryExW(
    IN LPCWSTR lpLibFileName,
    IN HANDLE hFile,
    IN DWORD dwFlags
    );
#ifdef UNICODE
#define LoadLibraryEx  LoadLibraryExW
#else
#define LoadLibraryEx  LoadLibraryExA
#endif // !UNICODE

#define DONT_RESOLVE_DLL_REFERENCES     0x00000001
#define LOAD_LIBRARY_AS_DATAFILE        0x00000002
#define LOAD_WITH_ALTERED_SEARCH_PATH   0x00000008

WINBASEAPI
DWORD
WINAPI
GetModuleFileNameA(
    HMODULE hModule,
    LPSTR lpFilename,
    DWORD nSize
    );
WINBASEAPI
DWORD
WINAPI
GetModuleFileNameW(
    HMODULE hModule,
    LPWSTR lpFilename,
    DWORD nSize
    );
#ifdef UNICODE
#define GetModuleFileName  GetModuleFileNameW
#else
#define GetModuleFileName  GetModuleFileNameA
#endif // !UNICODE

WINBASEAPI
HMODULE
WINAPI
GetModuleHandleA(
    LPCSTR lpModuleName
    );
WINBASEAPI
HMODULE
WINAPI
GetModuleHandleW(
    LPCWSTR lpModuleName
    );
#ifdef UNICODE
#define GetModuleHandle  GetModuleHandleW
#else
#define GetModuleHandle  GetModuleHandleA
#endif // !UNICODE

//
//  File structures
//
/////////////////////////////////////////////////////////////////////////////////////////

#define MSGQUEUE_NOPRECOMMIT            0x00000001
#define MSGQUEUE_ALLOW_BROKEN           0x00000002

#define MSGQUEUE_MSGALERT               0x00000001



typedef struct MSGQUEUEOPTIONS_OS {
    DWORD dwSize;                           // size of the structure
    DWORD dwFlags;                          // behavior of message queue
    DWORD dwMaxMessages;                    // max # of msgs in queue
    DWORD cbMaxMessage;                     // max size of msg
    BOOL  bReadAccess;                      // read access requested
} MSGQUEUEOPTIONS, FAR *LPMSGQUEUEOPTIONS, *PMSGQUEUEOPTIONS;

typedef struct MSGQUEUEINFO {
    DWORD dwSize;                           // size of structure
    DWORD dwFlags;                          // behavior of message queue
    DWORD dwMaxMessages;                    // max # of msgs in queue
    DWORD cbMaxMessage;                     // max size of msg
    DWORD dwCurrentMessages;                // # of message in queue currently
    DWORD dwMaxQueueMessages;               // high water mark of queue
    WORD  wNumReaders;                      // # of readers
    WORD  wNumWriters;                      // # of writes
} MSGQUEUEINFO, *PMSGQUEUEINFO, FAR *LPMSGQUEUEINFO;

typedef struct _OVERLAPPED {
    DWORD   Internal;
    DWORD   InternalHigh;
    DWORD   Offset;
    DWORD   OffsetHigh;
    HANDLE  hEvent;
} OVERLAPPED, *LPOVERLAPPED;

typedef struct _SECURITY_ATTRIBUTES {
    DWORD nLength;
    LPVOID lpSecurityDescriptor;
    BOOL bInheritHandle;
} SECURITY_ATTRIBUTES, *PSECURITY_ATTRIBUTES, *LPSECURITY_ATTRIBUTES;

typedef DWORD (WINAPI *LPTHREAD_START_ROUTINE)(LPVOID pvarg);

typedef LPTHREAD_START_ROUTINE PTHREAD_START_ROUTINE;

#define DEBUG_PROCESS                       0x00000001
#define DEBUG_ONLY_THIS_PROCESS             0x00000002
#define CREATE_SUSPENDED                    0x00000004
#define CREATE_NEW_CONSOLE                  0x00000010
#define STACK_SIZE_PARAM_IS_A_RESERVATION 	0x00010000
#define INHERIT_CALLER_PRIORITY             0x00020000


#define THREAD_PRIORITY_TIME_CRITICAL   0
#define THREAD_PRIORITY_HIGHEST         1
#define THREAD_PRIORITY_ABOVE_NORMAL    2
#define THREAD_PRIORITY_NORMAL          3
#define THREAD_PRIORITY_BELOW_NORMAL    4
#define THREAD_PRIORITY_LOWEST          5
#define THREAD_PRIORITY_ABOVE_IDLE      6
#define THREAD_PRIORITY_IDLE            7

#define THREAD_PRIORITY_ERROR_RETURN    (MAXLONG)

#define EXCEPTION_DEBUG_EVENT       1
#define CREATE_THREAD_DEBUG_EVENT   2
#define CREATE_PROCESS_DEBUG_EVENT  3
#define EXIT_THREAD_DEBUG_EVENT     4
#define EXIT_PROCESS_DEBUG_EVENT    5
#define LOAD_DLL_DEBUG_EVENT        6
#define UNLOAD_DLL_DEBUG_EVENT      7
#define OUTPUT_DEBUG_STRING_EVENT   8
#define RIP_EVENT                   9

typedef struct _EXCEPTION_DEBUG_INFO {
    EXCEPTION_RECORD ExceptionRecord;
    DWORD dwFirstChance;
} EXCEPTION_DEBUG_INFO, *LPEXCEPTION_DEBUG_INFO;

typedef struct _CREATE_THREAD_DEBUG_INFO {
    HANDLE hThread;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
} CREATE_THREAD_DEBUG_INFO, *LPCREATE_THREAD_DEBUG_INFO;

typedef struct _CREATE_PROCESS_DEBUG_INFO {
    HANDLE hFile;
    HANDLE hProcess;
    HANDLE hThread;
    LPVOID lpBaseOfImage;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpThreadLocalBase;
    LPTHREAD_START_ROUTINE lpStartAddress;
    LPVOID lpImageName;
    WORD fUnicode;
} CREATE_PROCESS_DEBUG_INFO, *LPCREATE_PROCESS_DEBUG_INFO;

typedef struct _EXIT_THREAD_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_THREAD_DEBUG_INFO, *LPEXIT_THREAD_DEBUG_INFO;

typedef struct _EXIT_PROCESS_DEBUG_INFO {
    DWORD dwExitCode;
} EXIT_PROCESS_DEBUG_INFO, *LPEXIT_PROCESS_DEBUG_INFO;

typedef struct _LOAD_DLL_DEBUG_INFO {
    HANDLE hFile;
    LPVOID lpBaseOfDll;
    DWORD dwDebugInfoFileOffset;
    DWORD nDebugInfoSize;
    LPVOID lpImageName;
    WORD fUnicode;
} LOAD_DLL_DEBUG_INFO, *LPLOAD_DLL_DEBUG_INFO;

typedef struct _UNLOAD_DLL_DEBUG_INFO {
    LPVOID lpBaseOfDll;
} UNLOAD_DLL_DEBUG_INFO, *LPUNLOAD_DLL_DEBUG_INFO;

typedef struct _OUTPUT_DEBUG_STRING_INFO {
    LPSTR lpDebugStringData;
    WORD fUnicode;
    WORD nDebugStringLength;
} OUTPUT_DEBUG_STRING_INFO, *LPOUTPUT_DEBUG_STRING_INFO;

typedef struct _RIP_INFO {
    DWORD dwError;
    DWORD dwType;
} RIP_INFO, *LPRIP_INFO;

typedef struct _DEBUG_EVENT {
    DWORD dwDebugEventCode;
    DWORD dwProcessId;
    DWORD dwThreadId;
    union {
        EXCEPTION_DEBUG_INFO Exception;
        CREATE_THREAD_DEBUG_INFO CreateThread;
        CREATE_PROCESS_DEBUG_INFO CreateProcessInfo;
        EXIT_THREAD_DEBUG_INFO ExitThread;
        EXIT_PROCESS_DEBUG_INFO ExitProcess;
        LOAD_DLL_DEBUG_INFO LoadDll;
        UNLOAD_DLL_DEBUG_INFO UnloadDll;
        OUTPUT_DEBUG_STRING_INFO DebugString;
        RIP_INFO RipInfo;
    } u;
} DEBUG_EVENT, *LPDEBUG_EVENT;
#define WAIT_OBJECT_0   	0x00000000L
#define WAIT_ABANDONED  	0x00000080L
#define WAIT_ABANDONED_0	0x00000080L
#define WAIT_FAILED         0xffffffffL
#define INFINITE            0xffffffffL

#define EXCEPTION_ACCESS_VIOLATION          STATUS_ACCESS_VIOLATION
#define EXCEPTION_DATATYPE_MISALIGNMENT     STATUS_DATATYPE_MISALIGNMENT
#define EXCEPTION_BREAKPOINT                STATUS_BREAKPOINT
#define EXCEPTION_SINGLE_STEP               STATUS_SINGLE_STEP
#define EXCEPTION_ARRAY_BOUNDS_EXCEEDED     STATUS_ARRAY_BOUNDS_EXCEEDED
#define EXCEPTION_FLT_DENORMAL_OPERAND      STATUS_FLOAT_DENORMAL_OPERAND
#define EXCEPTION_FLT_DIVIDE_BY_ZERO        STATUS_FLOAT_DIVIDE_BY_ZERO
#define EXCEPTION_FLT_INEXACT_RESULT        STATUS_FLOAT_INEXACT_RESULT
#define EXCEPTION_FLT_INVALID_OPERATION     STATUS_FLOAT_INVALID_OPERATION
#define EXCEPTION_FLT_OVERFLOW              STATUS_FLOAT_OVERFLOW
#define EXCEPTION_FLT_STACK_CHECK           STATUS_FLOAT_STACK_CHECK
#define EXCEPTION_FLT_UNDERFLOW             STATUS_FLOAT_UNDERFLOW
#define EXCEPTION_INT_DIVIDE_BY_ZERO        STATUS_INTEGER_DIVIDE_BY_ZERO
#define EXCEPTION_INT_OVERFLOW              STATUS_INTEGER_OVERFLOW
#define EXCEPTION_PRIV_INSTRUCTION          STATUS_PRIVILEGED_INSTRUCTION
#define EXCEPTION_IN_PAGE_ERROR             STATUS_IN_PAGE_ERROR
#define EXCEPTION_ILLEGAL_INSTRUCTION       STATUS_ILLEGAL_INSTRUCTION
#define EXCEPTION_NONCONTINUABLE_EXCEPTION  STATUS_NONCONTINUABLE_EXCEPTION
#define EXCEPTION_STACK_OVERFLOW            STATUS_STACK_OVERFLOW
#define EXCEPTION_INVALID_DISPOSITION       STATUS_INVALID_DISPOSITION
#define EXCEPTION_GUARD_PAGE                STATUS_GUARD_PAGE_VIOLATION
#define EXCEPTION_INVALID_HANDLE            STATUS_INVALID_HANDLE

typedef PCONTEXT LPCONTEXT;

#ifdef UNDER_NT
typedef struct _RTL_CRITICAL_SECTION_DEBUG {
    WORD   Type;
    WORD   CreatorBackTraceIndex;
    struct _RTL_CRITICAL_SECTION *CriticalSection;
    LIST_ENTRY ProcessLocksList;
    DWORD EntryCount;
    DWORD ContentionCount;
    DWORD Spare[ 2 ];
} RTL_CRITICAL_SECTION_DEBUG, *PRTL_CRITICAL_SECTION_DEBUG;

typedef struct _RTL_CRITICAL_SECTION {
    PRTL_CRITICAL_SECTION_DEBUG DebugInfo;

    //
    //  The following three fields control entering and exiting the critical
    //  section for the resource
    //

    LONG LockCount;
    LONG RecursionCount;
    HANDLE OwningThread;        // from the thread's ClientId->UniqueThread
    HANDLE LockSemaphore;
    DWORD Reserved;
} RTL_CRITICAL_SECTION, *PRTL_CRITICAL_SECTION;

typedef RTL_CRITICAL_SECTION CRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION PCRITICAL_SECTION;
typedef PRTL_CRITICAL_SECTION LPCRITICAL_SECTION;

#else

typedef struct CRITICAL_SECTION {
    unsigned int LockCount;         /* Nesting count on critical section */
    HANDLE OwnerThread;         	/* Handle of owner thread */
    HANDLE hCrit;					/* Handle to this critical section */
    DWORD needtrap;					/* Trap in when freeing critical section */
    DWORD dwContentions;			/* Count of contentions */
} CRITICAL_SECTION, *LPCRITICAL_SECTION;
#endif

VOID
WINAPI
EnterCriticalSection (
    LPCRITICAL_SECTION pcsCriticalSection
    );

VOID
WINAPI
LeaveCriticalSection (
    LPCRITICAL_SECTION pcsCriticalSection
    );

VOID
WINAPI
InitializeCriticalSection (
    LPCRITICAL_SECTION pcsCriticalSection
    );

WINBASEAPI
BOOL
WINAPI
TryEnterCriticalSection(
    IN OUT LPCRITICAL_SECTION lpCriticalSection
    );
    
VOID
WINAPI
DeleteCriticalSection (
    LPCRITICAL_SECTION pcsCriticalSection
    );

BOOL
WINAPI
WaitForDebugEvent(
    LPDEBUG_EVENT lpDebugEvent,
    DWORD dwMilliseconds
    );

BOOL
WINAPI
ContinueDebugEvent(
    DWORD dwProcessId,
    DWORD dwThreadId,
    DWORD dwContinueStatus
    );

BOOL
WINAPI
DebugActiveProcess(
    DWORD dwProcessId
    );

LPVOID
WINAPI
MapViewOfFile(
    HANDLE hFileMappingObject,
    DWORD dwDesiredAccess,
    DWORD dwFileOffsetHigh,
    DWORD dwFileOffsetLow,
    DWORD dwNumberOfBytesToMap
    );

BOOL
WINAPI
UnmapViewOfFile(
    LPCVOID lpBaseAddress
    );

WINBASEAPI
BOOL
WINAPI
FlushViewOfFile(
    LPCVOID lpBaseAddress,
    DWORD dwNumberOfBytesToFlush
    );

WINBASEAPI
HANDLE
WINAPI
CreateFileMappingA(
    HANDLE hFile,
    LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    DWORD flProtect,
    DWORD dwMaximumSizeHigh,
    DWORD dwMaximumSizeLow,
    LPCSTR lpName
    );

WINBASEAPI
HANDLE
WINAPI
CreateFileMappingW(
    HANDLE hFile,
    LPSECURITY_ATTRIBUTES lpFileMappingAttributes,
    DWORD flProtect,
    DWORD dwMaximumSizeHigh,
    DWORD dwMaximumSizeLow,
    LPCWSTR lpName
    );

#ifdef UNICODE
#define CreateFileMapping  CreateFileMappingW
#else
#define CreateFileMapping  CreateFileMappingA
#endif // !UNICODE

WINBASEAPI
HANDLE
WINAPI
CreateFileForMappingW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode,
	LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
	HANDLE hTemplateFile);

#ifdef UNICODE
#define CreateFileForMapping  CreateFileForMappingW
#else
#define CreateFileForMapping  CreateFileForMappingA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
GetTempPathA(
    __in DWORD nBufferLength,
    __out_ecount(nBufferLength) __opt LPSTR lpBuffer
    );

WINBASEAPI
DWORD
WINAPI
GetTempPathW(
    __in DWORD nBufferLength,
    __out_ecount(nBufferLength) __opt LPWSTR lpBuffer
    );

#ifdef UNICODE
#define GetTempPath  GetTempPathW
#else
#define GetTempPath  GetTempPathA
#endif // !UNICODE

#define TLS_MINIMUM_AVAILABLE   64

HANDLE
WINAPI
CreateThread (
    LPSECURITY_ATTRIBUTES lpsa,
    DWORD   cbStack,
    LPTHREAD_START_ROUTINE lpStartAddr,
    LPVOID lpvThreadParam,
    DWORD fdwCreate,
    LPDWORD lpIDThread
    );

VOID
WINAPI
ExitThread (
    DWORD   dwExitCode
    );

BOOL
WINAPI
TerminateThread(
    HANDLE hThread,
    DWORD dwExitCode
    );

#define STILL_ACTIVE 0x00000103

// @CESYSGEN IF COREDLL_FIBER
// @CESYSGEN ENDIF

LPVOID
WINAPI
TlsGetValue (
    DWORD dwTlsIndex
    );

BOOL
WINAPI
TlsSetValue (
    DWORD dwTlsIndex,
    LPVOID lpvTlsValue
    );


typedef struct _PROCESS_INFORMATION {
    HANDLE  hProcess;
    HANDLE  hThread;
    DWORD   dwProcessId;
    DWORD   dwThreadId;
} PROCESS_INFORMATION, *LPPROCESS_INFORMATION;

typedef struct _STARTUPINFOA {
    DWORD   cb;
    LPSTR   lpReserved;
    LPSTR   lpDesktop;
    LPSTR   lpTitle;
    DWORD   dwX;
    DWORD   dwY;
    DWORD   dwXSize;
    DWORD   dwYSize;
    DWORD   dwXCountChars;
    DWORD   dwYCountChars;
    DWORD   dwFillAttribute;
    DWORD   dwFlags;
    WORD    wShowWindow;
    WORD    cbReserved2;
    LPBYTE  lpReserved2;
    HANDLE  hStdInput;
    HANDLE  hStdOutput;
    HANDLE  hStdError;
} STARTUPINFOA, *LPSTARTUPINFOA;

typedef struct _STARTUPINFOW {
    DWORD   cb;
    LPWSTR  lpReserved;
    LPWSTR  lpDesktop;
    LPWSTR  lpTitle;
    DWORD   dwX;
    DWORD   dwY;
    DWORD   dwXSize;
    DWORD   dwYSize;
    DWORD   dwXCountChars;
    DWORD   dwYCountChars;
    DWORD   dwFillAttribute;
    DWORD   dwFlags;
    WORD    wShowWindow;
    WORD    cbReserved2;
    LPBYTE  lpReserved2;
    HANDLE  hStdInput;
    HANDLE  hStdOutput;
    HANDLE  hStdError;
} STARTUPINFOW, *LPSTARTUPINFOW;

#ifdef UNICODE
typedef STARTUPINFOW STARTUPINFO;
typedef LPSTARTUPINFOW LPSTARTUPINFO;
#else
typedef STARTUPINFOA STARTUPINFO;
typedef LPSTARTUPINFOA LPSTARTUPINFO;
#endif // UNICODE

BOOL
WINAPI
CreateProcessA (
    LPCSTR pszImageName,
    LPCSTR pszCmdLine,
    LPSECURITY_ATTRIBUTES psaProcess,
    LPSECURITY_ATTRIBUTES psaThread,
    BOOL fInheritHandles,
    DWORD fdwCreate,
    LPVOID pvEnvironment,
    LPSTR pszCurDir,
    LPSTARTUPINFO psiStartInfo,
    LPPROCESS_INFORMATION pProcInfo
    );
BOOL
WINAPI
CreateProcessW (
    LPCWSTR pszImageName,
    LPCWSTR pszCmdLine,
    LPSECURITY_ATTRIBUTES psaProcess,
    LPSECURITY_ATTRIBUTES psaThread,
    BOOL fInheritHandles,
    DWORD fdwCreate,
    LPVOID pvEnvironment,
    LPWSTR pszCurDir,
    LPSTARTUPINFO psiStartInfo,
    LPPROCESS_INFORMATION pProcInfo
    );
#ifdef UNICODE
#define CreateProcess CreateProcessW
#else
#define CreateProcess CreateProcessA
#endif

WINBASEAPI
DWORD
WINAPI
GetProcessVersion(
    DWORD ProcessId
    );

__inline DWORD GetDllVersion (HMODULE hMod)
{
    // GetProcessVersion now takes module handle
    // as parameter as well as process id.
    return GetProcessVersion ((DWORD) hMod);
}

WINBASEAPI
LPSTR
WINAPI
GetCommandLineA(
    VOID
    );
WINBASEAPI
LPWSTR
WINAPI
GetCommandLineW(
    VOID
    );
#ifdef UNICODE
#define GetCommandLine  GetCommandLineW
#else
#define GetCommandLine  GetCommandLineA
#endif // !UNICODE

#include <winerror.h>

//
//  These are the generic rights.
//

#define GENERIC_READ                     (0x80000000L)
#define GENERIC_WRITE                    (0x40000000L)
#define GENERIC_EXECUTE                  (0x20000000L)
#define GENERIC_ALL                      (0x10000000L)

#define FILE_SHARE_READ                 0x00000001
#define FILE_SHARE_WRITE                0x00000002

//
// File creation flags must start at the high end since they
// are combined with the attributes
//

#define FILE_FLAG_WRITE_THROUGH         0x80000000
#define FILE_FLAG_OVERLAPPED            0x40000000
#define FILE_FLAG_NO_BUFFERING          0x20000000
#define FILE_FLAG_RANDOM_ACCESS         0x10000000
#define FILE_FLAG_SEQUENTIAL_SCAN       0x08000000
#define FILE_FLAG_DELETE_ON_CLOSE       0x04000000
#define FILE_FLAG_BACKUP_SEMANTICS      0x02000000
#define FILE_FLAG_POSIX_SEMANTICS       0x01000000

#define CREATE_NEW          1
#define CREATE_ALWAYS       2
#define OPEN_EXISTING       3
#define OPEN_ALWAYS         4
#define TRUNCATE_EXISTING   5
#define OPEN_FOR_LOADER		6

// Windows CE File API definitions copied from NT's winbase.h

typedef struct _WIN32_FIND_DATAA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwReserved0;
    DWORD dwReserved1;
    CHAR  cFileName[ MAX_PATH ];
    CHAR  cAlternateFileName[ 14 ];
} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

typedef struct _WIN32_FIND_DATAW {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD dwOID;
    WCHAR cFileName[ MAX_PATH ];
} WIN32_FIND_DATAW, *PWIN32_FIND_DATAW, *LPWIN32_FIND_DATAW;
#ifdef UNICODE
typedef WIN32_FIND_DATAW WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAW PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAW LPWIN32_FIND_DATA;
#else
typedef WIN32_FIND_DATAA WIN32_FIND_DATA;
typedef PWIN32_FIND_DATAA PWIN32_FIND_DATA;
typedef LPWIN32_FIND_DATAA LPWIN32_FIND_DATA;
#endif // UNICODE

typedef struct _WIN32_FILE_ATTRIBUTE_DATA {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
} WIN32_FILE_ATTRIBUTE_DATA, *LPWIN32_FILE_ATTRIBUTE_DATA;

WINBASEAPI
DWORD
WINAPI
GetLastError(
    VOID
    );

WINBASEAPI
DWORD
WINAPI
GetTickCount(
    VOID
    );

WINBASEAPI
DWORD
WINAPI
GetIdleTime(
    VOID
    );

// @CESYSGEN IF COREDLL_FMTMSG

WINBASEAPI
DWORD
WINAPI
FormatMessageA(
    DWORD dwFlags,
    LPCVOID lpSource,
    DWORD dwMessageId,
    DWORD dwLanguageId,
    LPSTR lpBuffer,
    DWORD nSize,
    va_list *Arguments
    );

#ifndef COREDLL
WINBASEAPI
#endif
DWORD
WINAPI
FormatMessageW(
    DWORD dwFlags,
    LPCVOID lpSource,
    DWORD dwMessageId,
    DWORD dwLanguageId,
    LPWSTR lpBuffer,
    DWORD nSize,
    va_list *Arguments
    );

#ifdef UNICODE
#define FormatMessage  FormatMessageW
#else
#define FormatMessage  FormatMessageA
#endif // !UNICODE

// @CESYSGEN ENDIF

#define FORMAT_MESSAGE_ALLOCATE_BUFFER 0x00000100
#define FORMAT_MESSAGE_IGNORE_INSERTS  0x00000200
#define FORMAT_MESSAGE_FROM_STRING     0x00000400
#define FORMAT_MESSAGE_FROM_HMODULE    0x00000800
#define FORMAT_MESSAGE_FROM_SYSTEM     0x00001000
#define FORMAT_MESSAGE_ARGUMENT_ARRAY  0x00002000
#define FORMAT_MESSAGE_MAX_WIDTH_MASK  0x000000FF

WINBASEAPI
BOOL
WINAPI
CloseHandle(
    HANDLE hObject
    );

WINBASEAPI
BOOL
WINAPI
DuplicateHandle(
    HANDLE hSourceProcessHandle,
    HANDLE hSourceHandle,
    HANDLE hTargetProcessHandle,
    LPHANDLE lpTargetHandle,
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    DWORD dwOptions
    );

WINBASEAPI
HGLOBAL
WINAPI
LoadResource(
    HMODULE hModule,
    HRSRC hResInfo
    );

WINBASEAPI
DWORD
WINAPI
SizeofResource(
    HMODULE hModule,
    HRSRC hResInfo
    );

WINBASEAPI
HRSRC
WINAPI
FindResourceA(
    HMODULE hModule,
    LPCSTR lpName,
    LPCSTR lpType
    );

WINBASEAPI
HRSRC
WINAPI
FindResourceW(
    HMODULE hModule,
    LPCWSTR lpName,
    LPCWSTR lpType
    );

#ifdef UNICODE
#define FindResource  FindResourceW
#else
#define FindResource  FindResourceA
#endif  // !UNICODE

int
WINAPI
LoadStringA(
    HINSTANCE hInstance,
    UINT uID,
    LPSTR lpBuffer,
    int nBufferMax);

int
WINAPI
LoadStringW(
    HINSTANCE hInstance,
    UINT uID,
    LPWSTR lpBuffer,
    int nBufferMax);
#ifdef UNICODE
#define LoadString  LoadStringW
#else
#define LoadString  LoadStringA
#endif // !UNICODE

#ifndef UNDER_CE    // The WinCE case is in kfuncs.h
WINBASEAPI
LPVOID
WINAPI
LockResource(
        HGLOBAL hResData
        );
#endif

// @CESYSGEN IF GWES_ATOM
// @CESYSGEN ENDIF

WINBASEAPI
BOOL
WINAPI
TerminateProcess(
    HANDLE hProcess,
    DWORD uExitCode
    );

#define ExitProcess(code)   TerminateProcess (GetCurrentProcess (), (code))

BOOL
WINAPI
FlushInstructionCache(
    HANDLE hProcess,
    LPCVOID lpBaseAddress,
    DWORD dwSize
    );

BOOL
WINAPI
ReadProcessMemory(
    HANDLE hProcess,
    LPCVOID lpBaseAddress,
    LPVOID lpBuffer,
    DWORD nSize,
    LPDWORD lpNumberOfBytesRead
    );

BOOL
WINAPI
WriteProcessMemory(
    HANDLE hProcess,
    LPVOID lpBaseAddress,
    LPVOID lpBuffer,
    DWORD nSize,
    LPDWORD lpNumberOfBytesWritten
    );

HANDLE
WINAPI
OpenProcess(
	DWORD fdwAccess,
	BOOL fInherit,
	DWORD IDProcess
	);

WINBASEAPI
BOOL
WINAPI
GetThreadContext(
    HANDLE hThread,
    LPCONTEXT lpContext
    );

WINBASEAPI
BOOL
WINAPI
SetThreadContext(
    HANDLE hThread,
    CONST CONTEXT *lpContext
    );

WINBASEAPI
DWORD
WINAPI
SuspendThread(
    HANDLE hThread
    );
WINBASEAPI
DWORD
WINAPI
ResumeThread(
    HANDLE hThread
    );

WINBASEAPI
BOOL
WINAPI
GetThreadTimes(
    HANDLE hThread,
    LPFILETIME lpCreationTime,
    LPFILETIME lpExitTime,
    LPFILETIME lpKernelTime,
    LPFILETIME lpUserTime
    );

WINBASEAPI
int
WINAPI
GetThreadPriority(
    HANDLE hThread
    );

WINBASEAPI
BOOL
WINAPI
SetThreadPriority(
    HANDLE hThread,
    int nPriority
    );

WINBASEAPI
int
WINAPI
CeGetThreadPriority(
    HANDLE hThread
    );

WINBASEAPI
BOOL
WINAPI
CeSetThreadPriority(
	HANDLE hThread,
	int nPriority
	);

WINBASEAPI
DWORD
WINAPI
CeGetThreadQuantum(
    HANDLE hThread
    );

WINBASEAPI
BOOL
WINAPI
CeSetThreadQuantum(
	HANDLE hThread,
	DWORD dwTime
	);

WINBASEAPI
LPVOID
WINAPI
CeZeroPointer(
    LPVOID ptr
    );

WINBASEAPI
VOID
WINAPI
SetLastError(
    DWORD dwErrCode
    );

WINBASEAPI
UINT
WINAPI
GetTempFileNameA(
    IN LPCSTR lpPathName,
    IN LPCSTR lpPrefixString,
    IN UINT uUnique,
    __out_ecount(MAX_PATH) LPSTR lpTempFileName
    );
WINBASEAPI
UINT
WINAPI
GetTempFileNameW(
    IN LPCWSTR lpPathName,
    IN LPCWSTR lpPrefixString,
    IN UINT uUnique,
    __out_ecount(MAX_PATH) LPWSTR lpTempFileName
    );
#ifdef UNICODE
#define GetTempFileName  GetTempFileNameW
#else
#define GetTempFileName  GetTempFileNameA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
CeGetCanonicalPathNameA(
    IN LPCSTR lpPathName,
    __out_ecount(MAX_PATH) LPSTR lpCanonicalPathName,
    IN DWORD cchCanonicalPathName,
    IN DWORD dwReserved
    );
WINBASEAPI
DWORD
WINAPI
CeGetCanonicalPathNameW(
    IN LPCWSTR lpPathName,
    __out_ecount(MAX_PATH) LPWSTR lpCanonicalPathName,
    IN DWORD cchCanonicalPathName,
    IN DWORD dwReserved
    );
#ifdef UNICODE
#define CeGetCanonicalPathName  CeGetCanonicalPathNameW
#else
#define CeGetCanonicalPathName  CeGetCanonicalPathNameA
#endif // !UNICODE

// @CESYSGEN IF FILESYS_FSMAIN

BOOL GetDiskFreeSpaceExA(LPCSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes);

BOOL GetDiskFreeSpaceExW(LPCWSTR lpDirectoryName, PULARGE_INTEGER lpFreeBytesAvailableToCaller,
    PULARGE_INTEGER lpTotalNumberOfBytes, PULARGE_INTEGER lpTotalNumberOfFreeBytes);

#ifdef UNICODE
#define GetDiskFreeSpaceEx  GetDiskFreeSpaceExW
#else
#define GetDiskFreeSpaceEx  GetDiskFreeSpaceExA
#endif // !UNICODE

WINBASEAPI
HANDLE
WINAPI
CreateFileA(
    LPCSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );

WINBASEAPI
HANDLE
WINAPI
CreateFileW(
    LPCWSTR lpFileName,
    DWORD dwDesiredAccess,
    DWORD dwShareMode,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    DWORD dwCreationDisposition,
    DWORD dwFlagsAndAttributes,
    HANDLE hTemplateFile
    );
#ifdef UNICODE
#define CreateFile  CreateFileW
#else
#define CreateFile  CreateFileA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
DeleteFileA(
    LPCSTR lpFileName
    );
WINBASEAPI
BOOL
WINAPI
DeleteFileW(
    LPCWSTR lpFileName
    );
#ifdef UNICODE
#define DeleteFile  DeleteFileW
#else
#define DeleteFile  DeleteFileA
#endif // !UNICODE

typedef enum _FINDEX_INFO_LEVELS {
    FindExInfoStandard,
    FindExInfoMaxInfoLevel
} FINDEX_INFO_LEVELS;

typedef enum _FINDEX_SEARCH_OPS {
    FindExSearchNameMatch,
    FindExSearchLimitToDirectories,
    FindExSearchLimitToDevices,
    FindExSearchMaxSearchOp
} FINDEX_SEARCH_OPS;

#define FIND_FIRST_EX_CASE_SENSITIVE   0x00000001

WINBASEAPI
HANDLE
WINAPI
FindFirstFileExA(
    IN LPCSTR lpFileName,
    IN FINDEX_INFO_LEVELS fInfoLevelId,
    OUT LPVOID lpFindFileData,
    IN FINDEX_SEARCH_OPS fSearchOp,
    IN LPVOID lpSearchFilter,
    IN DWORD dwAdditionalFlags
    );
WINBASEAPI
HANDLE
WINAPI
FindFirstFileExW(
    IN LPCWSTR lpFileName,
    IN FINDEX_INFO_LEVELS fInfoLevelId,
    OUT LPVOID lpFindFileData,
    IN FINDEX_SEARCH_OPS fSearchOp,
    IN LPVOID lpSearchFilter,
    IN DWORD dwAdditionalFlags
    );
#ifdef UNICODE
#define FindFirstFileEx  FindFirstFileExW
#else
#define FindFirstFileEx  FindFirstFileExA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
DeleteAndRenameFile(
    LPCWSTR lpOldFileName,
    LPCWSTR lpNewFileName
    );

WINBASEAPI
HANDLE
WINAPI
FindFirstFileA(
    LPCSTR lpFileName,
    LPWIN32_FIND_DATAA lpFindFileData
    );

WINBASEAPI
HANDLE
WINAPI
FindFirstFileW(
    LPCWSTR lpFileName,
    LPWIN32_FIND_DATAW lpFindFileData
    );
#ifdef UNICODE
#define FindFirstFile  FindFirstFileW
#else
#define FindFirstFile  FindFirstFileA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindNextFileA(
    HANDLE hFindFile,
    LPWIN32_FIND_DATAA lpFindFileData
    );

WINBASEAPI
BOOL
WINAPI
FindNextFileW(
    HANDLE hFindFile,
    LPWIN32_FIND_DATAW lpFindFileData
    );
#ifdef UNICODE
#define FindNextFile  FindNextFileW
#else
#define FindNextFile  FindNextFileA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
CopyFileA(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName,
    BOOL bFailIfExists
    );
WINBASEAPI
BOOL
WINAPI
CopyFileW(
    LPCWSTR lpExistingFileName,
    LPCWSTR lpNewFileName,
    BOOL bFailIfExists
    );
#ifdef UNICODE
#define CopyFile  CopyFileW
#else
#define CopyFile  CopyFileA
#endif // !UNICODE

#define CALLBACK_CHUNK_FINISHED               0x00000000
#define CALLBACK_STREAM_SWITCH                0x00000001

#define COPY_FILE_FAIL_IF_EXISTS              0x00000001
#define COPY_FILE_RESTARTABLE                 0x00000002
#define COPY_FILE_OPEN_SOURCE_FOR_WRITE       0x00000004
#define COPY_FILE_ALLOW_DECRYPTED_DESTINATION 0x00000008

#define PROGRESS_CONTINUE                     0
#define PROGRESS_CANCEL                       1
#define PROGRESS_STOP                         2
#define PROGRESS_QUIET                        3

typedef
DWORD
(WINAPI* LPPROGRESS_ROUTINE)(
    LARGE_INTEGER TotalFileSize,
    LARGE_INTEGER TotalBytesTransferred,
    LARGE_INTEGER StreamSize,
    LARGE_INTEGER StreamBytesTransferred,
    DWORD dwStreamNumber,
    DWORD dwCallbackReason,
    HANDLE hSourceFile,
    HANDLE hDestinationFile,
    LPVOID lpData
    );

WINBASEAPI
BOOL
WINAPI
CopyFileExA(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName,
    LPPROGRESS_ROUTINE lpProgressRoutine,
    LPVOID lpData,
    LPBOOL pbCancel,
    DWORD dwCopyFlags
    );
WINBASEAPI
BOOL
WINAPI
CopyFileExW(
    LPCWSTR lpExistingFileName,
    LPCWSTR lpNewFileName,
    LPPROGRESS_ROUTINE lpProgressRoutine,
    LPVOID lpData,
    LPBOOL pbCancel,
    DWORD dwCopyFlags
    );
#ifdef UNICODE
#define CopyFileEx  CopyFileExW
#else
#define CopyFileEx  CopyFileExA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
MoveFileA(
    LPCSTR lpExistingFileName,
    LPCSTR lpNewFileName
    );
WINBASEAPI
BOOL
WINAPI
MoveFileW(
    LPCWSTR lpExistingFileName,
    LPCWSTR lpNewFileName
    );
#ifdef UNICODE
#define MoveFile  MoveFileW
#else
#define MoveFile  MoveFileA
#endif // !UNICODE


WINBASEAPI
BOOL
WINAPI
CreateDirectoryA(
    LPCSTR lpPathName,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
WINBASEAPI
BOOL
WINAPI
CreateDirectoryW(
    LPCWSTR lpPathName,
    LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#ifdef UNICODE
#define CreateDirectory  CreateDirectoryW
#else
#define CreateDirectory  CreateDirectoryA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
RemoveDirectoryA(
    LPCSTR lpPathName
    );
WINBASEAPI
BOOL
WINAPI
RemoveDirectoryW(
    LPCWSTR lpPathName
    );
#ifdef UNICODE
#define RemoveDirectory  RemoveDirectoryW
#else
#define RemoveDirectory  RemoveDirectoryA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
WriteFile(
    HANDLE hFile,
    LPCVOID lpBuffer,
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpNumberOfBytesWritten,
    LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
ReadFile(
    HANDLE hFile,
    LPVOID lpBuffer,
    DWORD nNumberOfBytesToRead,
    LPDWORD lpNumberOfBytesRead,
    LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
ReadFileScatter(
    HANDLE hFile,
    FILE_SEGMENT_ELEMENT aSegmentArray[],
    DWORD nNumberOfBytesToRead,
    LPDWORD lpReserved,
    LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
WriteFileGather(
    HANDLE hFile,
    FILE_SEGMENT_ELEMENT aSegmentArray[],
    DWORD nNumberOfBytesToWrite,
    LPDWORD lpReserved,
    LPOVERLAPPED lpOverlapped
    );

#define LOCKFILE_FAIL_IMMEDIATELY 0x00000001
#define LOCKFILE_EXCLUSIVE_LOCK   0x00000002

WINBASEAPI
BOOL
WINAPI
LockFileEx(
    HANDLE hFile,
    DWORD dwFlags,
    DWORD dwReserved,
    DWORD nNumberOfBytesToLockLow,
    DWORD nNumberOfBytesToLockHigh,
    LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
WINAPI
UnlockFileEx(
    HANDLE hFile,
    DWORD dwReserved,
    DWORD nNumberOfBytesToUnlockLow,
    DWORD nNumberOfBytesToUnlockHigh,
    LPOVERLAPPED lpOverlapped
    );

WINBASEAPI BOOL WINAPI FlushFileBuffers (HANDLE hFile);
WINBASEAPI BOOL WINAPI GetFileTime (HANDLE hFile, LPFILETIME lpCreation, LPFILETIME lpLastAccess, LPFILETIME lpLastWrite);
WINBASEAPI BOOL WINAPI SetFileTime (HANDLE hFile, CONST FILETIME *lpCreation, CONST FILETIME *lpLastAccess, CONST FILETIME *lpLastWrite);
WINBASEAPI BOOL WINAPI SetEndOfFile (HANDLE hFile);
WINBASEAPI BOOL WINAPI DeviceIoControl (HANDLE hDevice, DWORD dwIoControlCode, LPVOID lpInBuf, DWORD nInBufSize, LPVOID lpOutBuf, DWORD nOutBufSize, LPDWORD lpBytesReturned, LPOVERLAPPED lpOverlapped);

WINBASEAPI
HANDLE
WINAPI
FindFirstChangeNotificationA(
    IN LPCSTR lpPathName,
    IN BOOL bWatchSubtree,
    IN DWORD dwNotifyFilter
    );
WINBASEAPI
HANDLE
WINAPI
FindFirstChangeNotificationW(
    IN LPCWSTR lpPathName,
    IN BOOL bWatchSubtree,
    IN DWORD dwNotifyFilter
    );
#ifdef UNICODE
#define FindFirstChangeNotification  FindFirstChangeNotificationW
#else
#define FindFirstChangeNotification  FindFirstChangeNotificationA
#endif // !UNICODE

WINBASEAPI
BOOL
WINAPI
FindNextChangeNotification(
    IN HANDLE hChangeHandle
    );

WINBASEAPI
BOOL
WINAPI
CeGetFileNotificationInfo(
    HANDLE hChangeHandle,
    DWORD dwFlags, 
    LPVOID lpBuffer, 
    DWORD nBufferLength, 
    LPDWORD lpBytesReturned, 
    LPDWORD lpBytesAvailable
    );


WINBASEAPI
BOOL
WINAPI
FindCloseChangeNotification(
    IN HANDLE hChangeHandle
    );

WINBASEAPI
BOOL
CeFsIoControlW(
    IN LPCWSTR pszRootPath, 
    IN DWORD dwIoctl, 
    IN LPVOID lpInBuf, 
    IN DWORD nInBufSize, 
    OUT LPVOID lpOutBuf, 
    IN DWORD nOutBufSize, 
    OUT LPDWORD lpBytesReturned, 
    IN LPOVERLAPPED lpOverlapped
    );

WINBASEAPI
BOOL
CeFsIoControlA(
    IN LPCSTR pszRootPath, 
    IN DWORD dwIoctl, 
    IN LPVOID lpInBuf, 
    IN DWORD nInBufSize, 
    OUT LPVOID lpOutBuf, 
    IN DWORD nOutBufSize, 
    OUT LPDWORD lpBytesReturned, 
    IN LPOVERLAPPED lpOverlapped
    );

#ifdef UNICODE
#define CeFsIoControl  CeFsIoControlW
#else
#define CeFsIoControl  CeFsIoControlA
#endif // !UNICODE

// @CESYSGEN IF COREDLL_ASYNCIO
// @CESYSGEN ENDIF

WINBASEAPI
BOOL
WINAPI
SetFileAttributesA(
    LPCSTR lpFileName,
    DWORD dwFileAttributes
    );
WINBASEAPI
BOOL
WINAPI
SetFileAttributesW(
    LPCWSTR lpFileName,
    DWORD dwFileAttributes
    );
#ifdef UNICODE
#define SetFileAttributes  SetFileAttributesW
#else
#define SetFileAttributes  SetFileAttributesA
#endif // !UNICODE

WINBASEAPI
DWORD
WINAPI
SetFilePointer(
    HANDLE hFile,
    LONG lDistanceToMove,
    PLONG lpDistanceToMoveHigh,
    DWORD dwMoveMethod
    );

WINBASEAPI
BOOL
WINAPI
FindClose(
    HANDLE hFindFile
    );


WINBASEAPI
DWORD
WINAPI
GetFileSize(
    HANDLE hFile,
    LPDWORD lpFileSizeHigh
    );

WINBASEAPI
DWORD
WINAPI
GetFileAttributesA(
    LPCSTR lpFileName
    );
WINBASEAPI
DWORD
WINAPI
GetFileAttributesW(
    LPCWSTR lpFileName
    );
#ifdef UNICODE
#define GetFileAttributes  GetFileAttributesW
#else
#define GetFileAttributes  GetFileAttributesA
#endif // !UNICODE

typedef enum _GET_FILEEX_INFO_LEVELS {
    GetFileExInfoStandard,
    GetFileExMaxInfoLevel
} GET_FILEEX_INFO_LEVELS;

WINBASEAPI
BOOL
WINAPI
GetFileAttributesExA(
    IN LPCSTR lpFileName,
    IN GET_FILEEX_INFO_LEVELS fInfoLevelId,
    OUT LPVOID lpFileInformation
    );
WINBASEAPI
BOOL
WINAPI
GetFileAttributesExW(
    IN LPCWSTR lpFileName,
    IN GET_FILEEX_INFO_LEVELS fInfoLevelId,
    OUT LPVOID lpFileInformation
    );
#ifdef UNICODE
#define GetFileAttributesEx  GetFileAttributesExW
#else
#define GetFileAttributesEx  GetFileAttributesExA
#endif // !UNICODE
typedef struct _BY_HANDLE_FILE_INFORMATION {
    DWORD dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    DWORD dwVolumeSerialNumber;
    DWORD nFileSizeHigh;
    DWORD nFileSizeLow;
    DWORD nNumberOfLinks;
    DWORD nFileIndexHigh;
    DWORD nFileIndexLow;
	DWORD dwOID;
} BY_HANDLE_FILE_INFORMATION, *PBY_HANDLE_FILE_INFORMATION, *LPBY_HANDLE_FILE_INFORMATION;

WINBASEAPI
BOOL
WINAPI
GetFileInformationByHandle(
    HANDLE hFile,
    LPBY_HANDLE_FILE_INFORMATION lpFileInformation
	);

// @CESYSGEN ENDIF

WINBASEAPI
VOID
WINAPI
Sleep(
    DWORD dwMilliseconds
    );

WINBASEAPI
HANDLE
WINAPI
CreateMutexA(
	LPSECURITY_ATTRIBUTES lpsa,
	BOOL bInitialOwner,
	LPCSTR lpName);

WINBASEAPI
HANDLE
WINAPI
CreateMutexW(
	LPSECURITY_ATTRIBUTES lpsa,
	BOOL bInitialOwner,
	LPCWSTR lpName);

#ifdef UNICODE
#define CreateMutex  CreateMutexW
#else
#define CreateMutex  CreateMutexA
#endif // !UNICODE

WINBASEAPI
HANDLE
WINAPI
CreateEventA(
    LPSECURITY_ATTRIBUTES lpEventAttributes,
    BOOL bManualReset,
    BOOL bInitialState,
    LPCSTR lpName
    );

WINBASEAPI
HANDLE
WINAPI
CreateEventW(
    LPSECURITY_ATTRIBUTES lpEventAttributes,
    BOOL bManualReset,
    BOOL bInitialState,
    LPCWSTR lpName
    );
#ifdef UNICODE
#define CreateEvent  CreateEventW
#else
#define CreateEvent  CreateEventA
#endif // !UNICODE

WINBASEAPI
HANDLE
WINAPI
OpenEventA(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    LPCSTR lpName
    );

WINBASEAPI
HANDLE
WINAPI
OpenEventW(
    DWORD dwDesiredAccess,
    BOOL bInheritHandle,
    LPCWSTR lpName
    );
#ifdef UNICODE
#define OpenEvent  OpenEventW
#else
#define OpenEvent  OpenEventA
#endif // !UNICODE

WINBASEAPI
HANDLE
WINAPI
CreateSemaphoreA(
    IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    IN LONG lInitialCount,
    IN LONG lMaximumCount,
    IN LPCSTR lpName
    );
WINBASEAPI
HANDLE
WINAPI
CreateSemaphoreW(
    IN LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
    IN LONG lInitialCount,
    IN LONG lMaximumCount,
    IN LPCWSTR lpName
    );
#ifdef UNICODE
#define CreateSemaphore  CreateSemaphoreW
#else
#define CreateSemaphore  CreateSemaphoreA
#endif // !UNICODE

#ifndef UNDER_CE    // The WinCE case is in kfuncs.h
WINBASEAPI
BOOL
WINAPI
SetEvent(
    HANDLE hEvent
    );

WINBASEAPI
BOOL
WINAPI
ResetEvent(
    HANDLE hEvent
    );

WINBASEAPI
BOOL
WINAPI
PulseEvent(
    HANDLE hEvent
    );
#endif

WINBASEAPI
BOOL
WINAPI
ReleaseSemaphore(
    IN HANDLE hSemaphore,
    IN LONG lReleaseCount,
    OUT LPLONG lpPreviousCount
    );

WINBASEAPI
BOOL
WINAPI
ReleaseMutex(
    HANDLE hMutex
    );

WINBASEAPI
DWORD
WINAPI
WaitForSingleObject(
    HANDLE hHandle,
    DWORD dwMilliseconds
    );

WINBASEAPI
DWORD
WINAPI
WaitForMultipleObjects(
	DWORD cObjects,
	CONST HANDLE *lphObjects,
	BOOL fWaitAll,
	DWORD dwTimeout
	);

WINBASEAPI
BOOL
WINAPI
GetExitCodeThread(
    HANDLE hThread,
    LPDWORD lpExitCode
    );

WINBASEAPI
BOOL
WINAPI
GetExitCodeProcess(
    HANDLE hProcess,
    LPDWORD lpExitCode
    );

#ifndef UNDER_CE    // The WinCE case is in kfuncs.h
WINBASEAPI
HANDLE
WINAPI
GetCurrentThread(
    VOID
    );

WINBASEAPI
HANDLE
WINAPI
GetCurrentProcess(
    VOID
    );

WINBASEAPI
DWORD
WINAPI
GetCurrentThreadId(
    VOID
    );

WINBASEAPI
DWORD
WINAPI
GetCurrentProcessId(
    VOID
    );
#endif

DWORD
WINAPI
TlsAlloc(
    VOID
    );

WINBASEAPI
BOOL
WINAPI
IsBadCodePtr(
    FARPROC lpfn
    );

WINBASEAPI
BOOL
WINAPI
IsBadReadPtr(
    CONST VOID *lp,
    UINT ucb
    );

BOOL
WINAPI
TlsFree(
    DWORD dwTlsIndex
    );

WINBASEAPI
BOOL
WINAPI
IsBadWritePtr(
    LPVOID lp,
    UINT ucb
	);

WINBASEAPI
VOID
WINAPI
GetSystemInfo(
    LPSYSTEM_INFO lpSystemInfo
    );

WINBASEAPI
BOOL
WINAPI
QueryInstructionSet(
    DWORD dwInstructionSet,
    LPDWORD lpdwCurrentInstructionSet
    );

WINBASEAPI
BOOL
WINAPI
IsProcessorFeaturePresent(
    DWORD dwProcessorFeature
    );

WINBASEAPI
VOID
WINAPI
RaiseException(
    DWORD dwExceptionCode,
    DWORD dwExceptionFlags,
    DWORD nNumberOfArguments,
    CONST DWORD *lpArguments
    );

WINBASEAPI
BOOL
WINAPI
FreeLibrary(
    HMODULE hLibModule
    );

WINBASEAPI
VOID
WINAPI
FreeLibraryAndExitThread(
    HMODULE hLibModule,
    DWORD dwExitCode
    );

WINBASEAPI
BOOL
WINAPI
DisableThreadLibraryCalls(
    IN HMODULE hLibModule
    );
    
#ifdef UNDER_CE
#include <dbgapi.h>
#include <kfuncs.h>
#endif

#define FILE_MAP_WRITE      SECTION_MAP_WRITE
#define FILE_MAP_READ       SECTION_MAP_READ
#define FILE_MAP_ALL_ACCESS SECTION_ALL_ACCESS

// @CESYSGEN IF COREDLL_SERDEV

//
// Serial provider type.
//

#define SP_SERIALCOMM    ((DWORD)0x00000001)

//
// Provider SubTypes
//

#define PST_UNSPECIFIED      ((DWORD)0x00000000)
#define PST_RS232            ((DWORD)0x00000001)
#define PST_PARALLELPORT     ((DWORD)0x00000002)
#define PST_RS422            ((DWORD)0x00000003)
#define PST_RS423            ((DWORD)0x00000004)
#define PST_RS449            ((DWORD)0x00000005)
#define PST_MODEM            ((DWORD)0x00000006)
#define PST_FAX              ((DWORD)0x00000021)
#define PST_SCANNER          ((DWORD)0x00000022)
#define PST_NETWORK_BRIDGE   ((DWORD)0x00000100)
#define PST_LAT              ((DWORD)0x00000101)
#define PST_TCPIP_TELNET     ((DWORD)0x00000102)
#define PST_X25              ((DWORD)0x00000103)


//
// Provider capabilities flags.
//

#define PCF_DTRDSR        ((DWORD)0x0001)
#define PCF_RTSCTS        ((DWORD)0x0002)
#define PCF_RLSD          ((DWORD)0x0004)
#define PCF_PARITY_CHECK  ((DWORD)0x0008)
#define PCF_XONXOFF       ((DWORD)0x0010)
#define PCF_SETXCHAR      ((DWORD)0x0020)
#define PCF_TOTALTIMEOUTS ((DWORD)0x0040)
#define PCF_INTTIMEOUTS   ((DWORD)0x0080)
#define PCF_SPECIALCHARS  ((DWORD)0x0100)
#define PCF_16BITMODE     ((DWORD)0x0200)

//
// Comm provider settable parameters.
//

#define SP_PARITY         ((DWORD)0x0001)
#define SP_BAUD           ((DWORD)0x0002)
#define SP_DATABITS       ((DWORD)0x0004)
#define SP_STOPBITS       ((DWORD)0x0008)
#define SP_HANDSHAKING    ((DWORD)0x0010)
#define SP_PARITY_CHECK   ((DWORD)0x0020)
#define SP_RLSD           ((DWORD)0x0040)

//
// Settable baud rates in the provider.
//

#define BAUD_075          ((DWORD)0x00000001)
#define BAUD_110          ((DWORD)0x00000002)
#define BAUD_134_5        ((DWORD)0x00000004)
#define BAUD_150          ((DWORD)0x00000008)
#define BAUD_300          ((DWORD)0x00000010)
#define BAUD_600          ((DWORD)0x00000020)
#define BAUD_1200         ((DWORD)0x00000040)
#define BAUD_1800         ((DWORD)0x00000080)
#define BAUD_2400         ((DWORD)0x00000100)
#define BAUD_4800         ((DWORD)0x00000200)
#define BAUD_7200         ((DWORD)0x00000400)
#define BAUD_9600         ((DWORD)0x00000800)
#define BAUD_14400        ((DWORD)0x00001000)
#define BAUD_19200        ((DWORD)0x00002000)
#define BAUD_38400        ((DWORD)0x00004000)
#define BAUD_56K          ((DWORD)0x00008000)
#define BAUD_128K         ((DWORD)0x00010000)
#define BAUD_115200       ((DWORD)0x00020000)
#define BAUD_57600        ((DWORD)0x00040000)
#define BAUD_USER         ((DWORD)0x10000000)

//
// Settable Data Bits
//

#define DATABITS_5        ((WORD)0x0001)
#define DATABITS_6        ((WORD)0x0002)
#define DATABITS_7        ((WORD)0x0004)
#define DATABITS_8        ((WORD)0x0008)
#define DATABITS_16       ((WORD)0x0010)
#define DATABITS_16X      ((WORD)0x0020)

//
// Settable Stop and Parity bits.
//

#define STOPBITS_10       ((WORD)0x0001)
#define STOPBITS_15       ((WORD)0x0002)
#define STOPBITS_20       ((WORD)0x0004)
#define PARITY_NONE       ((WORD)0x0100)
#define PARITY_ODD        ((WORD)0x0200)
#define PARITY_EVEN       ((WORD)0x0400)
#define PARITY_MARK       ((WORD)0x0800)
#define PARITY_SPACE      ((WORD)0x1000)

typedef struct _COMMPROP {
    WORD wPacketLength;
    WORD wPacketVersion;
    DWORD dwServiceMask;
    DWORD dwReserved1;
    DWORD dwMaxTxQueue;
    DWORD dwMaxRxQueue;
    DWORD dwMaxBaud;
    DWORD dwProvSubType;
    DWORD dwProvCapabilities;
    DWORD dwSettableParams;
    DWORD dwSettableBaud;
    WORD wSettableData;
    WORD wSettableStopParity;
    DWORD dwCurrentTxQueue;
    DWORD dwCurrentRxQueue;
    DWORD dwProvSpec1;
    DWORD dwProvSpec2;
    WCHAR wcProvChar[1];
} COMMPROP,*LPCOMMPROP;

//
// Set dwProvSpec1 to COMMPROP_INITIALIZED to indicate that wPacketLength
// is valid before a call to GetCommProperties().
//
#define COMMPROP_INITIALIZED ((DWORD)0xE73CF52E)

typedef struct _COMSTAT {
    DWORD fCtsHold : 1;
    DWORD fDsrHold : 1;
    DWORD fRlsdHold : 1;
    DWORD fXoffHold : 1;
    DWORD fXoffSent : 1;
    DWORD fEof : 1;
    DWORD fTxim : 1;
    DWORD fReserved : 25;
    DWORD cbInQue;
    DWORD cbOutQue;
} COMSTAT, *LPCOMSTAT;

//
// DTR Control Flow Values.
//
#define DTR_CONTROL_DISABLE    0x00
#define DTR_CONTROL_ENABLE     0x01
#define DTR_CONTROL_HANDSHAKE  0x02

//
// RTS Control Flow Values
//
#define RTS_CONTROL_DISABLE    0x00
#define RTS_CONTROL_ENABLE     0x01
#define RTS_CONTROL_HANDSHAKE  0x02
#define RTS_CONTROL_TOGGLE     0x03

typedef struct _DCB {
    DWORD DCBlength;      /* sizeof(DCB)                     */
    DWORD BaudRate;       /* Baudrate at which running       */
    DWORD fBinary: 1;     /* Binary Mode (skip EOF check)    */
    DWORD fParity: 1;     /* Enable parity checking          */
    DWORD fOutxCtsFlow:1; /* CTS handshaking on output       */
    DWORD fOutxDsrFlow:1; /* DSR handshaking on output       */
    DWORD fDtrControl:2;  /* DTR Flow control                */
    DWORD fDsrSensitivity:1; /* DSR Sensitivity              */
    DWORD fTXContinueOnXoff: 1; /* Continue TX when Xoff sent */
    DWORD fOutX: 1;       /* Enable output X-ON/X-OFF        */
    DWORD fInX: 1;        /* Enable input X-ON/X-OFF         */
    DWORD fErrorChar: 1;  /* Enable Err Replacement          */
    DWORD fNull: 1;       /* Enable Null stripping           */
    DWORD fRtsControl:2;  /* Rts Flow control                */
    DWORD fAbortOnError:1; /* Abort all reads and writes on Error */
    DWORD fDummy2:17;     /* Reserved                        */
    WORD wReserved;       /* Not currently used              */
    WORD XonLim;          /* Transmit X-ON threshold         */
    WORD XoffLim;         /* Transmit X-OFF threshold        */
    BYTE ByteSize;        /* Number of bits/byte, 4-8        */
    BYTE Parity;          /* 0-4=None,Odd,Even,Mark,Space    */
    BYTE StopBits;        /* 0,1,2 = 1, 1.5, 2               */
    char XonChar;         /* Tx and Rx X-ON character        */
    char XoffChar;        /* Tx and Rx X-OFF character       */
    char ErrorChar;       /* Error replacement char          */
    char EofChar;         /* End of Input character          */
    char EvtChar;         /* Received Event character        */
    WORD wReserved1;      /* Fill for now.                   */
} DCB, *LPDCB;

typedef struct _COMMTIMEOUTS {
    DWORD ReadIntervalTimeout;          /* Maximum time between read chars. */
    DWORD ReadTotalTimeoutMultiplier;   /* Multiplier of characters.        */
    DWORD ReadTotalTimeoutConstant;     /* Constant in milliseconds.        */
    DWORD WriteTotalTimeoutMultiplier;  /* Multiplier of characters.        */
    DWORD WriteTotalTimeoutConstant;    /* Constant in milliseconds.        */
} COMMTIMEOUTS,*LPCOMMTIMEOUTS;

typedef struct _COMMCONFIG {
    DWORD dwSize;               /* Size of the entire struct */
    WORD wVersion;              /* version of the structure */
    WORD wReserved;             /* alignment */
    DCB dcb;                    /* device control block */
    DWORD dwProviderSubType;    /* ordinal value for identifying
                                   provider-defined data structure format*/
    DWORD dwProviderOffset;     /* Specifies the offset of provider specific
                                   data field in bytes from the start */
    DWORD dwProviderSize;       /* size of the provider-specific data field */
    WCHAR wcProviderData[1];    /* provider-specific data */
} COMMCONFIG,*LPCOMMCONFIG;

#define NOPARITY            0
#define ODDPARITY           1
#define EVENPARITY          2
#define MARKPARITY          3
#define SPACEPARITY         4

#define ONESTOPBIT          0
#define ONE5STOPBITS        1
#define TWOSTOPBITS         2

#define IGNORE              0       // Ignore signal

//
// Basud rates at which the communication device operates
//

#define CBR_110             110
#define CBR_300             300
#define CBR_600             600
#define CBR_1200            1200
#define CBR_2400            2400
#define CBR_4800            4800
#define CBR_9600            9600
#define CBR_14400           14400
#define CBR_19200           19200
#define CBR_38400           38400
#define CBR_56000           56000
#define CBR_57600           57600
#define CBR_115200          115200
#define CBR_128000          128000
#define CBR_256000          256000

//
// Error Flags
//

#define CE_RXOVER           0x0001  // Receive Queue overflow
#define CE_OVERRUN          0x0002  // Receive Overrun Error
#define CE_RXPARITY         0x0004  // Receive Parity Error
#define CE_FRAME            0x0008  // Receive Framing error
#define CE_BREAK            0x0010  // Break Detected
#define CE_TXFULL           0x0100  // TX Queue is full
#define CE_PTO              0x0200  // LPTx Timeout
#define CE_IOE              0x0400  // LPTx I/O Error
#define CE_DNS              0x0800  // LPTx Device not selected
#define CE_OOP              0x1000  // LPTx Out-Of-Paper
#define CE_MODE             0x8000  // Requested mode unsupported

#define IE_BADID            (-1)    // Invalid or unsupported id
#define IE_OPEN             (-2)    // Device Already Open
#define IE_NOPEN            (-3)    // Device Not Open
#define IE_MEMORY           (-4)    // Unable to allocate queues
#define IE_DEFAULT          (-5)    // Error in default parameters
#define IE_HARDWARE         (-10)   // Hardware Not Present
#define IE_BYTESIZE         (-11)   // Illegal Byte Size
#define IE_BAUDRATE         (-12)   // Unsupported BaudRate

//
// Events
//

#define EV_RXCHAR           0x0001  // Any Character received
#define EV_RXFLAG           0x0002  // Received certain character
#define EV_TXEMPTY          0x0004  // Transmitt Queue Empty
#define EV_CTS              0x0008  // CTS changed state
#define EV_DSR              0x0010  // DSR changed state
#define EV_RLSD             0x0020  // RLSD changed state
#define EV_BREAK            0x0040  // BREAK received
#define EV_ERR              0x0080  // Line status error occurred
#define EV_RING             0x0100  // Ring signal detected
#define EV_PERR             0x0200  // Printer error occured
#define EV_RX80FULL         0x0400  // Receive buffer is 80 percent full
#define EV_EVENT1           0x0800  // Provider specific event 1
#define EV_EVENT2           0x1000  // Provider specific event 2
#define EV_POWER			0x2000  // WINCE Power event.

//
// Escape Functions
//

#define SETXOFF             1       // Simulate XOFF received
#define SETXON              2       // Simulate XON received
#define SETRTS              3       // Set RTS high
#define CLRRTS              4       // Set RTS low
#define SETDTR              5       // Set DTR high
#define CLRDTR              6       // Set DTR low
// Gap for NT code RESETDEV, not supported on CE
#define SETBREAK            8       // Set the device break line.
#define CLRBREAK            9       // Clear the device break line.
// Some CE devices share a UART between an IRDA port and a serial port.
// These escape functions allow control over the mode.
#define SETIR               10      // Set the port to IR mode.
#define CLRIR               11      // Set the port to non-IR mode.

//
// PURGE function flags.
//
#define PURGE_TXABORT       0x0001  // Kill the pending/current writes to the comm port.
#define PURGE_RXABORT       0x0002  // Kill the pending/current reads to the comm port.
#define PURGE_TXCLEAR       0x0004  // Kill the transmit queue if there.
#define PURGE_RXCLEAR       0x0008  // Kill the typeahead buffer if there.

#define LPTx                0x80    // Set if ID is for LPT device

//
// Modem Status Flags
//
#define MS_CTS_ON           ((DWORD)0x0010)
#define MS_DSR_ON           ((DWORD)0x0020)
#define MS_RING_ON          ((DWORD)0x0040)
#define MS_RLSD_ON          ((DWORD)0x0080)

BOOL
WINAPI
ClearCommBreak(
    HANDLE hFile
    );


BOOL
WINAPI
ClearCommError(
    HANDLE hFile,
    LPDWORD lpErrors,
    LPCOMSTAT lpStat
    );


BOOL
WINAPI
SetupComm(
    HANDLE hFile,
    DWORD dwInQueue,
    DWORD dwOutQueue
    );


BOOL
WINAPI
EscapeCommFunction(
    HANDLE hFile,
    DWORD dwFunc
    );


BOOL
WINAPI
GetCommMask(
    HANDLE hFile,
    LPDWORD lpEvtMask
    );


BOOL
WINAPI
GetCommProperties(
    HANDLE hFile,
    LPCOMMPROP lpCommProp
    );


BOOL
WINAPI
GetCommModemStatus(
    HANDLE hFile,
    LPDWORD lpModemStat
    );


BOOL
WINAPI
GetCommState(
    HANDLE hFile,
    LPDCB lpDCB
    );


BOOL
WINAPI
GetCommTimeouts(
    HANDLE hFile,
    LPCOMMTIMEOUTS lpCommTimeouts
    );


BOOL
WINAPI
PurgeComm(
    HANDLE hFile,
    DWORD dwFlags
    );


BOOL
WINAPI
SetCommBreak(
    HANDLE hFile
    );


BOOL
WINAPI
SetCommMask(
    HANDLE hFile,
    DWORD dwEvtMask
    );


BOOL
WINAPI
SetCommState(
    HANDLE hFile,
    LPDCB lpDCB
    );


BOOL
WINAPI
SetCommTimeouts(
    HANDLE hFile,
    LPCOMMTIMEOUTS lpCommTimeouts
    );


BOOL
WINAPI
TransmitCommChar(
    HANDLE hFile,
    char cChar
    );


BOOL
WINAPI
WaitCommEvent(
    HANDLE hFile,
    LPDWORD lpEvtMask,
    LPOVERLAPPED lpOverlapped
    );

// End if IF COREDLL_SERDEV
// @CESYSGEN ENDIF


//
// Power Management APIs
//
#define AC_LINE_OFFLINE                 0x00
#define AC_LINE_ONLINE                  0x01
#define AC_LINE_BACKUP_POWER            0x02
#define AC_LINE_UNKNOWN                 0xFF

#define BATTERY_FLAG_HIGH               0x01
#define BATTERY_FLAG_LOW                0x02
#define BATTERY_FLAG_CRITICAL           0x04
#define BATTERY_FLAG_CHARGING           0x08
#define BATTERY_FLAG_NO_BATTERY         0x80
#define BATTERY_FLAG_UNKNOWN            0xFF

#define BATTERY_PERCENTAGE_UNKNOWN      0xFF

#define BATTERY_LIFE_UNKNOWN        0xFFFFFFFF

typedef struct _SYSTEM_POWER_STATUS_EX {
    BYTE ACLineStatus;
    BYTE BatteryFlag;
    BYTE BatteryLifePercent;
    BYTE Reserved1;
    DWORD BatteryLifeTime;
    DWORD BatteryFullLifeTime;
    BYTE Reserved2;
    BYTE BackupBatteryFlag;
    BYTE BackupBatteryLifePercent;
    BYTE Reserved3;
    DWORD BackupBatteryLifeTime;
    DWORD BackupBatteryFullLifeTime;
}   SYSTEM_POWER_STATUS_EX, *PSYSTEM_POWER_STATUS_EX, *LPSYSTEM_POWER_STATUS_EX;



/*
	@struct SYSTEM_POWER_STATUS_EX2 | contains information about the power status
	of the system.  

    @field BYTE | ACLineStatus | AC power status. Must be one of 
    AC_LINE_OFFLINE, AC_LINE_ONLINE, AC_LINE_BACKUP_POWER, or AC_LINE_UNKNOWN.

    @field BYTE | BatteryFlag | Battery charge status.  Must be one of 
	BATTERY_FLAG_HIGH, BATTERY_FLAG_LOW, BATTERY_FLAG_CRITICAL,
	BATTERY_FLAG_CHARGING, BATTERY_FLAG_NO_BATTERY, or BATTERY_FLAG_UNKNOWN    

    @field BYTE | BatteryLifePercent | Percentage of full battery charge 
    remaining.  Must be in the range 0 to 100, or BATTERY_PERCENTAGE_UNKNOWN.

	@field BYTE | Reserved1 | Must be zero.

    @field DWORD |  BatteryLifeTime | Number of seconds of battery life 
    remaining, or BATTERY_LIFE_UNKNOWN if remaining seconds are unknown.

    @field DWORD | BatteryFullLifeTime | Number of seconds of battery life when 
    at full charge, or BATTERY_LIFE_UNKNOWN if full lifetime is unknown. 

	@field BYTE | Reserved2 | Must be zero.

    @field BYTE | BackupBatteryFlag | Backup battery charge status.  Must be one 
    of BATTERY_FLAG_HIGH, BATTERY_FLAG_LOW, BATTERY_FLAG_CRITICAL,
	BATTERY_FLAG_CHARGING, BATTERY_FLAG_NO_BATTERY, or BATTERY_FLAG_UNKNOWN.   

    @field BYTE | BackupBatteryLifePercent |  Percentage of full backup battery 
    charge remaining.  Must be in the range 0 to 100, or 
    BATTERY_PERCENTAGE_UNKNOWN. 

	@field BYTE | Reserved3 | Must be zero.

    @field DWORD | BackupBatteryLifeTime | Number of seconds of backup battery 
    life remaining, or BATTERY_LIFE_UNKNOWN if remaining seconds are unknown.

    @field DWORD | BackupBatteryFullLifeTime | Number of seconds of backup 
    battery life when at full charge, or BATTERY_LIFE_UNKNOWN if full lifetime 
    is unknown. 

*/

#define BATTERY_CHEMISTRY_ALKALINE     0x01
#define BATTERY_CHEMISTRY_NICD         0x02
#define BATTERY_CHEMISTRY_NIMH         0x03
#define BATTERY_CHEMISTRY_LION         0x04
#define BATTERY_CHEMISTRY_LIPOLY       0x05
#define BATTERY_CHEMISTRY_ZINCAIR      0x06
#define BATTERY_CHEMISTRY_UNKNOWN      0xFF

typedef struct _SYSTEM_POWER_STATUS_EX2 {
    BYTE ACLineStatus;
    BYTE BatteryFlag;
    BYTE BatteryLifePercent;
    BYTE Reserved1;
    DWORD BatteryLifeTime;
    DWORD BatteryFullLifeTime;
    BYTE Reserved2;
    BYTE BackupBatteryFlag;
    BYTE BackupBatteryLifePercent;
    BYTE Reserved3;
    DWORD BackupBatteryLifeTime;
    DWORD BackupBatteryFullLifeTime;
    // Above here is old struct, below are new fields
    DWORD BatteryVoltage; 				// Reports Reading of battery voltage in millivolts (0..65535 mV)
    DWORD BatteryCurrent;				// Reports Instantaneous current drain (mA). 0..32767 for charge, 0 to -32768 for discharge
    DWORD BatteryAverageCurrent; 		// Reports short term average of device current drain (mA). 0..32767 for charge, 0 to -32768 for discharge
    DWORD BatteryAverageInterval;		// Reports time constant (mS) of integration used in reporting BatteryAverageCurrent	
    DWORD BatterymAHourConsumed; 		// Reports long-term cumulative average DISCHARGE (mAH). Reset by charging or changing the batteries. 0 to 32767 mAH  
    DWORD BatteryTemperature;			// Reports Battery temp in 0.1 degree C (-3276.8 to 3276.7 degrees C)
    DWORD BackupBatteryVoltage;			// Reports Reading of backup battery voltage
    BYTE  BatteryChemistry; 		    // See Chemistry defines above

    // New fields can be added below, but don't change any existing ones
}   SYSTEM_POWER_STATUS_EX2, *PSYSTEM_POWER_STATUS_EX2, *LPSYSTEM_POWER_STATUS_EX2;

// @CESYSGEN IF COREDLL_BATTERY
// @CESYSGEN ENDIF

// @CESYSGEN IF FILESYS_FSPASS
// @CESYSGEN ENDIF

// define dwFlag values for LoadFSDEx function...
#define LOADFSD_ASYNCH				0x0000
#define LOADFSD_SYNCH				0x0001


HANDLE RegisterDevice (LPCWSTR lpszName, DWORD index, LPCWSTR lpszLib, DWORD dwInfo);
BOOL DeregisterDevice (HANDLE hDevice);
BOOL LoadFSD (HANDLE hDevice, LPCWSTR lpFSDName);
BOOL LoadFSDEx (HANDLE hDevice, LPCWSTR lpFSDName, DWORD dwFlag);
HANDLE ActivateDevice(LPCWSTR lpszDevKey, DWORD dwClientInfo);
HANDLE ActivateDeviceEx(LPCWSTR lpszDevKey, LPCVOID lpRegEnts, DWORD cRegEnts, LPVOID lpvParam);
BOOL DeactivateDevice(HANDLE hDevice);
BOOL CeResyncFilesys(HANDLE hDevice);

// This structure is used to obtain information about a device driver.  Some drivers may be
// accessible via multiple names; if a particular name does not exist, the first byte of the
// entry in the structure will be NULL.
typedef struct _DevmgrDeviceInformation_tag {
    DWORD dwSize;                   // size of this structure
    HANDLE hDevice;                 // device handle from ActivateDevice()
    HANDLE hParentDevice;           // parent device's handle from ActivateDevice()
    WCHAR szLegacyName[6];          // e.g, "COM1:"
    WCHAR szDeviceKey[MAX_PATH];    // key path passed to ActivateDevice
    WCHAR szDeviceName[MAX_PATH];   // in $device namespace
    WCHAR szBusName[MAX_PATH];      // in $bus namespace
} DEVMGR_DEVICE_INFORMATION, *PDEVMGR_DEVICE_INFORMATION;

BOOL GetDeviceInformationByDeviceHandle(HANDLE hDevice, PDEVMGR_DEVICE_INFORMATION pdi);
BOOL GetDeviceInformationByFileHandle(HANDLE hFile, PDEVMGR_DEVICE_INFORMATION pdi);

typedef enum {
    DeviceSearchByLegacyName, 
    DeviceSearchByDeviceName, 
    DeviceSearchByBusName, 
    DeviceSearchByGuid, 
    DeviceSearchByParent 
} DeviceSearchType;

HANDLE FindFirstDevice(DeviceSearchType searchType, LPCVOID pvSearchParam, PDEVMGR_DEVICE_INFORMATION pdi);
BOOL FindNextDevice(HANDLE h, PDEVMGR_DEVICE_INFORMATION pdi);

BOOL EnumDeviceInterfaces(HANDLE h, DWORD dwIndex, GUID *pClass, LPWSTR pszNameBuf, LPDWORD lpdwNameBufSize);

// It looks like a registry operation but it's really a device operation
HKEY RegOpenProcessKey(DWORD hPnp);

// This function is also exported by FILESYS
BOOL CeGenRandom(DWORD dwLen, PBYTE pbBuffer);

// @CESYSGEN IF DEVICE_IORM
BOOL ResourceCreateList (DWORD dwResId, DWORD dwMinimum, DWORD dwCount);
BOOL ResourceDestroyList (DWORD dwResId);
BOOL ResourceRequest (DWORD dwResId, DWORD dwId, DWORD dwLen);
BOOL ResourceRequestEx (DWORD dwResId, DWORD dwId, DWORD dwLen, DWORD dwFlags);
BOOL ResourceRelease (DWORD dwResId, DWORD dwId, DWORD dwLen);
BOOL ResourceMarkAsShareable(DWORD dwResId, DWORD dwId, DWORD dwLen, BOOL fShareable);

// flag values for ResourceRequestEx()
#define RREXF_REQUEST_EXCLUSIVE         0x0001
// @CESYSGEN ENDIF

// @CESYSGEN IF FILESYS_FSADVERTISE
HANDLE RequestDeviceNotifications (const GUID *devclass, HANDLE hMsgQ, BOOL fAll);
BOOL StopDeviceNotifications (HANDLE h);
BOOL AdvertiseInterface (const GUID *devclass, LPCWSTR name, BOOL fAdd);
// @CESYSGEN ENDIF

// @CESYSGEN IF FILESYS_MSGQUEUE

// NOTE: DO NOT USE '<msgqueue.h>' to ensure we include the file locally
#include "msgqueue.h"

// @CESYSGEN ENDIF


//
// Extended Name APIs for NTDS
//

typedef enum
{
    // Examples for the following formats assume a fictitous company
    // which hooks into the global X.500 and DNS name spaces as follows.
    //
    // Enterprise root domain in DNS is
    //
    //      widget.com
    //
    // Enterprise root domain in X.500 (RFC 1779 format) is
    //
    //      O=Widget, C=US
    //
    // There exists the child domain
    //
    //      engineering.widget.com
    //
    // equivalent to
    //
    //      OU=Engineering, O=Widget, C=US
    //
    // There exists a container within the Engineering domain
    //
    //      OU=Software, OU=Engineering, O=Widget, C=US
    //
    // There exists the user
    //
    //      CN=Spencer Katt, OU=Software, OU=Engineering, O=Widget, C=US
    //
    // And this user's downlevel (pre-NTDS) user name is
    //
    //      Engineering\SpencerK

    // unknown name type
    NameUnknown = 0,

    // CN=Spencer Katt, OU=Software, OU=Engineering, O=Widget, C=US
    NameFullyQualifiedDN = 1,

    // Engineering\SpencerK
    NameSamCompatible = 2,

    // Probably "Spencer Katt" but could be something else.  I.e. The
    // display name is not necessarily the defining RDN.
    NameDisplay = 3,

    // xxx@engineering.widget.com where xxx could be "SpencerK" or
    // anything else.  Could be multi-valued to handle migration and aliasing.
    NameDomainSimple = 4,

    // xxx@widget.com where xxx could be "SpencerK" or anything else.
    // Could be multi-valued to handle migration and aliasing.
    NameEnterpriseSimple = 5,

    // String-ized GUID as returned by IIDFromString().
    // eg: {4fa050f0-f561-11cf-bdd9-00aa003a77b6}
    NameUniqueId = 6,

    // engineering.widget.com/software/spencer katt
    NameCanonical = 7,

    // local logon name
    NameWindowsCeLocal = 0x80000001

} EXTENDED_NAME_FORMAT, * PEXTENDED_NAME_FORMAT ;

BOOLEAN
WINAPI
GetUserNameExA(
    EXTENDED_NAME_FORMAT  NameFormat,
    LPSTR lpNameBuffer,
    PULONG nSize
    );

BOOLEAN
WINAPI
GetUserNameExW(
    EXTENDED_NAME_FORMAT NameFormat,
    LPWSTR lpNameBuffer,
    PULONG nSize
    );

//
// Event logging APIs
//

BOOL WINAPI ReportEventW(HANDLE hEventLog, WORD wType, WORD wCategory, DWORD dwEventID,
                 PSID lpUserSid, WORD wNumStrings, DWORD dwDataSize, 
                 LPCWSTR* lpStrings, LPVOID lpRawData);

HANDLE WINAPI RegisterEventSourceW(LPCWSTR lpUNCServerName,  LPCWSTR lpSourceName);

BOOL WINAPI DeregisterEventSource(HANDLE hEventLog);
BOOL WINAPI ClearEventLogW(HANDLE hEventLog, LPCWSTR lpBackupFileName);

HANDLE WINAPI OpenEventLogW(LPCTSTR lpUNCServerName, LPCTSTR lpSourceName);
BOOL WINAPI CloseEventLog(HANDLE hEventLog);
BOOL WINAPI BackupEventLogW(HANDLE hEventLog, LPCTSTR szBackupFileName);
BOOL WINAPI LockEventLog(HANDLE hEventLog);
BOOL WINAPI UnLockEventLog(HANDLE hEventLog);
BOOL WINAPI ReadEventLogRaw(HANDLE hEventLog, BYTE *pReadBuffer, DWORD dwReadBufferSize, DWORD *pdwBytesRead);

#define ReportEvent           ReportEventW
#define RegisterEventSource   RegisterEventSourceW
#define ClearEventLog         ClearEventLogW
#define OpenEventLog          OpenEventLogW
#define BackupEventLog        BackupEventLogW


#ifdef UNICODE
#define GetUserNameEx   GetUserNameExW
#else
#define GetUserNameEx   GetUserNameExA
#endif

#include <winnls.h>

#ifdef UNDER_CE
#ifdef WINCEOEM
#include <pwinbase.h>	// internal defines
#ifdef WINCEMACRO
#include <mwinbase.h>
#endif
#endif
#include <windbase.h>
#endif

#ifdef UNDER_CE
// special multi-language redirection resource previously defined in oak\inc\pwinbase.h
#define RT_MUI  222 // type of the redirection resource
#define ID_MUI  1   // ID of the redirection resource
// the redirection resource is a wide-string, whose value is the basename of the resource DLL
#endif

#ifdef __cplusplus
}
#endif

#endif /* __WINBASE_H__ */
