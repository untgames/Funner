#if defined (_MSC_VER) || defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
  #if defined(_MSC_VER) && !defined(_WIN32_WCE)

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
  #endif

  inline int atomic_increment (volatile int& rc)
  {      
    return XTL_INTERLOCKED_INCREMENT ((volatile long*)&rc) - 1;
  }

  inline int atomic_decrement (volatile int& rc)
  {
    return XTL_INTERLOCKED_DECREMENT ((volatile long*)&rc) + 1;
  }

  inline int atomic_compare_and_swap (volatile int& value, int old_value, int new_value)
  {
    return XTL_INTERLOCKED_COMPARE_EXCHANGE ((volatile long*)&value, new_value, old_value);
  } 

#elif defined (__GNUC__) && (defined( __i386__) || defined (__x86_64__))

  /*
      from boost
  */
  
  inline int atomic_increment (volatile int& rc)
  {
    int r;

    asm volatile
    (
       "lock\n\t"
       "xadd %1, %0":
       "+m"( rc ), "=r"( r ): // outputs (%0, %1)
       "1"( 1 ): // inputs (%2 == %1)
       "memory", "cc" // clobbers
    );

    return r;
  }

  inline int atomic_compare_and_swap (volatile int& value, int old_value, int new_value)
  {
    int prev = old_value;    

    __asm__ volatile
    (
      "lock\n\t"
      "cmpxchg %3,%1"
      : "=a" (prev), "=m" (*(&value))
      : "0" (prev), "r" (new_value)
      : "memory", "cc"
    );

    return prev;
  }  

  inline int atomic_decrement (volatile int& rc)
  {
    return atomic_increment (rc, -1);
  }
  
#elif defined (ARM)

#if defined (BEAGLEBOARD) || defined (TABLETOS)

inline int __sync_fetch_and_add (volatile int *ptr, volatile int val)
{
  unsigned long tmp;
  int result;

  __asm__ __volatile__("@ atomic_add\n"
  "1: ldrex %0, [%3]\n"
  " add %0, %0, %4\n"
  " strex %1, %0, [%3]\n"
  " teq %1, #0\n"
  " bne 1b"
  : "=&r" (result), "=&r" (tmp), "+Qo" (*ptr)
  : "r" (ptr), "Ir" (val)
  : "cc");
  
  return result - val;
}
  
inline int __sync_fetch_and_sub (volatile int *ptr, volatile int val)
{
  unsigned long tmp;
  int result;

  __asm__ __volatile__("@ atomic_add\n"
  "1: ldrex %0, [%3]\n"
  " add %0, %0, %4\n"
  " strex %1, %0, [%3]\n"
  " teq %1, #0\n"
  " bne 1b"
  : "=&r" (result), "=&r" (tmp), "+Qo" (*ptr)
  : "r" (ptr), "Ir" (-val)
  : "cc");
  
  return result + val;
}
  
inline int __sync_val_compare_and_swap (volatile int *ptr, volatile int _old, volatile int _new)
{
  unsigned long oldval, res;

  __asm__ __volatile__ ("dmb" : : : "memory");

  do {
    __asm__ __volatile__("@ atomic_cmpxchg\n"
    "ldrex  %1, [%3]\n"
    "mov  %0, #0\n"
    "teq  %1, %4\n"
    "strexeq %0, %5, [%3]\n"
        : "=&r" (res), "=&r" (oldval), "+Qo" (*ptr)
        : "r" (ptr), "Ir" (_old), "r" (_new)
        : "cc");
  } while (res);

  __asm__ __volatile__ ("dmb" : : : "memory");  

  return oldval;
}

#elif defined (ANDROID)

//from http://gcc.gnu.org/ml/gcc-patches/2008-07/msg00025.html

namespace detail
{

typedef int (__kernel_cmpxchg_t) (volatile int oldval, volatile int newval, volatile int *ptr);

#define __kernel_cmpxchg (*(detail::__kernel_cmpxchg_t *) 0xffff0fc0)

}

inline int __sync_fetch_and_add (volatile int *ptr, volatile int val)
{
  volatile int failure, tmp;

  do
  {
    tmp     = *ptr;
    failure = __kernel_cmpxchg (tmp, tmp + val, ptr);
  } while (failure);

  return tmp;
}
  
inline int __sync_fetch_and_sub (volatile int *ptr, volatile int val)
{
  return __sync_fetch_and_add (ptr, -val);
}
  
inline int __sync_val_compare_and_swap (volatile int *ptr, volatile int oldval, volatile int newval)
{
  volatile int actual_oldval, fail;
    
  for (;;)
  {
    actual_oldval = *ptr;

    if (oldval != actual_oldval)
      return actual_oldval;

    fail = __kernel_cmpxchg (actual_oldval, newval, ptr);

    if (!fail)
      return oldval;
  }
}

#undef __kernel_cmpxchg

#endif

inline int atomic_increment (volatile int& rc)
{
  return __sync_fetch_and_add (&rc, 1);
}

inline int atomic_decrement (volatile int& rc)
{
  return __sync_fetch_and_sub (&rc, 1);
}

#endif

inline int atomic_conditional_increment (volatile int& rc)
{
  for (;;)
  {
    int tmp = rc;
    
    if (tmp == 0)
      return tmp;

    if (atomic_compare_and_swap (rc, tmp, tmp + 1) == tmp)
      return tmp;
  }
}

inline int atomic_conditional_decrement (volatile int& rc)
{
  for (;;)
  {
    int tmp = rc;
    
    if (tmp == 0)
      return tmp;

    if (atomic_compare_and_swap (rc, tmp, tmp - 1) == tmp)
      return tmp;
  }  
}

inline int atomic_increment (volatile int& rc, int value)
{
  for (;;)
  {
    int tmp = rc;
    
    if (atomic_compare_and_swap (rc, tmp, tmp + value) == tmp)
      return tmp;
  }
}

inline int atomic_decrement (volatile int& rc, int value)
{
  for (;;)
  {
    int tmp = rc;
    
    if (atomic_compare_and_swap (rc, tmp, tmp - value) == tmp)
      return tmp;
  }  
}  
