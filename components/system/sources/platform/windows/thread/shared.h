#ifndef SYSLIB_WIN32_PLATFORM_THREAD_SHARED_HEADER
#define SYSLIB_WIN32_PLATFORM_THREAD_SHARED_HEADER

#include "../shared.h"

namespace syslib
{

struct mutex_handle
{
  HANDLE mutex;
};

}

#endif
