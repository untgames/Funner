#include <pthread.h>

#include <xtl/common_exceptions.h>

#include <common/strlib.h>

#include <platform/platform.h>

using namespace common;

namespace
{

//мьютекс
struct Mutex
{
  pthread_mutex_t handle;
};

//генерация исключения с кодом ошибки
void pthread_raise_error (const char* source, int status)
{
  throw xtl::format_operation_exception (source, "Operation failed. Reason: %s (function exit with code %d)", common::strerror (status), status);
}

}

/*
    Работа с блокировками
*/

void UnistdPlatform::InitLockable (lockable_t& lockable)
{
  try
  {
    Mutex* mutex = (Mutex*)malloc (sizeof (Mutex));

    if (!mutex)
      throw xtl::format_operation_exception ("", "No memory");

    pthread_mutexattr_t attributes;

    pthread_mutexattr_init (&attributes);

    int status = pthread_mutexattr_settype (&attributes, PTHREAD_MUTEX_RECURSIVE);

    if (status)
    {
      free (mutex);
      pthread_raise_error ("::pthread_mutexattr_init", status);
    }

    status = pthread_mutex_init (&mutex->handle, 0);

    pthread_mutexattr_destroy (&attributes);

    if (status)
    {
      free (mutex);
      pthread_raise_error ("::pthread_mutex_init", status);
    }

    lockable.data = mutex;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::UnistdPlatform::CreateLockable");
    throw;
  }
}

void UnistdPlatform::DestroyLockable (lockable_t& lockable)
{
  try
  {
    if (!lockable.data)
      throw xtl::make_null_argument_exception ("", "lockable");

    Mutex* mutex = (Mutex*)lockable.data;

    int status = pthread_mutex_destroy (&mutex->handle);

    if (status)
      pthread_raise_error ("::pthread_mutex_destroy", status);

    free (lockable.data);

    lockable.data = 0;
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::UnistdPlatform::DestroyLockable");
    throw;
  }
}

void UnistdPlatform::Lock (lockable_t& lockable)
{
  try
  {
    if (!lockable.data)
      throw xtl::make_null_argument_exception ("", "lockable");

    Mutex* mutex = (Mutex*)lockable.data;

    int status = pthread_mutex_lock (&mutex->handle);

    if (status)
      pthread_raise_error ("::pthread_mutex_lock", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::UnistdPlatform::Lock");
    throw;
  }
}

void UnistdPlatform::Unlock (lockable_t& lockable)
{
  try
  {
    if (!lockable.data)
      throw xtl::make_null_argument_exception ("", "lockable");

    Mutex* mutex = (Mutex*)lockable.data;

    int status = pthread_mutex_unlock (&mutex->handle);

    if (status)
      pthread_raise_error ("::pthread_mutex_unlock", status);
  }
  catch (xtl::exception& exception)
  {
    exception.touch ("common::UnistdPlatform::Unlock");
    throw;
  }
}
