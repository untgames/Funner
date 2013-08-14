#ifndef SYSLIB_THREAD_TESTS_SHARED_HEADER
#define SYSLIB_THREAD_TESTS_SHARED_HEADER

#include <cstdio>

#include <stl/algorithm>
#include <stl/string>
#include <stl/vector>

#include <xtl/bind.h>
#include <xtl/common_exceptions.h>
#include <xtl/function.h>
#include <xtl/high_precision_time.h>
#include <xtl/shared_ptr.h>

#include <common/action_queue.h>
#include <common/async.h>
#include <common/lockable.h>
#include <common/log.h>
#include <common/time.h>

#include <syslib/application.h>
#include <syslib/condition.h>
#include <syslib/mutex.h>
#include <syslib/semaphore.h>
#include <syslib/shared_queue.h>
#include <syslib/thread.h>
#include <syslib/tls.h>

using namespace syslib;
using namespace common;

void print_log (const char* log, const char* message)
{
  printf ("%s: %s\n", log, message);
  fflush (stdout);
}

#endif
