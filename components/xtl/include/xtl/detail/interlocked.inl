#if defined (_MSC_VER) || defined (WIN32) || defined (_WIN32) || defined (__WIN32__)
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
  #endif

  inline int atomic_increment (volatile int& rc)
  {      
    return XTL_INTERLOCKED_INCREMENT ((volatile long*)&rc) - 1;
  }

  inline int atomic_decrement (volatile int& rc)
  {
    return XTL_INTERLOCKED_DECREMENT ((volatile long*)&rc) + 1;
  }
  
  inline int atomic_conditional_increment (volatile int& rc)
  {
    for (;;)
    {
      long tmp = *(const volatile long*)&rc;
      
      if (tmp == 0)
        return tmp;

      if (XTL_INTERLOCKED_COMPARE_EXCHANGE ((volatile long*)&rc, tmp + 1, tmp) == tmp)
        return tmp;
    }
  }
  
  inline int atomic_conditional_decrement (volatile int& rc)
  {
    for (;;)
    {
      long tmp = *(const volatile long*)&rc;
      
      if (tmp == 0)
        return tmp;

      if (XTL_INTERLOCKED_COMPARE_EXCHANGE ((volatile long*)&rc, tmp - 1, tmp) == tmp)
        return tmp;
    }  
  }

#elif defined (__GNUC__) && (defined( __i386__) || defined (__x86_64__))

  /*
      from boost
  */
  
  inline int atomic_increment (volatile int& rc)
  {
    int r;

    __asm__ __volatile__
    (
        "lock\n\t"
        "xadd %1, %0":
        "=m"( rc ), "=r"( r ): // outputs (%0, %1)
        "m"( rc ), "1"( 1 ): // inputs (%2, %3 == %1)
        "memory", "cc" // clobbers
    );

    return r;  
  }  

  inline int atomic_decrement (volatile int& rc)
  {
    int r;

    __asm__ __volatile__
    (
        "lock\n\t"
        "xadd %1, %0":
        "=m"( rc ), "=r"( r ): // outputs (%0, %1)
        "m"( rc ), "1"( -1 ): // inputs (%2, %3 == %1)
        "memory", "cc" // clobbers
    );

    return r;
  }

  inline int atomic_conditional_increment (volatile int& rc)
  {
      int rv, tmp;

      __asm__
      (
          "movl %0, %%eax\n\t"
          "0:\n\t"
          "test %%eax, %%eax\n\t"
          "je 1f\n\t"
          "movl %%eax, %2\n\t"
          "incl %2\n\t"
          "lock\n\t"
          "cmpxchgl %2, %0\n\t"
          "jne 0b\n\t"
          "1:":
          "=m"( rc ), "=&a"( rv ), "=&r"( tmp ): // outputs (%0, %1, %2)
          "m"( rc ): // input (%3)
          "cc" // clobbers
      );

      return rv;
  }

  inline int atomic_conditional_decrement (volatile int& rc)
  {
      int rv, tmp;

      __asm__
      (
          "movl %0, %%eax\n\t"
          "0:\n\t"
          "test %%eax, %%eax\n\t"
          "je 1f\n\t"
          "movl %%eax, %2\n\t"
          "decl %2\n\t"
          "lock\n\t"
          "cmpxchgl %2, %0\n\t"
          "jne 0b\n\t"
          "1:":
          "=m"( rc ), "=&a"( rv ), "=&r"( tmp ): // outputs (%0, %1, %2)
          "m"( rc ): // input (%3)
          "cc" // clobbers
      );

      return rv;
  }
  
#elif defined (ARM)

#if defined (BEAGLEBOARD)

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

inline int atomic_conditional_increment (volatile int& rc)
{
  for (;;)
  {
    int tmp = rc;
    
    if (tmp == 0)
      return tmp;

    if (__sync_val_compare_and_swap (&rc, tmp, tmp + 1) == tmp)
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

    if (__sync_val_compare_and_swap (&rc, tmp, tmp - 1) == tmp)
      return tmp;
  }
}

#endif
