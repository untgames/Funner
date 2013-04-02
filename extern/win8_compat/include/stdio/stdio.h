#pragma once

#include <stdarg.h>

#ifdef WP8
#include <../../WP80/include/stdio.h>
#else
#include <../../VC/include/stdio.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

int win8_vprintf (const char* format, va_list list);

__inline int win8_printf (const char* format, ...)
{
  va_list list;  

  va_start (list, format);

  return win8_vprintf (format, list);
}

#define printf  win8_printf
#define vprintf win8_vprintf

#ifdef __cplusplus
}
#endif
