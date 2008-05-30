#ifndef XTL_STRING_HEADER
#define XTL_STRING_HEADER

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

namespace xtl
{

/*
    Сравнение строк
*/

int xstrcmp   (const char* s1, const char* s2);
int xstricmp  (const char* s1, const char* s2);
int xstrncmp  (const char* s1, const char* s2, size_t max_count);
int xstrnicmp (const char* s1, const char* s2, size_t max_count);

/*
    Печать в строку:
      - если размер буфера недостаточен для печати строки возвращается -1;
      - count - максимальное количество записываемых символов включая '\0';
      - если buffer == NULL && count == 0, то возвращается размер форматирования.
*/

int xsnprintf   (char* buffer, size_t count, const char* format, ...);
int xvsnprintf  (char* buffer, size_t count, const char* format, va_list list);

#include <xtl/detail/string.inl>

}

#endif
