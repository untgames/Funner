#ifndef __EGLIB_CONFIG_HW
#define __EGLIB_CONFIG_HW

#ifdef _MSC_VER
#include "eglib-config.hw"
#elif defined (__MINGW32__)

#include <io.h>
#include <stdint.h>
#include <stdio.h>
#include <windows.h>

#define G_OS_WIN32 1

#define G_GNUC_PRETTY_FUNCTION   __PRETTY_FUNCTION__
#define G_GNUC_UNUSED            
#define G_BYTE_ORDER             1234
#define G_GNUC_NORETURN          
#define G_BREAKPOINT()
#define MAXPATHLEN 242

typedef uintptr_t gsize;
typedef intptr_t gssize;
typedef int pid_t;

#define G_DIR_SEPARATOR          '/'
#define G_DIR_SEPARATOR_S        "/"
#define G_SEARCHPATH_SEPARATOR_S ":"
#define G_SEARCHPATH_SEPARATOR   ':'
#define G_GSIZE_FORMAT   "d"
#define G_GUINT64_FORMAT "d"
#define G_GINT64_FORMAT "d"
#define GPOINTER_TO_INT(ptr)   ((gint)(intptr_t) (ptr))
#define GPOINTER_TO_UINT(ptr)  ((guint)(intptr_t) (ptr))
#define GINT_TO_POINTER(v)     ((gpointer)(intptr_t) (v))
#define GUINT_TO_POINTER(v)    ((gpointer)(intptr_t) (v))

//#define INT32_MAX 2147483647
//#define INT32_MIN (~ INT32_MAX)
//#define INT64_MAX 9223372036854775807i64
//#define INT64_MIN (~INT64_MAX)
//#define UINT32_MAX 0xffffffffU
//#define UINT64_MAX 0xffffffffffffffffULL

//#define STDOUT_FILENO (int)(intptr_t)stdout
//#define STDERR_FILENO (int)(intptr_t)stderr


/* FIXME: what should this be ?*/
//#define X_OK 4 /* This is really read */
#define WNOHANG 1
#define F_SETFD 1
#define FD_CLOEXEC 1

#undef inline
#define inline __inline

#define strtok_r strtok_s

#undef G_HAVE_UNISTD_H
#undef G_HAVE_SYS_TIME_H
#undef G_HAVE_SYS_WAIT_H
#undef G_HAVE_PWD_H
#undef G_HAVE_STRNDUP
#define G_HAVE_GETOPT_H 1

#else
#error Unknown compiler
#endif

#endif
