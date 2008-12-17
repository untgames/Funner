#include <common/singleton.h>

#include <platform/platform.h>
#include <platform/pthreads/public.h>

using namespace common;

ICustomThreadSystem* Win32Platform::GetThreadSystem ()
{
  return Singleton<PThreadsSystem>::InstancePtr ();
}
