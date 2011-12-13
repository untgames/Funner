/*
	cpucount_asm.h

    Copyright 2007 Xo W.

    Free to use as anyone likes...
*/

#ifndef INC_CPUCOUNT_ASM_H
#define INC_CPUCOUNT_ASM_H

extern "C"
{
	int   __MaxCorePerPhysicalProc__(void);
}

extern "C"
{
	int   __find_maskwidth__(int);
}

#endif
