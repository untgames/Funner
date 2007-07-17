#ifndef XTL_INTERLOCKED_API_HEADER
#define XTL_INTERLOCKED_API_HEADER

/*
    Функции для поддержки многопоточного shared_ptr
*/

#ifdef _MSC_VER

extern "C" long __cdecl _InterlockedIncrement       (volatile long*);
extern "C" long __cdecl _InterlockedDecrement       (volatile long*);
extern "C" long __cdecl _InterlockedCompareExchange (volatile long*, long, long);

#pragma intrinsic (_InterlockedIncrement)
#pragma intrinsic (_InterlockedDecrement)
#pragma intrinsic (_InterlockedCompareExchange)

#define XTL_INTERLOCKED_INCREMENT        _InterlockedIncrement
#define XTL_INTERLOCKED_DECREMENT        _InterlockedDecrement
#define XTL_INTERLOCKED_COMPARE_EXCHANGE _InterlockedCompareExchange

#elif defined (WIN32) || defined (_WIN32) || defined (__WIN32__)

extern "C" __declspec (dllimport) long __stdcall InterlockedIncrement       (volatile long*);
extern "C" __declspec (dllimport) long __stdcall InterlockedDecrement       (volatile long*);
extern "C" __declspec (dllimport) long __stdcall InterlockedCompareExchange (volatile long*, long, long);

#define XTL_INTERLOCKED_INCREMENT        InterlockedIncrement
#define XTL_INTERLOCKED_DECREMENT        InterlockedDecrement
#define XTL_INTERLOCKED_COMPARE_EXCHANGE InterlockedCompareExchange

#else
  #error "Interlocked intrinsics not available"
#endif

#endif
