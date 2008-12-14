#ifndef XTL_INTERLOCKED_API_HEADER
#define XTL_INTERLOCKED_API_HEADER

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__) || defined (__GNUC__) && (defined( __i386__) || defined (__x86_64__))
  #define XTL_HAS_INTERLOCKED
#endif

/*
    Функции для поддержки многопоточного подсчёта ссылок
*/

namespace xtl
{

#ifdef XTL_HAS_INTERLOCKED

int atomic_increment             (volatile int& rc); //int r = rc; ++rc; return r;
int atomic_decrement             (volatile int& rc); //int r = rc; --rc; return r;
int atomic_conditional_increment (volatile int& rc); //int r = rc; if (r) ++rc; return r;
int atomic_conditional_decrement (volatile int& rc); //int r = rc; if (r) --rc; return r;

#include <xtl/detail/interlocked.inl>

#endif

}

#endif
