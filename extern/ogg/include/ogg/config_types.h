#ifndef __CONFIG_TYPES_H__
#define __CONFIG_TYPES_H__

#ifdef ARM9

typedef short          ogg_int16_t;
typedef unsigned short ogg_uint16_t;
typedef int            ogg_int32_t;
typedef unsigned int   ogg_uint32_t;
typedef long long      ogg_int64_t;

#elif defined (__CYGWIN__)

#include <sys/types.h>

typedef int16_t   ogg_int16_t;
typedef uint16_t  ogg_uint16_t;
typedef int32_t   ogg_int32_t;
typedef uint32_t  ogg_uint32_t;
typedef int64_t   ogg_int64_t;

#elif defined (__APPLE__)

#include <sys/types.h>

typedef int16_t   ogg_int16_t;
typedef __uint16_t  ogg_uint16_t;
typedef int32_t   ogg_int32_t;
typedef __uint32_t  ogg_uint32_t;
typedef int64_t   ogg_int64_t;

#elif defined (__linux__)

#include <sys/types.h>

typedef int16_t   ogg_int16_t;
typedef __uint16_t  ogg_uint16_t;
typedef int32_t   ogg_int32_t;
typedef __uint32_t  ogg_uint32_t;
typedef int64_t   ogg_int64_t;

#elif defined (__ARM_ARCH_5__)

typedef short          ogg_int16_t;
typedef unsigned short ogg_uint16_t;
typedef int            ogg_int32_t;
typedef unsigned int   ogg_uint32_t;
typedef long long      ogg_int64_t;

#else

#error Unknown compiler

#endif

#endif
