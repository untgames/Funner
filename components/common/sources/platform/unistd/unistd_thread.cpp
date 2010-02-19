#include <pthread.h>

#include <platform/platform.h>

using namespace common;

/*
    Получение идентификатора текущей нити
*/
   
UnistdPlatform::threadid_t UnistdPlatform::GetCurrentThreadId ()
{
  return (UnistdPlatform::threadid_t)pthread_self ();
}
