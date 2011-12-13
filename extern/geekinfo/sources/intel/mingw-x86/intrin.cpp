#include "../cputopology.h"
#include "intrin.h"

void get_cpuid_info(CPUIDinfo * info, const unsigned int func, const unsigned int subfunc)
{
  int _eax = 0, _ebx = 0, _ecx = 0, _edx = 0;
  
  asm ("cpuid"
              : "=a" (_eax),
                "=b" (_ebx),
                "=c" (_ecx),
                "=d" (_edx)
              : "a" (func),
                "b" (subfunc));
              
  info->EAX = _eax;
  info->EBX = _ebx;
  info->ECX = _ecx;
  info->EDX = _edx;
}
