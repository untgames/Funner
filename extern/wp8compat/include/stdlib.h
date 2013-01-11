#ifndef WP8_COMPATIBILITY_STDLIB_H_HEADER
#define WP8_COMPATIBILITY_STDLIB_H_HEADER

#include <../../WP80/include/stdlib.h>

#ifdef getenv
#undef getenv
#endif

__inline char* getenv (const char* name) { return 0; }

__inline int putenv (const char* string) { return 1; }

#endif
