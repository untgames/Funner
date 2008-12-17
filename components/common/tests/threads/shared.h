#include <cstdio>

#include <xtl/function.h>

#include <common/log.h>
#include <common/thread.h>

using namespace common;

void print_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

