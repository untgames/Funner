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
  
#elif defined (IPHONE)

inline int atomic_increment (volatile int& rc)
{
  int r = rc;

  ++rc;

  return r;
}

inline int atomic_decrement (volatile int& rc)
{
  int r = rc;
  
  --rc;
  
  return r;
}

inline int atomic_conditional_increment (volatile int& rc)
{
  int r = rc;
  
  if (r) ++rc;
  
  return r;  
}

inline int atomic_conditional_decrement (volatile int& rc)
{
  int r = rc;
  
  if (r) --rc;
  
  return r;
}

#endif
