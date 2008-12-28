#ifndef COMMONLIB_THREAD_TESTS_SHARED_HEADER
#define COMMONLIB_THREAD_TESTS_SHARED_HEADER

#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/shared_ptr.h>

#include <common/log.h>
#include <common/time.h>
#include <common/thread.h>

using namespace common;

void print_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

#endif
