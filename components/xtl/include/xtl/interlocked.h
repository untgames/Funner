#ifndef XTL_INTERLOCKED_API_HEADER
#define XTL_INTERLOCKED_API_HEADER

#if defined (WIN32) || defined (_WIN32) || defined (__WIN32__) || defined (__GNUC__) && (defined( __i386__) || defined (__x86_64__))
  #define XTL_HAS_INTERLOCKED
#elif defined (ARM)
  #define XTL_HAS_INTERLOCKED
#elif defined (BADA)
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
int atomic_increment             (volatile int& rc, int value); //int r = rc; rc += value; return r;
int atomic_decrement             (volatile int& rc, int value); //int r = rc; rc -= value; return r;
int atomic_compare_and_swap      (volatile int& value, int old_value, int new_value); //if (value == old_value) value = new_value; return old_value;

#include <xtl/detail/interlocked.inl>

#endif

}

#endif
