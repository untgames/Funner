#include <common/singleton.h>

#include <platform/platform.h>
#include <platform/pthreads/public.h>

using namespace common;

ICustomThreadSystem* UnistdPlatform::GetThreadSystem ()
{
  return Singleton<PThreadsSystem>::InstancePtr ();
}
