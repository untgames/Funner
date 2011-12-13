#ifndef MINGW_INTRIN_WRAPPER_HEADER
#define MINGW_INTRIN_WRAPPER_HEADER

inline void __cpuid (int a[4], int op)
{
  int _eax = 0, _ebx = 0, _ecx = 0, _edx = 0;
  
  asm ("cpuid"
              : "=a" (_eax),
                "=b" (_ebx),
                "=c" (_ecx),
                "=d" (_edx)
              : "a" (op));
              
  a [0] = _eax;
  a [1] = _ebx;
  a [2] = _ecx;
  a [3] = _edx;
}

#endif
