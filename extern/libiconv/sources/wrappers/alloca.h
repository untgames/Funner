#ifdef _MSC_VER

#ifndef _ALLOCA_H_
#define _ALLOCA_H_     

#include <malloc.h>

#define alloca _alloca

#endif

#else

#include_next <alloca.h.

#endif
