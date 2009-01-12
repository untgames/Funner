#ifndef SYSLIB_THREAD_TESTS_SHARED_HEADER
#define SYSLIB_THREAD_TESTS_SHARED_HEADER

#include <cstdio>

#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/shared_ptr.h>

#include <common/lockable.h>
#include <common/log.h>
#include <common/time.h>

#include <syslib/mutex.h>
#include <syslib/semaphore.h>
#include <syslib/thread.h>

using namespace syslib;
using namespace common;

void print_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
}

#endif
