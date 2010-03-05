#include <stdio.h>
#include <stdarg.h>

#include <FBaseSys.h>

_EXPORT_ int OspMain (int argc, char *argv [])
{
  return main (argc, argv);
}

int printf (const char* format, ...)
{
  AppLog ("HELLO");
  
  return 1;
}

