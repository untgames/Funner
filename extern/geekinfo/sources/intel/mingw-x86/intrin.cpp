#include "../cputopology.h"
#include "intrin.h"

void get_cpuid_info(CPUIDinfo * info, const unsigned int func, const unsigned int subfunc)
{
  if (subfunc == 0)
  {
    __cpuid ((int*)info, func);
  }
}
