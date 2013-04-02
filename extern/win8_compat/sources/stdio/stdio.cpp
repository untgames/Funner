#include <stdio.h>

#undef vprintf

extern "C"
{

int win8_vprintf (const char *format, va_list list)
{
  return vprintf (format, list);
}

}
