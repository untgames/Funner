#include <pthread.h>

#include <platform/platform.h>

using namespace common;

/*
    Получение идентификатора текущей нити
*/
   
UnistdPlatform::threadid_t UnistdPlatform::GetCurrentThreadId ()
{
  return (threadid_t)::GetCurrentThreadId ();
}
