#ifndef INC_CPUCOUNT_H
#define INC_CPUCOUNT_H

#include "types.h"

uint8 CPUCount(uint32 * logical, uint32 * core, uint32 * physical);
uint32 MaxLogicalProcPerPhysicalProc(void);

#endif
